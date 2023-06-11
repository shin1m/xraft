#include "window.h"
#include "pixmap.h"

#include <xemmaix/cairo/context.h>
#include <xraft/cairo.h>

namespace xemmaix::xraftcairo
{

using namespace xraft;
using namespace xemmai;

class t_library : public xemmai::t_library
{
	t_slot v_module_cairo;
	xemmaix::cairo::t_library* v_cairo;

public:
	t_library(xemmai::t_library::t_handle* a_handle, const t_pvalue& a_cairo) : xemmai::t_library(a_handle), v_module_cairo(a_cairo), v_cairo(&v_module_cairo->f_as<t_module>().v_body->f_as<xemmaix::cairo::t_library>())
	{
	}
	XEMMAI__LIBRARY__MEMBERS
};

XEMMAI__LIBRARY__BASE(t_library, t_global, f_global())
XEMMAI__LIBRARY__BASE(t_library, xemmaix::cairo::t_library, v_cairo)

namespace
{

t_pvalue f_surface_create(t_library* a_library, auto& a_target)
{
	return xemmaix::cairo::t_surface::f_construct(a_library->f_type<xemmaix::cairo::t_surface>(), f_surface_create(a_target));
}

void f_draw(t_library* a_library, auto& a_target, const t_pvalue& a_callable)
{
	f_draw(a_target, [&](cairo_t* a_context)
	{
		a_callable(xemmaix::cairo::t_context::f_construct(a_library->f_type<xemmaix::cairo::t_context>(), a_context));
	});
}

}

void t_library::f_scan(t_scan a_scan)
{
	a_scan(v_module_cairo);
}

std::vector<std::pair<t_root, t_rvalue>> t_library::f_define()
{
	return t_define(this)
		(L"BitmapSurface"sv, t_static<t_pvalue(*)(t_library*, t_bitmap&), f_surface_create<t_bitmap>>())
		(L"PixmapSurface"sv, t_static<t_pvalue(*)(t_library*, t_pixmap&), f_surface_create<t_pixmap>>())
		(L"draw_on_window"sv, t_static<void(*)(t_library*, t_window&, const t_pvalue&), f_draw<t_window>>())
		(L"draw_on_graphics"sv, t_static<void(*)(t_library*, t_graphics&, const t_pvalue&), f_draw<t_graphics>>())
	;
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	using namespace std::literals;
	return xemmai::f_new<xemmaix::xraftcairo::t_library>(a_handle, xemmai::t_module::f_instantiate(L"cairo"sv));
}
