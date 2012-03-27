#include "region.h"
#include "geometry.h"

namespace xemmai
{

using xraft::xemmai::t_with_application_thread;

void t_type_of<t_region>::f_define(t_extension* a_extension)
{
	t_define<t_region, ::xemmai::t_object>(a_extension, L"Region")
		(L"clear", t_member<void (t_region::*)(), &t_region::f_clear, t_with_application_thread>())
		(L"swap", t_member<void (t_region::*)(t_region&), &t_region::f_swap, t_with_application_thread>())
		(L"empty", t_member<bool (t_region::*)() const, &t_region::f_empty, t_with_application_thread>())
		(L"contains", t_member<bool (t_region::*)(int, int, unsigned, unsigned) const, &t_region::f_contains, t_with_application_thread>())
		(L"bounds", t_member<t_rectangle (t_region::*)() const, &t_region::f_bounds, t_with_application_thread>())
		(L"intersect", t_member<void (t_region::*)(const t_region&, const t_region&), &t_region::f_intersect, t_with_application_thread>())
		(L"unite",
			t_member<void (t_region::*)(const t_region&, const t_region&), &t_region::f_unite, t_with_application_thread>(),
			t_member<void (t_region::*)(const t_region&, int, int, unsigned, unsigned), &t_region::f_unite, t_with_application_thread>()
		)
		(L"subtract", t_member<void (t_region::*)(const t_region&, const t_region&), &t_region::f_subtract, t_with_application_thread>())
		(L"xor", t_member<void (t_region::*)(const t_region&, const t_region&), &t_region::f_xor, t_with_application_thread>())
		(L"translate", t_member<void (t_region::*)(int, int), &t_region::f_translate, t_with_application_thread>())
	;
}

t_type* t_type_of<t_region>::f_derive(t_object* a_this)
{
	return new t_type_of(v_module, a_this);
}

void t_type_of<t_region>::f_finalize(t_object* a_this)
{
	delete f_as<t_region*>(a_this);
}

t_transfer t_type_of<t_region>::f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n)
{
	return t_construct<>::t_bind<t_region>::f_do(a_class, a_stack, a_n);
}

}
