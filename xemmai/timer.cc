#include "timer.h"

namespace xraft
{

namespace xemmai
{

struct t_timer : xraft::t_timer, t_wrapper<t_timer>
{
	static t_transfer f_construct(::xemmai::t_object* a_class, const t_transfer& a_callable)
	{
		return t_proxy::f_construct(a_class, new t_timer(a_callable));
	}

	t_scoped v_callable;

	t_timer(const t_transfer& a_callable) : v_callable(a_callable)
	{
	}
	virtual void operator()()
	{
		v_callable();
	}
};

}

}

namespace xemmai
{

using xraft::xemmai::t_with_application_thread;

void t_type_of<t_timer>::f_define(t_extension* a_extension)
{
	t_define<t_timer, xraft::t_object>(a_extension, L"Timer")
		(L"start",
			t_member<void (*)(t_timer&, size_t), f_start, t_with_application_thread>(),
			t_member<void (t_timer::*)(size_t, bool), &t_timer::f_start, t_with_application_thread>()
		)
		(L"stop", t_member<void (t_timer::*)(), &t_timer::f_stop, t_with_application_thread>())
	;
}

t_type* t_type_of<t_timer>::f_derive(::xemmai::t_object* a_this)
{
	return new t_type_of(v_module, a_this);
}

t_transfer t_type_of<t_timer>::f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n)
{
	return t_construct_with<t_transfer (*)(::xemmai::t_object*, const t_transfer&), xraft::xemmai::t_timer::f_construct>::t_bind<t_timer>::f_do(a_class, a_stack, a_n);
}

}
