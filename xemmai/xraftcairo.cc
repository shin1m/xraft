#include "window.h"
#include "pixmap.h"

#include <xemmaix/cairo/context.h>
#include <xraft/cairo.h>

namespace xemmaix::xraftcairo
{

using namespace xraft;
using namespace xemmai;

class t_extension : public xemmai::t_extension
{
	t_slot v_module_cairo;
	xemmaix::cairo::t_extension* v_cairo;

public:
	t_extension(xemmai::t_object* a_module, const t_pvalue& a_cairo);
	virtual void f_scan(t_scan a_scan)
	{
		a_scan(v_module_cairo);
	}
	template<typename T>
	t_slot_of<t_type>& f_type_slot()
	{
		return v_cairo->f_type_slot<T>();
	}
	template<typename T>
	t_type* f_type() const
	{
		return const_cast<t_extension*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_pvalue f_as(const T& a_value) const
	{
		return v_cairo->f_as(a_value);
	}
};

namespace
{

template<typename T>
t_pvalue f_surface_create(t_extension* a_extension, T& a_target)
{
	return xemmaix::cairo::t_surface::f_construct(a_extension->f_type<xemmaix::cairo::t_surface>(), f_surface_create(a_target));
}

template<typename T>
void f_draw(t_extension* a_extension, T& a_target, const t_pvalue& a_callable)
{
	f_draw(a_target, [&](cairo_t* a_context)
	{
		a_callable(xemmaix::cairo::t_context::f_construct(a_extension->f_type<xemmaix::cairo::t_context>(), a_context));
	});
}

}

t_extension::t_extension(xemmai::t_object* a_module, const t_pvalue& a_cairo) : xemmai::t_extension(a_module), v_module_cairo(a_cairo)
{
	v_cairo = f_extension<xemmaix::cairo::t_extension>(v_module_cairo);
	f_define<t_pvalue(*)(t_extension*, t_bitmap&), f_surface_create<t_bitmap>>(this, L"BitmapSurface"sv);
	f_define<t_pvalue(*)(t_extension*, t_pixmap&), f_surface_create<t_pixmap>>(this, L"PixmapSurface"sv);
	f_define<void(*)(t_extension*, t_window&, const t_pvalue&), f_draw<t_window>>(this, L"draw_on_window"sv);
	f_define<void(*)(t_extension*, t_graphics&, const t_pvalue&), f_draw<t_graphics>>(this, L"draw_on_graphics"sv);
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	using namespace std::literals;
	return new xemmaix::xraftcairo::t_extension(a_module, xemmai::t_module::f_instantiate(L"cairo"sv));
}
