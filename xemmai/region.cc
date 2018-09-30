#include "region.h"
#include "geometry.h"

namespace xemmai
{

void t_type_of<xraft::t_region>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	using xemmaix::xraft::t_with_application_thread;
	t_define<t_region, xemmai::t_object>(a_extension, L"Region"sv)
		(L"clear"sv, t_member<void(t_region::*)(), &t_region::f_clear, t_with_application_thread>())
		(L"swap"sv, t_member<void(t_region::*)(t_region&), &t_region::f_swap, t_with_application_thread>())
		(L"empty"sv, t_member<bool(t_region::*)() const, &t_region::f_empty, t_with_application_thread>())
		(L"contains"sv, t_member<bool(t_region::*)(int, int, unsigned, unsigned) const, &t_region::f_contains, t_with_application_thread>())
		(L"bounds"sv, t_member<t_rectangle(t_region::*)() const, &t_region::f_bounds, t_with_application_thread>())
		(L"intersect"sv, t_member<void(t_region::*)(const t_region&, const t_region&), &t_region::f_intersect, t_with_application_thread>())
		(L"unite"sv,
			t_member<void(t_region::*)(const t_region&, const t_region&), &t_region::f_unite, t_with_application_thread>(),
			t_member<void(t_region::*)(const t_region&, int, int, unsigned, unsigned), &t_region::f_unite, t_with_application_thread>()
		)
		(L"subtract"sv, t_member<void(t_region::*)(const t_region&, const t_region&), &t_region::f_subtract, t_with_application_thread>())
		(L"xor"sv, t_member<void(t_region::*)(const t_region&, const t_region&), &t_region::f_xor, t_with_application_thread>())
		(L"translate"sv, t_member<void(t_region::*)(int, int), &t_region::f_translate, t_with_application_thread>())
	;
}

t_scoped t_type_of<xraft::t_region>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct<false>::t_bind<xraft::t_region>::f_do(this, a_stack, a_n);
}

}
