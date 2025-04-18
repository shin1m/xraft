#include "region.h"
#include "geometry.h"

namespace xemmai
{

void t_type_of<xraft::t_region>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
	(L"clear"sv, t_member<void(t_region::*)(), &t_region::f_clear>())
	(L"swap"sv, t_member<void(t_region::*)(t_region&), &t_region::f_swap>())
	(L"empty"sv, t_member<bool(t_region::*)() const, &t_region::f_empty>())
	(L"contains"sv, t_member<bool(t_region::*)(int, int, unsigned, unsigned) const, &t_region::f_contains>())
	(L"bounds"sv, t_member<t_rectangle(t_region::*)() const, &t_region::f_bounds>())
	(L"intersect"sv, t_member<void(t_region::*)(const t_region&, const t_region&), &t_region::f_intersect>())
	(L"unite"sv,
		t_member<void(t_region::*)(const t_region&, const t_region&), &t_region::f_unite>(),
		t_member<void(t_region::*)(const t_region&, int, int, unsigned, unsigned), &t_region::f_unite>()
	)
	(L"subtract"sv, t_member<void(t_region::*)(const t_region&, const t_region&), &t_region::f_subtract>())
	(L"xor"sv, t_member<void(t_region::*)(const t_region&, const t_region&), &t_region::f_xor>())
	(L"translate"sv, t_member<void(t_region::*)(int, int), &t_region::f_translate>())
	.f_derive<t_region, xemmai::t_object>();
}

t_pvalue t_type_of<xraft::t_region>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct<>::t_bind<xraft::t_region>::f_do(this, a_stack, a_n);
}

}
