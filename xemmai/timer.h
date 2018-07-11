#ifndef XEMMAIX__XRAFT__TIMER_H
#define XEMMAIX__XRAFT__TIMER_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<xraft::t_timer> : xemmaix::xraft::t_derivable<t_bears<xraft::t_timer, t_type_of<xraft::t_object>>>
{
	static void f_start(xraft::t_timer& a_self, size_t a_interval)
	{
		a_self.f_start(a_interval);
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
};

}

#endif
