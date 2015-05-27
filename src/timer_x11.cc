#include <xraft/application.h>

namespace xraft
{

t_timer::~t_timer()
{
	f_stop();
}

void t_timer::f_add()
{
	gettimeofday(&v_time, NULL);
	v_time.tv_usec += v_interval * 1000;
	v_time.tv_sec += v_time.tv_usec / 1000000;
	v_time.tv_usec %= 1000000;
	t_timer** p = &f_application()->v_timer;
	while (*p && timercmp(&(*p)->v_time, &v_time, <=)) p = &(*p)->v_next;
	v_next = *p;
	*p = this;
}

void t_timer::f_stop()
{
	if (!v_queued) return;
	v_queued = false;
	t_timer** p = &f_application()->v_timer;
	while (*p != this) p = &(*p)->v_next;
	*p = v_next;
	v_next = 0;
}

}
