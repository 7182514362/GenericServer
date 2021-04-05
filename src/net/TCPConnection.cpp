#include "TCPConnection.h"
#include "Log.h"
#include "InetAddress.h"
#include "Event.h"
#include "EventLoop.h"
#include "Buffer.h"
#include "Socket.h"
#include "IdleConnections.h"
#include "Timer.h"

using namespace generic;

TCPConnection::TCPConnection(int fd, InetAddress &addr, EventLoopPtr loop)
    : m_local(SocketOP::getLocalAddr(fd)),
      m_peer(addr),
      m_loop(loop),
      m_event(fd),
      m_rbuf(std::make_shared<Buffer>(BUFFER_SIZE)),
      m_wbuf(std::make_shared<Buffer>(BUFFER_SIZE)),
      m_entry(),
      m_timerList()
{
    m_event.setReadCallback(std::bind(&TCPConnection::handleRead, this));
    m_event.setWriteCallback(std::bind(&TCPConnection::handleWrite, this));
    m_event.setCloseCallback(std::bind(&TCPConnection::handleClose, this));
    m_event.setErrorCallback(std::bind(&TCPConnection::handleError, this));

    m_event.enableRead();
}

void TCPConnection::init()
{
    m_loop->runInLoop(
        [this]() {
            if (m_connectedCallback)
            {
                m_connectedCallback(shared_from_this());
            }
            m_loop->addEvent(&m_event);
        });
}

void TCPConnection::shutdown()
{
    LOG_TRACE << "TCPConnection::shutdown";
    stopPollEvent();
    if (m_closeCallback)
        m_closeCallback(shared_from_this());
    SocketOP::close(getFd());
}

void TCPConnection::startPollEvent()
{
    m_loop->addEvent(&m_event);
}

void TCPConnection::stopPollEvent()
{
    m_loop->removeEvent(&m_event);
}

int TCPConnection::getFd() const
{
    return m_event.fd();
}

void TCPConnection::sendMsg(const char *msg, int len)
{
    LOG_TRACE << "TCPConnection::sendMsg";
    {
        std::lock_guard<std::mutex> guard(m_wlock);
        int n = m_wbuf->append(msg, len);
        if (n < len)
        {
            LOG_INFO << "TCPConnection::sendMsg(): write buffer full";
        }
    }

    m_event.enableWrite(true);
    m_loop->modifyEvent(&m_event);
}

void TCPConnection::addTask(std::function<void()> f, int ms, bool repeat)
{
    std::shared_ptr<TimerEntry> timer = std::make_shared<TimerEntry>(f, ms, repeat);
    m_loop->addTimer(timer);
    m_timerList.push_back(timer);
}

void TCPConnection::handleRead()
{
    LOG_TRACE << "TCPConnection::handleRead";

    // no need to guard m_rbuf, m_rbuf only accessed in m_loop's thread
    // std::lock_guard<std::mutex> guard(m_rlock);
    int n = SocketOP::recv(m_event.fd(), m_rbuf.get());
    if (n == 0 || errno == ECONNRESET)
    {
        handleClose();
        return;
    }

    assert(m_msgCallback != nullptr);
    m_msgCallback(shared_from_this());
    m_rbuf->clear();
}

void TCPConnection::handleWrite()
{
    LOG_TRACE << "TCPConnection::handleWrite";

    std::lock_guard<std::mutex> guard(m_wlock);
    int size = m_wbuf->data_size();
    if (size > 0)
    {
        int n = SocketOP::send(m_event.fd(), m_wbuf.get());
        if (n < size && n > 0)
        {
            LOG_TRACE << "send " << n << " bytes, less than " << size;
            //m_event.enableWrite(true);
        }
        else
        {
            m_event.enableWrite(false);
            m_loop->modifyEvent(&m_event);
        }
    }
    else
    {
        LOG_ERROR << "TCPConnection::handleWrite() no data to write!";
        m_event.enableWrite(false);
        m_loop->modifyEvent(&m_event);
    }
}

void TCPConnection::handleClose()
{
    LOG_TRACE << "TCPConnection::handleClose";
    // clear revents !!!
    m_event.set_revents(0);
    m_loop->removeEvent(&m_event);
    m_event.close();
    // disable all timers
    for (auto &timer : m_timerList)
    {
        std::shared_ptr<TimerEntry> ptr = timer.lock();
        if (ptr)
            ptr->disable();
    }
    m_timerList.clear();

    assert(m_closeCallback != nullptr);
    m_closeCallback(shared_from_this());
}

void TCPConnection::handleError()
{
    LOG_TRACE << "TCPConnection::handleError";
    handleClose();
}

void TCPConnection::addToLoop()
{
    LOG_TRACE << "TCPConnection::addToLoop";
    m_loop->addEvent(&m_event);
}

bool TCPConnection::operator==(const TCPConnection &conn) const
{
    return m_local == m_peer;
}

size_t TCPConnection::connection_hash(const TCPConnection &conn)
{
    return conn.m_local.hash() + conn.m_peer.hash();
}