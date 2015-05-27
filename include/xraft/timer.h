#ifndef XRAFT__TIMER_H
#define XRAFT__TIMER_H

#include "object.h"

#ifdef XRAFT_X11
#include <cstddef>
#include <sys/time.h>
#endif
#ifdef XRAFT_WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

namespace xraft
{

class t_application;

struct t_runnable
{
	virtual void operator()() = 0;
};

class t_timer : public t_object, protected t_runnable
{
	friend class t_application;

	size_t v_interval;
	bool v_single;
#ifdef XRAFT_X11
	bool v_queued = false;
	t_timer* v_next = nullptr;
	timeval v_time;
#endif
#ifdef XRAFT_WIN32
	FILETIME v_time;
#endif

protected:
	XRAFT__EXPORT virtual ~t_timer();
	XRAFT__EXPORT void f_add();

public:
	void f_start(size_t a_interval, bool a_single = false)
	{
		f_stop();
		v_interval = a_interval;
		v_single = a_single;
#ifdef XRAFT_X11
		v_queued = true;
#endif
		f_add();
	}
	XRAFT__EXPORT void f_stop();
};

}

#endif
