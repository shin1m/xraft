#ifndef XRAFT__XEMMAI__GRAPHICS_H
#define XRAFT__XEMMAI__GRAPHICS_H

#include "geometry.h"
#include "object.h"

namespace xemmai
{

using namespace xraft;

template<>
struct t_type_of<t_font> : t_type_of<xraft::t_object>
{
	static t_scoped f_construct(::xemmai::t_object* a_class, const std::wstring& a_name)
	{
		return xraft::xemmai::t_proxy::f_construct(a_class, new t_font(portable::f_convert(a_name).c_str()));
	}
	static unsigned f_width(t_font& a_self, size_t a_c)
	{
		return a_self.f_width(a_c);
	}
	static unsigned f_width(t_font& a_self, const std::wstring& a_s)
	{
		return a_self.f_width(a_s.c_str(), a_s.size());
	}
	static unsigned f_width(t_font& a_self, const std::wstring& a_s, unsigned a_x)
	{
		return a_self.f_width(a_s.c_str(), a_s.size(), a_x);
	}
	static void f_define(t_extension* a_extension);

	using t_type_of<xraft::t_object>::t_type_of;
	virtual t_scoped f_construct(::xemmai::t_object* a_class, t_scoped* a_stack, size_t a_n);
};

template<>
struct t_type_of<t_color> : t_type
{
	typedef xraft::xemmai::t_extension t_extension;

#ifdef XRAFT_X11
	static t_scoped f_construct(::xemmai::t_object* a_class, const std::wstring& a_name)
	{
		t_scoped object = ::xemmai::t_object::f_allocate(a_class);
		object.f_pointer__(new t_color(portable::f_convert(a_name).c_str()));
		return object;
	}
#endif
	static t_scoped f_construct(::xemmai::t_object* a_class, size_t a_red, size_t a_green, size_t a_blue)
	{
		t_scoped object = ::xemmai::t_object::f_allocate(a_class);
		object.f_pointer__(new t_color(a_red, a_green, a_blue));
		return object;
	}
	static size_t f_red(t_color& a_self)
	{
		return a_self.f_red();
	}
	static size_t f_green(t_color& a_self)
	{
		return a_self.f_green();
	}
	static size_t f_blue(t_color& a_self)
	{
		return a_self.f_blue();
	}
	static void f_define(t_extension* a_extension);

	using t_type::t_type;
	virtual t_type* f_derive(::xemmai::t_object* a_this);
	virtual void f_finalize(::xemmai::t_object* a_this);
	virtual t_scoped f_construct(::xemmai::t_object* a_class, t_scoped* a_stack, size_t a_n);
};

template<>
struct t_type_of<t_graphics> : t_type
{
	template<typename T0, typename T1>
	struct t_as
	{
		typedef T0 t_type;

		static T0 f_call(T1 a_object)
		{
			T0* p = static_cast<T0*>(f_object(a_object)->f_pointer());
			if (!p) t_throwable::f_throw(L"already destroyed.");
			return *p;
		}
	};
	template<typename T0, typename T1>
	struct t_as<T0*, T1>
	{
		typedef T0* t_type;

		static T0* f_call(T1 a_object)
		{
			T0* p = static_cast<T0*>(f_object(a_object)->f_pointer());
			if (!p) t_throwable::f_throw(L"already destroyed.");
			return p;
		}
	};
	template<typename T0, typename T1>
	struct t_as<T0&, T1>
	{
		typedef T0& t_type;

		static T0& f_call(T1 a_object)
		{
			T0* p = static_cast<T0*>(f_object(a_object)->f_pointer());
			if (!p) t_throwable::f_throw(L"already destroyed.");
			return *p;
		}
	};
	typedef xraft::xemmai::t_extension t_extension;

	static void f_extract(const t_value& a_points0, std::vector<t_point>& a_points1);
	static void f_font(t_graphics& a_self, const xraft::t_pointer<t_font>& a_font)
	{
		if (!a_font) t_throwable::f_throw(L"font must not be null.");
		a_self.f_font(a_font);
	}
	static void f_draw(t_graphics& a_self, const t_value& a_points)
	{
		std::vector<t_point> points;
		f_extract(a_points, points);
		a_self.f_draw(&points[0], points.size());
	}
	static void f_fill(t_graphics& a_self, const t_value& a_points)
	{
		std::vector<t_point> points;
		f_extract(a_points, points);
		a_self.f_fill(&points[0], points.size());
	}
	static void f_draw(t_graphics& a_self, int a_x, int a_y, const std::wstring& a_s)
	{
		a_self.f_draw(a_x, a_y, a_s.c_str(), a_s.size());
	}
	static void f_draw(t_graphics& a_self, int a_x, int a_y, const xraft::t_pointer<t_pixmap>& a_pixmap, int a_left, int a_top, unsigned a_width, unsigned a_height)
	{
		if (!a_pixmap) t_throwable::f_throw(L"pixmap must not be null.");
		a_self.f_draw(a_x, a_y, a_pixmap, a_left, a_top, a_width, a_height);
	}
	static void f_draw(t_graphics& a_self, int a_x, int a_y, const xraft::t_pointer<t_pixmap>& a_pixmap, int a_left, int a_top, unsigned a_width, unsigned a_height, const xraft::t_pointer<t_bitmap>& a_bitmap)
	{
		if (!a_pixmap) t_throwable::f_throw(L"pixmap must not be null.");
		a_self.f_draw(a_x, a_y, a_pixmap, a_left, a_top, a_width, a_height, a_bitmap);
	}
	static void f_define(t_extension* a_extension);

	using t_type::t_type;
	virtual t_type* f_derive(::xemmai::t_object* a_this);
};

template<>
struct t_type_of<t_graphics::t_function> : t_enum_of<t_graphics::t_function, xraft::xemmai::t_extension>
{
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

}

#endif
