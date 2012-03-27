#ifndef LOCK_H
#define LOCK_H

#ifdef XRAFT_WIN32

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>

class t_mutex
{
	CRITICAL_SECTION v_cs;

public:
	t_mutex()
	{
		InitializeCriticalSection(&v_cs);
	}
	~t_mutex()
	{
		DeleteCriticalSection(&v_cs);
	}
	void f_lock()
	{
		EnterCriticalSection(&v_cs);
	}
	void f_unlock()
	{
		LeaveCriticalSection(&v_cs);
	}
};

#else

#include <pthread.h>

class t_mutex
{
	pthread_mutex_t v_mutex;

public:
	t_mutex()
	{
		pthread_mutex_init(&v_mutex, NULL);
	}
	~t_mutex()
	{
		pthread_mutex_destroy(&v_mutex);
	}
	void f_lock()
	{
		pthread_mutex_lock(&v_mutex);
	}
	void f_unlock()
	{
		pthread_mutex_unlock(&v_mutex);
	}
};

#endif

template<typename T>
class t_lock
{
	T& v_lock;

public:
	t_lock(T& a_lock) : v_lock(a_lock)
	{
		v_lock.f_lock();
	}
	~t_lock()
	{
		v_lock.f_unlock();
	}
};

#endif
