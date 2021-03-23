#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include "EventLoop.h"
#include "EventLoopThread.h"
#include "InetAddress.h"
#include "TCPConnection.h"
#include "Connector.h"

#include <unordered_map>

namespace generic
{

    class TCPClient
    {
        using EventLoopPtr = std::shared_ptr<EventLoop>;
        using TCPConnectionPtr = std::shared_ptr<TCPConnection>;
        using EventLoopThreadPtr = std::shared_ptr<EventLoopThread>;

    public:
        TCPClient(InetAddress addr) : m_loop(std::make_shared<EventLoop>()),
                                      m_loopThread(std::make_shared<EventLoopThread>(m_loop)),
                                      m_serverAddr(addr),
                                      m_conns(),
                                      m_connector(new Connector(addr, m_loop))
        {
            assert(m_loop != nullptr);
            assert(m_connector != nullptr);

            m_connector->setNewConnectionCallback(std::bind(&TCPClient::connected, this, std::placeholders::_1));
            m_loopThread->start();
        }

        void connect()
        {
            m_connector->connect();
        }

        void disconnect()
        {
        }

        void send(std::string &msg)
        {
            for (auto &i : m_conns)
            {
                int ret = SocketOP::send(i.first, msg.c_str(), msg.length());
                LOG_TRACE << "send " << ret << " bytes to " << i.second->getPeerAddr().toString();
            }
        }

    private:
        void connected(int fd)
        {
            LOG_TRACE << "TCPClient::connected";
            TCPConnectionPtr conn = std::make_shared<TCPConnection>(fd, m_serverAddr, m_loop);
            conn->setMsgCallback(std::bind(&TCPClient::msgCallback, this, std::placeholders::_1));
            conn->setCloseCallback(std::bind(&TCPClient::closeCallback, this, std::placeholders::_1));

            // fixme
            /*
            conn->addTask(
                [conn]() {
                    static int num = 0;
                    num++;
                    std::string msg = "Heart beat,num=" + std::to_string(num);
                    conn->sendMsg(msg.c_str(), msg.length());
                    LOG_DEBUG << "timer callback";
                },
                5000, true);
                */

            m_conns[fd] = conn;
            conn->startPollEvent();
        }

        void msgCallback(TCPConnectionPtr conn)
        {
            TCPConnection::BufferPtr buf = conn->getReadBuffer();
            LOG_DEBUG << "Msg from " << conn->getPeerAddr().toString() << " " << buf->toString();
        }

        void closeCallback(TCPConnectionPtr conn)
        {
            m_conns.erase(conn->getFd());
        }

    private:
        EventLoopPtr m_loop;
        EventLoopThreadPtr m_loopThread;
        InetAddress m_serverAddr;
        std::unordered_map<int, TCPConnectionPtr> m_conns;
        std::unique_ptr<Connector> m_connector;
    };
}

#endif