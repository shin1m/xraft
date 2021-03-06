#ifndef XEMMAIX__XRAFT__TIMER_H
#define XEMMAIX__XRAFT__TIMER_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<xraft::t_timer> : xemmaix::xraft::t_derivable<xemmaix::xraft::t_bears_pointer<xraft::t_timer, t_type_of<xraft::t_object>>>
{
	static void f_start(xraft::t_timer& a_self, size_t a_interval)
	{
		a_self.f_start(a_interval);
	}
	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
