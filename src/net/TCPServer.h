#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "Noncopyable.h"
#include "EventLoopThreadPool.h"
#include "InetAddress.h"
#include "Acceptor.h"
#include "Config.h"
#include <memory>
#include <functional>
#include <unordered_map>

namespace generic
{
	class TCPConnection;

	class TCPServer : Noncopyable
	{
		using EventLoopPtr = std::shared_ptr<EventLoop>;
		using TCPConnectionPtr = std::shared_ptr<TCPConnection>;
		using TCPConnectionWeakPtr = std::weak_ptr<TCPConnection>;

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
		std::unique_ptr<EventLoopThreadPool> m_pool;
	};
} // namespace generic

#endif