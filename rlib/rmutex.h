#ifndef rmutex_h__
#define rmutex_h__

#ifdef _MSC_VER
#include <windows.h>
#else
#include <semaphore.h>
#endif

struct rmutex
{
#ifdef _MSC_VER
	CRITICAL_SECTION m_mutex;
#else
	sem_t m_mutex;
#endif
	rmutex()
	{
#ifdef _MSC_VER
		InitializeCriticalSection(&m_mutex);
#else
		sem_init(&m_mutex,0,1);
#endif
	}

	~rmutex()
	{
#ifdef _MSC_VER
		DeleteCriticalSection(&m_mutex);
#else
#endif
	}

	void enter()
	{
#ifdef _MSC_VER
		EnterCriticalSection(&m_mutex);
#else
		sem_wait(&m_mutex);
#endif
	}

	void leave()
	{
#ifdef _MSC_VER
		LeaveCriticalSection(&m_mutex);
#else
		sem_post(&m_mutex);
#endif
	}
};

//用于整个函数的局部临时mutex
struct rmutex_t
{
	rmutex* m_p;

	rmutex_t(rmutex& m)
	{
		m_p=&m;
		m_p->enter();
	}

	~rmutex_t()
	{
		m_p->leave();
	}
};

#endif