#include "Timer.h"
#include "Log.h"

using namespace generic;

void TimingWheel::add(TimerEntryPtr &ent)
{
    LOG_TRACE << "TimingWheel::add";
    addEntry(ent);
}

// called every m_interval ms
void TimingWheel::tick()
{
    LOG_TRACE << "TimingWheel::tick";
    Itor it = m_slots[m_cur].begin();
    for (; it != m_slots[m_cur].end();)
    {
        if (!(*it))
        {
            it = m_slots[m_cur].erase(it);
            continue;
        }
        if ((*it)->m_disabled)
        {
            it = m_slots[m_cur].erase(it);
            continue;
        }
        if ((*it)->m_round > 0)
        {
            --(*it)->m_round;
            ++it;
        }
        else
        {
            (*it)->expired();
            if ((*it)->m_repeat)
            {
                addEntry(*it);
            }
            it = m_slots[m_cur].erase(it);
        }
    }
    m_cur = (m_cur + 1) % ROUND_SIZE;
}

void TimingWheel::addEntry(TimerEntryPtr &ent)
{
    ent->m_round = (m_cur + (ent->m_timeoutms / m_interval)) / ROUND_SIZE;
    int slot = (m_cur + (ent->m_timeoutms / m_interval)) % ROUND_SIZE;
    m_slots[slot].push_back(ent);
}