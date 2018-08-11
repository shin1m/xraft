#ifndef XEMMAIX__XRAFT__GRAPHICS_H
#define XEMMAIX__XRAFT__GRAPHICS_H

#include "geometry.h"
#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<xraft::t_font> : t_bears<xraft::t_font, t_type_of<xraft::t_object>>
{
	static t_scoped f_construct(t_type* a_class, const std::wstring& a_name)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_font(portable::f_convert(a_name).c_str()));
	}
	static unsigned f_width(xraft::t_font& a_self, size_t a_c)
	{
		return a_self.f_width(a_c);
	}
	static unsigned f_width(xraft::t_font& a_self, const std::wstring& a_s)
	{
		return a_self.f_width(a_s.c_str(), a_s.size());
	}
	static unsigned f_width(xraft::t_font& a_self, const std::wstring& a_s, unsigned a_x)
	{
		return a_self.f_width(a_s.c_str(), a_s.size(), a_x);
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
};

template<>
struct t_type_of<xraft::t_color> : xemmaix::xraft::t_derivable<t_holds<xraft::t_color>>
{
	typedef xemmaix::xraft::t_extension t_extension;

#ifdef XRAFT_X11
	static t_scoped f_construct(t_type* a_class, const std::wstring& a_name)
	{
		t_scoped object = xemmai::t_object::f_allocate(a_class);
		object.f_pointer__(new xraft::t_color(portable::f_convert(a_name).c_str()));
		return object;
	}
#endif
	static t_scoped f_construct(t_type* a_class, size_t a_red, size_t a_green, size_t a_blue)
	{
		t_scoped object = xemmai::t_object::f_allocate(a_class);
		object.f_pointer__(new xraft::t_color(a_red, a_green, a_blue));
		return object;
	}
	static size_t f_red(xraft::t_color& a_self)
	{
		return a_self.f_red();
	}
	static size_t f_green(xraft::t_color& a_self)
	{
		return a_self.f_green();
	}
	static size_t f_blue(xraft::t_color& a_self)
	{
		return a_self.f_blue();
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
};

template<>
struct t_type_of<xraft::t_graphics> : t_underivable<t_bears<xraft::t_graphics>>
{
	template<typename T0>
	struct t_as
	{
		template<typename T1>
		static T0 f_call(T1&& a_object)
		{
			return *t_as<typename t_fundamental<T0>::t_type*>::f_call(std::forward<T1>(a_object));
		}
	};
	template<typename T0>
	struct t_as<T0*>
	{
		template<typename T1>
		static T0* f_call(T1&& a_object)
		{
			auto p = static_cast<T0*>(f_object(std::forward<T1>(a_object))->f_pointer());
			if (!p) f_throw(L"already destroyed.");
			return p;
		}
	};
	typedef xemmaix::xraft::t_extension t_extension;

	static void f_extract(const t_value& a_points0, std::vector<xraft::t_point>& a_points1);
	static void f_font(xraft::t_graphics& a_self, const xraft::t_pointer<xraft::t_font>& a_font)
	{
		if (!a_font) f_throw(L"font must not be null.");
		a_self.f_font(a_font);
	}
	static void f_draw(xraft::t_graphics& a_self, const t_value& a_points)
	{
		std::vector<xraft::t_point> points;
		f_extract(a_points, points);
		a_self.f_draw(&points[0], points.size());
	}
	static void f_fill(xraft::t_graphics& a_self, const t_value& a_points)
	{
		std::vector<xraft::t_point> points;
		f_extract(a_points, points);
		a_self.f_fill(&points[0], points.size());
	}
	static void f_draw(xraft::t_graphics& a_self, int a_x, int a_y, const std::wstring& a_s)
	{
		a_self.f_draw(a_x, a_y, a_s.c_str(), a_s.size());
	}
	static void f_draw(xraft::t_graphics& a_self, int a_x, int a_y, const xraft::t_pointer<xraft::t_pixmap>& a_pixmap, int a_left, int a_top, unsigned a_width, unsigned a_height)
	{
		if (!a_pixmap) f_throw(L"pixmap must not be null.");
		a_self.f_draw(a_x, a_y, a_pixmap, a_left, a_top, a_width, a_height);
	}
	static void f_draw(xraft::t_graphics& a_self, int a_x, int a_y, const xraft::t_pointer<xraft::t_pixmap>& a_pixmap, int a_left, int a_top, unsigned a_width, unsigned a_height, const xraft::t_pointer<xraft::t_bitmap>& a_bitmap)
	{
		if (!a_pixmap) f_throw(L"pixmap must not be null.");
		a_self.f_draw(a_x, a_y, a_pixmap, a_left, a_top, a_width, a_height, a_bitmap);
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

template<>
struct t_type_of<xraft::t_graphics::t_function> : t_enum_of<xraft::t_graphics::t_function, xemmaix::xraft::t_extension>
{
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

}

#endif
