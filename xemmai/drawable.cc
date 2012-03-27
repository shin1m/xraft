#include "drawable.h"
#include "graphics.h"

namespace xemmai
{

using xraft::xemmai::t_with_application_thread;

void t_type_of<t_drawable>::f_paint(t_drawable& a_self, const t_value& a_callable, bool a_include)
{
	t_extension* extension = f_extension<t_extension>(f_as<t_type*>(xraft::xemmai::f_self(&a_self)->f_type())->v_module);
	t_scoped object = ::xemmai::t_object::f_allocate(extension->f_type<t_graphics>());
	t_graphics g(&a_self, a_include);
	object.f_pointer__(&g);
	try {
		a_callable(object);
		object.f_pointer__(0);
	} catch (...) {
		object.f_pointer__(0);
		throw;
	}
}

void t_type_of<t_drawable>::f_define(t_extension* a_extension)
{
	t_define<t_drawable, xraft::t_object>(a_extension, L"Drawable")
		(L"paint",
			t_member<void (*)(t_drawable&, const t_value&), f_paint, t_with_application_thread>(),
			t_member<void (*)(t_drawable&, const t_value&, bool), f_paint, t_with_application_thread>()
		)
	;
}

}
