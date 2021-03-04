#include "drawable.h"
#include "graphics.h"

namespace xemmai
{

void t_type_of<xraft::t_drawable>::f_paint(xraft::t_drawable& a_self, const t_pvalue& a_callable, bool a_include)
{
	auto extension = f_extension<t_extension>(xemmaix::xraft::f_self(&a_self)->f_type()->v_module);
	auto object = f_engine()->f_allocate(false, sizeof(xraft::t_graphics*));
	xraft::t_graphics g(&a_self, a_include);
	object->f_as<xraft::t_graphics*>() = &g;
	object->f_be(extension->f_type<xraft::t_graphics>());
	try {
		a_callable(object);
		object->f_as<xraft::t_graphics*>() = nullptr;
	} catch (...) {
		object->f_as<xraft::t_graphics*>() = nullptr;
		throw;
	}
}

void t_type_of<xraft::t_drawable>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	using xemmaix::xraft::t_with_application_thread;
	t_define<t_drawable, xraft::t_object>(a_extension, L"Drawable"sv)
		(L"paint"sv,
			t_member<void(*)(t_drawable&, const t_pvalue&), f_paint, t_with_application_thread>(),
			t_member<void(*)(t_drawable&, const t_pvalue&, bool), f_paint, t_with_application_thread>()
		)
	;
}

}
