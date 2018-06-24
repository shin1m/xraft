#include "window.h"
#include "pixmap.h"

#include <xemmaix/cairo/context.h>
#include <xraft/cairo.h>

namespace xemmaix::xraftcairo
{

using namespace xraft;
using namespace xemmai;

namespace
{

template<typename T>
t_scoped f_surface_create(T& a_target)
{
	return xemmaix::cairo::t_surface::f_construct(f_surface_create(a_target));
}

struct t_draw
{
	const t_value& v_callable;

	t_draw(const t_value& a_callable) : v_callable(a_callable)
	{
	}
	void operator()(cairo_t* a_context) const
	{
		v_callable(xemmaix::cairo::t_context::f_construct(a_context));
	}
};

template<typename T>
void f_draw(T& a_target, const t_value& a_callable)
{
	f_draw(a_target, t_draw(a_callable));
}

}

class t_extension : public xemmai::t_extension
{
	t_slot v_module_cairo;
	xemmaix::cairo::t_extension* v_cairo;

public:
	t_extension(xemmai::t_object* a_module, t_scoped&& a_cairo) : xemmai::t_extension(a_module), v_module_cairo(std::move(a_cairo))
	{
		v_cairo = f_extension<xemmaix::cairo::t_extension>(v_module_cairo);
		f_define<t_scoped(*)(t_bitmap&), f_surface_create<t_bitmap>>(this, L"BitmapSurface");
		f_define<t_scoped(*)(t_pixmap&), f_surface_create<t_pixmap>>(this, L"PixmapSurface");
		f_define<void(*)(t_window&, const t_value&), f_draw<t_window>>(this, L"draw_on_window");
		f_define<void(*)(t_graphics&, const t_value&), f_draw<t_graphics>>(this, L"draw_on_graphics");
	}
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
	t_scoped f_as(const T& a_value) const
	{
		return v_cairo->f_as(a_value);
	}
};

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new xemmaix::xraftcairo::t_extension(a_module, xemmai::t_module::f_instantiate(L"cairo"));
}
