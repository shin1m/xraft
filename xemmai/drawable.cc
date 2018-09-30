#include "drawable.h"
#include "graphics.h"

namespace xemmai
{

void t_type_of<xraft::t_drawable>::f_paint(xraft::t_drawable& a_self, const t_value& a_callable, bool a_include)
{
	auto extension = f_extension<t_extension>(xemmaix::xraft::f_self(&a_self)->f_type()->v_module);
	t_scoped object = xemmai::t_object::f_allocate(extension->f_type<xraft::t_graphics>(), false);
	xraft::t_graphics g(&a_self, a_include);
	object.f_pointer__(&g);
	try {
		a_callable(object);
		object.f_pointer__(nullptr);
	} catch (...) {
		object.f_pointer__(nullptr);
		throw;
	}
}

void t_type_of<xraft::t_drawable>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	using xemmaix::xraft::t_with_application_thread;
	t_define<t_drawable, xraft::t_object>(a_extension, L"Drawable"sv)
		(L"paint"sv,
			t_member<void(*)(t_drawable&, const t_value&), f_paint, t_with_application_thread>(),
			t_member<void(*)(t_drawable&, const t_value&, bool), f_paint, t_with_application_thread>()
		)
	;
}

}
