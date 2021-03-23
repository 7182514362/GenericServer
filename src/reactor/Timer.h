#ifndef TIMER_H
#define TIMER_H

#include <functional>
#include <list>
#include <memory>
#include <assert.h>

#include "Noncopyable.h"

namespace generic
{
	class TimerEntry
	{
	public:
		TimerEntry(std::function<void()> f, int ms, bool repeat)
			: m_callback(std::move(f)),
			  m_timeoutms(ms),
			  m_round(0),
			  m_repeat(repeat),
			  m_disabled(false) {}

		void disable() { m_disabled = true; }

		void expired() const
		{
			assert(m_callback != nullptr);
			m_callback();
		}

	private:
		const std::function<void()> m_callback;
		int m_timeoutms;
		int m_round;
		bool m_repeat;
		bool m_disabled;

		friend class TimingWheel;
	};

#define ROUND_SIZE 100
#define WHEEL_INTERVAL_MS 10

	// TimingWheel doesn't modify TimerEntry
	class TimingWheel
	{
		using TimerEntryPtr = std::shared_ptr<TimerEntry>;
		using TimerList = std::list<TimerEntryPtr>;
		using Itor = std::list<TimerEntryPtr>::iterator;

	public:
		TimingWheel(int interval) : m_cur(0), m_interval(interval), m_slots() {}

		void add(TimerEntryPtr &ent);
		// called every m_interval ms
		void tick();

	private:
		void addEntry(TimerEntryPtr &ent);

	private:
		int m_cur;
		const int m_interval;
		TimerList m_slots[ROUND_SIZE];
	};

} // namespace generic
#endif