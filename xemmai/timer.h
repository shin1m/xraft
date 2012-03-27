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

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type_of<xraft::t_object>(a_module, a_super)
	{
	}
	virtual t_type* f_derive(::xemmai::t_object* a_this);
	virtual t_transfer f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
