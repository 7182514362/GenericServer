#include "IdleConnections.h"
#include "TCPConnection.h"
#include "Log.h"

using namespace generic;

IdleConnectionEntry::~IdleConnectionEntry()
{
    LOG_DEBUG << "conn expired";
    std::shared_ptr<TCPConnection> ptr = m_conn.lock();
    if (ptr)
        ptr->shutdown();
}

std::weak_ptr<IdleConnectionEntry>
IdleConnections::add(TCPConnectionWeakPtr conn)
{
    LOG_TRACE << "IdleConnections::add";
    //std::lock_guard<std::mutex> guard(m_mutex);
    std::shared_ptr<IdleConnectionEntry> ptr = std::make_shared<IdleConnectionEntry>(conn);
    m_conns[m_cur].push_back(ptr);
    return ptr;
}

void IdleConnections::update(std::shared_ptr<IdleConnectionEntry> &ent)
{
    LOG_TRACE << "IdleConnections::update";
    //std::lock_guard<std::mutex> guard(m_mutex);
    m_conns[m_cur].push_back(ent);
}

void IdleConnections::tick()
{
    LOG_TRACE << "IdleConnections::tick";
    //std::lock_guard<std::mutex> guard(m_mutex);
    ++m_cur;
    if (m_cur >= SLOT_SIZE)
    {
        m_cur = 0;
    }

    LOG_DEBUG << "expired Connnections size=" << m_conns[m_cur].size();
    // clear expired connections entry
    m_conns[m_cur].clear();
}
