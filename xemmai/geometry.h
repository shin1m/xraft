#ifndef XEMMAIX__XRAFT__GEOMETRY_H
#define XEMMAIX__XRAFT__GEOMETRY_H

#include "xraft.h"

namespace xemmai
{

template<>
struct t_type_of<xraft::t_point> : xemmaix::xraft::t_derivable<t_holds<xraft::t_point>>
{
	typedef xemmaix::xraft::t_extension t_extension;

	template<typename T_extension, typename T>
	static t_scoped f_transfer(T_extension* a_extension, T&& a_value)
	{
		t_scoped object = t_object::f_allocate(a_extension->template f_type<typename t_fundamental<T>::t_type>());
		object.f_pointer__(new xraft::t_point(std::forward<T>(a_value)));
		return object;
	}
	static int f_x(xraft::t_point& a_self)
	{
		return a_self.v_x;
	}
	static void f_x__(xraft::t_point& a_self, int a_value)
	{
		a_self.v_x = a_value;
	}
	static int f_y(xraft::t_point& a_self)
	{
		return a_self.v_y;
	}
	static void f_y__(xraft::t_point& a_self, int a_value)
	{
		a_self.v_y = a_value;
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
};

template<>
struct t_type_of<xraft::t_extent> : xemmaix::xraft::t_derivable<t_holds<xraft::t_extent>>
{
	typedef xemmaix::xraft::t_extension t_extension;

	template<typename T_extension, typename T>
	static t_scoped f_transfer(T_extension* a_extension, T&& a_value)
	{
		t_scoped object = t_object::f_allocate(a_extension->template f_type<typename t_fundamental<T>::t_type>());
		object.f_pointer__(new xraft::t_extent(std::forward<T>(a_value)));
		return object;
	}
	static unsigned f_width(xraft::t_extent& a_self)
	{
		return a_self.v_width;
	}
	static void f_width__(xraft::t_extent& a_self, unsigned a_value)
	{
		a_self.v_width = a_value;
	}
	static unsigned f_height(xraft::t_extent& a_self)
	{
		return a_self.v_height;
	}
	static void f_height__(xraft::t_extent& a_self, unsigned a_value)
	{
		a_self.v_height = a_value;
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
};

template<>
struct t_type_of<xraft::t_rectangle> : xemmaix::xraft::t_derivable<t_holds<xraft::t_rectangle>>
{
	typedef xemmaix::xraft::t_extension t_extension;

	template<typename T_extension, typename T>
	static t_scoped f_transfer(T_extension* a_extension, T&& a_value)
	{
		t_scoped object = t_object::f_allocate(a_extension->template f_type<typename t_fundamental<T>::t_type>());
		object.f_pointer__(new xraft::t_rectangle(std::forward<T>(a_value)));
		return object;
	}
	static int f_x(xraft::t_rectangle& a_self)
	{
		return a_self.v_x;
	}
	static void f_x__(xraft::t_rectangle& a_self, int a_value)
	{
		a_self.v_x = a_value;
	}
	static int f_y(xraft::t_rectangle& a_self)
	{
		return a_self.v_y;
	}
	static void f_y__(xraft::t_rectangle& a_self, int a_value)
	{
		a_self.v_y = a_value;
	}
	static unsigned f_width(xraft::t_rectangle& a_self)
	{
		return a_self.v_width;
	}
	static void f_width__(xraft::t_rectangle& a_self, unsigned a_value)
	{
		a_self.v_width = a_value;
	}
	static unsigned f_height(xraft::t_rectangle& a_self)
	{
		return a_self.v_height;
	}
	static void f_height__(xraft::t_rectangle& a_self, unsigned a_value)
	{
		a_self.v_height = a_value;
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
};

}

#endif
