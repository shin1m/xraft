#include "xraftcairo.h"

namespace xraft
{

namespace xemmai
{

namespace
{

template<typename T>
t_transfer f_surface_create(t_xraftcairo* a_extension, T& a_target)
{
	return cairo::xemmai::t_surface::f_construct(f_surface_create(a_target));
}

struct t_draw
{
	const t_value& v_callable;

	t_draw(const t_value& a_callable) : v_callable(a_callable)
	{
	}
	void operator()(cairo_t* a_context) const
	{
		v_callable(cairo::xemmai::t_context::f_construct(a_context));
	}
};

template<typename T>
void f_draw(t_xraftcairo* a_extension, T& a_target, const t_value& a_callable)
{
	f_draw(a_target, t_draw(a_callable));
}

}

t_xraftcairo::t_xraftcairo(t_object* a_module, const t_transfer& a_cairo) : ::xemmai::t_extension(a_module), v_module_cairo(a_cairo)
{
	v_cairo = f_extension<cairo::xemmai::t_extension>(v_module_cairo);
	f_define<t_transfer (*)(t_xraftcairo*, t_bitmap&), f_surface_create<t_bitmap> >(this, L"BitmapSurface");
	f_define<t_transfer (*)(t_xraftcairo*, t_pixmap&), f_surface_create<t_pixmap> >(this, L"PixmapSurface");
	f_define<void (*)(t_xraftcairo*, t_window&, const t_value&), f_draw<t_window> >(this, L"draw_on_window");
	f_define<void (*)(t_xraftcairo*, t_graphics&, const t_value&), f_draw<t_graphics> >(this, L"draw_on_graphics");
}

void t_xraftcairo::f_scan(t_scan a_scan)
{
	a_scan(v_module_cairo);
}

}

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new xraft::xemmai::t_xraftcairo(a_module, xemmai::t_module::f_instantiate(L"cairo"));
}
