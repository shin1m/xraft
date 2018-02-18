#include "timer.h"

namespace xemmaix::xraft
{

struct t_timer : ::xraft::t_timer, t_wrapper<t_timer>
{
	static t_scoped f_construct(xemmai::t_object* a_class, t_scoped&& a_callable)
	{
		return t_proxy::f_construct(a_class, new t_timer(std::move(a_callable)));
	}

	t_scoped v_callable;

	t_timer(t_scoped&& a_callable) : v_callable(std::move(a_callable))
	{
	}
	virtual void operator()()
	{
		v_callable();
	}
};

}

namespace xemmai
{

void t_type_of<xraft::t_timer>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	using xemmaix::xraft::t_with_application_thread;
	t_define<t_timer, xraft::t_object>(a_extension, L"Timer")
		(L"start",
			t_member<void(*)(t_timer&, size_t), f_start, t_with_application_thread>(),
			t_member<void(t_timer::*)(size_t, bool), &t_timer::f_start, t_with_application_thread>()
		)
		(L"stop", t_member<void(t_timer::*)(), &t_timer::f_stop, t_with_application_thread>())
	;
}

t_type* t_type_of<xraft::t_timer>::f_derive(xemmai::t_object* a_this)
{
	return new t_type_of(v_module, a_this);
}

t_scoped t_type_of<xraft::t_timer>::f_construct(xemmai::t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(xemmai::t_object*, t_scoped&&), xemmaix::xraft::t_timer::f_construct>::t_bind<xraft::t_timer>::f_do(a_class, a_stack, a_n);
}

}
