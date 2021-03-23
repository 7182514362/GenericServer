#ifndef ECHO_SERVER_H
#define ECHO_SERVER_H

#include "TCPServer.h"
#include "TCPConnection.h"
#include "IdleConnections.h"
#include <map>
using namespace generic;

class EchoServer
{
    using IdleConnsPtr = std::shared_ptr<IdleConnections>;
    using TCPConnectionPtr = std::shared_ptr<TCPConnection>;

public:
    EchoServer(Config config) : m_server(config)
    {
        m_server.setOnConnected(std::bind(&EchoServer::onConnected, this, std::placeholders::_1));
        m_server.setOnMessage(std::bind(&EchoServer::onMessage, this, std::placeholders::_1));

        m_server.init();

        auto loops = m_server.getWorkderEventLoops();
        for (auto &loop : loops)
        {
            int tid = loop->getThreadId();
            if (m_idleConnsMap.find(tid) == m_idleConnsMap.end())
            {
                m_idleConnsMap[tid] = std::make_shared<IdleConnections>();
            }
            // update idle connections in worker thread every 1000ms
            std::shared_ptr<TimerEntry> timer =
                std::make_shared<TimerEntry>(std::bind(&IdleConnections::tick, m_idleConnsMap[tid]), 1000, true);
            loop->addTimer(timer);
        }
    }

    void start()
    {
        m_server.start();
    }

private:
    // run in loop thread
    void onConnected(TCPConnectionPtr conn)
    {
        int threadId = conn->getEventLoop()->getThreadId();
        if (m_idleConnsMap.find(threadId) == m_idleConnsMap.end())
        {
            m_idleConnsMap[threadId] = std::make_shared<IdleConnections>();
        }
        setIdleConn(conn, threadId);
    }

    void onMessage(TCPConnectionPtr conn)
    {
        LOG_DEBUG << conn->getReadBuffer()->toString();
        updateIdleConn(conn->getIdleEntry(), conn->getEventLoop()->getThreadId());
        conn->sendMsg(conn->getReadBuffer()->read_ptr(), conn->getReadBuffer()->data_size());
    }

    // run in worker loop thread
    void setIdleConn(std::shared_ptr<TCPConnection> &conn, int threadId)
    {
        LOG_TRACE << "EchoServer::setIdleConn";
        auto entry = m_idleConnsMap[threadId]->add(conn);
        conn->setIdleEntry(entry);
    }
    // run in worker loop thread
    void updateIdleConn(std::weak_ptr<IdleConnectionEntry> ent, int threadId)
    {
        LOG_TRACE << "EchoServer::updateIdleConn";
        std::shared_ptr<IdleConnectionEntry> ptr = ent.lock();
        if (ptr)
            m_idleConnsMap[threadId]->update(ptr);
    }

private:
    TCPServer m_server;
    // one idleconnections per worker thread
    std::map<const int, IdleConnsPtr> m_idleConnsMap;
};

#endif