#ifndef IDLE_CONNECTIONS_H
#define IDLE_CONNECTIONS_H

#include <memory>
#include <mutex>
#include <vector>

namespace generic
{
    class TCPConnection;

    class IdleConnectionEntry
    {
    public:
        IdleConnectionEntry(std::weak_ptr<TCPConnection> conn) : m_conn(conn)
        {
        }
        ~IdleConnectionEntry();

    private:
        std::weak_ptr<TCPConnection> m_conn;
    };

#define SLOT_SIZE (60)
#define MILISECONDS_PER_SLOT (1000)
    // thread safe
    // Fixed expire time, current is 60 seconds
    class IdleConnections
    {
        using TCPConnectionWeakPtr = std::weak_ptr<TCPConnection>;

    public:
        // reference counter
        using EntryPtr = std::shared_ptr<IdleConnectionEntry>;
        using TCPConnections = std::vector<EntryPtr>;

    public:
        IdleConnections() : m_mutex(), m_conns(SLOT_SIZE), m_cur(0)
        {
        }

        std::weak_ptr<IdleConnectionEntry> add(TCPConnectionWeakPtr conn);
        void update(std::shared_ptr<IdleConnectionEntry> &ent);
        void tick();

    private:
        std::mutex m_mutex;
        std::vector<TCPConnections> m_conns;
        int m_cur;
    };
}

#endif