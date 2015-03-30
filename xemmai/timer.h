#ifndef XRAFT__XEMMAI__TIMER_H
#define XRAFT__XEMMAI__TIMER_H

#include "object.h"

namespace xemmai
{

using namespace xraft;

template<>
struct t_type_of<t_timer> : t_type_of<xraft::t_object>
{
	static void f_start(t_timer& a_self, size_t a_interval)
	{
		a_self.f_start(a_interval);
	}
	static void f_define(t_extension* a_extension);

	using t_type_of<xraft::t_object>::t_type_of;
	virtual t_type* f_derive(::xemmai::t_object* a_this);
	virtual t_scoped f_construct(::xemmai::t_object* a_class, t_scoped* a_stack, size_t a_n);
};

}

#endif
