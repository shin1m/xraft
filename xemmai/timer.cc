#include "timer.h"

namespace xemmaix::xraft
{

struct t_timer : ::xraft::t_timer, t_wrapper<t_timer>
{
	static t_pvalue f_construct(t_type* a_class, const t_pvalue& a_callable)
	{
		return t_proxy::f_construct(a_class, new t_timer(a_callable));
	}

	t_rvalue v_callable;

	t_timer(const t_pvalue& a_callable) : v_callable(a_callable)
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

void t_type_of<xraft::t_timer>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
	(L"start"sv,
		t_member<void(*)(t_timer&, size_t), f_start>(),
		t_member<void(t_timer::*)(size_t, bool), &t_timer::f_start>()
	)
	(L"stop"sv, t_member<void(t_timer::*)(), &t_timer::f_stop>())
	.f_derive<t_timer, xraft::t_object>();
}

t_pvalue t_type_of<xraft::t_timer>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_pvalue(*)(t_type*, const t_pvalue&), xemmaix::xraft::t_timer::f_construct>::t_bind<xraft::t_timer>::f_do(this, a_stack, a_n);
}

}
