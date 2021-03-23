#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <cstdio>
#include <functional>
#include <unordered_map>
#include <map>

#include "EventLoop.h"
#include "EventLoopThreadPool.h"
#include "InetAddress.h"
#include "Acceptor.h"
#include "Noncopyable.h"
#include "IdleConnections.h"
#include "TCPConnection.h"
#include "Time.h"
#include "Config.h"

namespace generic
{

	class TCPServer : Noncopyable
	{
		using EventLoopPtr = std::shared_ptr<EventLoop>;
		using TCPConnectionPtr = std::shared_ptr<TCPConnection>;
		using TCPConnectionWeakPtr = std::weak_ptr<TCPConnection>;
		using IdleConnsPtr = std::shared_ptr<IdleConnections>;

	public:
		std::function<void(TCPConnectionPtr)> m_connectionCallback;

		//TCPServer(const InetAddress &listenAddr, int nWorkers);
		TCPServer(Config &config);
		~TCPServer();

		void init();
		void start();

		EventLoopPtr &getMainEventLoop()
		{
			return m_loop;
		}

		std::vector<EventLoopPtr> getWorkderEventLoops();

		void setOnConnected(std::function<void(TCPConnectionPtr)> cb) { m_onConnected = std::move(cb); }
		void setOnMessage(std::function<void(TCPConnectionPtr)> cb) { m_onMessage = std::move(cb); }

	private:
		void onNewConnection(int fd, InetAddress &addr);
		//void onMessage(TCPConnectionPtr conn);
		void onClose(TCPConnectionPtr conn);
		void onError(TCPConnectionPtr conn);

	private:
		Config m_config;
		EventLoopPtr m_loop;
		InetAddress m_listenAddr;
		Acceptor m_acceptor;
		std::function<void(TCPConnectionPtr)> m_onConnected;
		std::function<void(TCPConnectionPtr)> m_onMessage;

		// file descriptor to conns
		std::unordered_map<int, TCPConnectionPtr> m_connMap;
		// thread id to idle conns, one idle Conns per loop thread
		//std::map<const int, IdleConnsPtr> m_idleConnsMap;
		std::unique_ptr<EventLoopThreadPool> m_pool;
	};
} // namespace generic

#endif