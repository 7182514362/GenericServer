#include "TCPServer.h"

using namespace generic;
/*
TCPServer::TCPServer(const InetAddress &listenAddr, int nWorkers)
	: m_listenAddr(listenAddr),
	  m_loop(std::make_shared<EventLoop>()),
	  m_acceptor(m_loop, m_listenAddr,
				 std::bind(&TCPServer::onNewConnection, this, std::placeholders::_1, std::placeholders::_2)),
	  m_connMap(),
	  m_pool(),
	  m_idleConnsMap()
{
	init(nWorkers);
}
*/

TCPServer::TCPServer(Config &config) : m_config(config),
									   m_listenAddr(config.ip, config.port, config.use_ipv6),
									   m_loop(std::make_shared<EventLoop>()),
									   m_acceptor(m_loop, m_listenAddr,
												  std::bind(&TCPServer::onNewConnection, this, std::placeholders::_1, std::placeholders::_2)),
									   m_connMap(),
									   m_pool()
{
	//init(config.work_thread_num);
}

void TCPServer::init()
{
	LOG_DEBUG << "TCPServer::init nWorkers=" << m_config.work_thread_num;
	int nWorkers = m_config.work_thread_num;
	if (nWorkers > 0)
	{
		if (nWorkers >= std::thread::hardware_concurrency())
		{
			LOG_WARN << "worker threads number larger than hardware_concurrency";
			abort();
		}

		m_pool.reset(new EventLoopThreadPool(nWorkers));
		m_pool->start();
	}
	else if (nWorkers == 0)
	{
	}
	else
	{
		LOG_ERROR << "wrong config";
		abort();
	}
}

TCPServer::~TCPServer()
{
	if (m_pool)
	{
		m_pool->stop();
		m_pool->join();
	}
}

void TCPServer::start()
{
	m_acceptor.listen();
	m_loop->loop();
}

void TCPServer::onNewConnection(int fd, InetAddress &addr)
{
	LOG_INFO << "TCPServer: newConnection from " << addr.toString();
	TCPConnectionPtr conn;
	if (m_pool)
	{
		conn = std::make_shared<TCPConnection>(fd, addr, m_pool->getNextLoop());
	}
	else
	{
		conn = std::make_shared<TCPConnection>(fd, addr, m_loop);
	}
	conn->setConnectedCallback(m_onConnected);
	conn->setMsgCallback(m_onMessage);
	conn->setCloseCallback(std::bind(&TCPServer::onClose, this, std::placeholders::_1));
	conn->setErrorCallback(std::bind(&TCPServer::onError, this, std::placeholders::_1));

	conn->init();
	m_connMap[fd] = conn;

	LOG_DEBUG << "current connection: " << m_connMap.size();
}

std::vector<std::shared_ptr<EventLoop>> TCPServer::getWorkderEventLoops()
{
	if (m_pool)
	{
		return m_pool->getAllLoop();
	}
	else
	{
		std::vector<EventLoopPtr> loops;
		loops.push_back(m_loop);
		return loops;
	}
}

void TCPServer::onClose(TCPConnectionPtr conn)
{
	LOG_TRACE << "TCPServer::onClose";
	m_connMap.erase(conn->getFd());
}

void TCPServer::onError(TCPConnectionPtr conn)
{
}