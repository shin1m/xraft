#ifndef XEMMAIX__XRAFT__GEOMETRY_H
#define XEMMAIX__XRAFT__GEOMETRY_H

#include "xraft.h"

namespace xemmai
{

template<>
struct t_type_of<xraft::t_point> : xemmaix::xraft::t_derivable<t_holds<xraft::t_point>>
{
	typedef xemmaix::xraft::t_library t_library;

	template<typename T_library, typename T>
	static t_pvalue f_transfer(T_library* a_library, T&& a_value)
	{
		return xemmai::f_new<typename t_fundamental<T>::t_type>(a_library, std::forward<T>(a_value));;
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
	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

template<>
struct t_type_of<xraft::t_extent> : xemmaix::xraft::t_derivable<t_holds<xraft::t_extent>>
{
	typedef xemmaix::xraft::t_library t_library;

	template<typename T_library, typename T>
	static t_pvalue f_transfer(T_library* a_library, T&& a_value)
	{
		return xemmai::f_new<typename t_fundamental<T>::t_type>(a_library, std::forward<T>(a_value));;
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
	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

template<>
struct t_type_of<xraft::t_rectangle> : xemmaix::xraft::t_derivable<t_holds<xraft::t_rectangle>>
{
	typedef xemmaix::xraft::t_library t_library;

	template<typename T_library, typename T>
	static t_pvalue f_transfer(T_library* a_library, T&& a_value)
	{
		return xemmai::f_new<typename t_fundamental<T>::t_type>(a_library, std::forward<T>(a_value));;
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
	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
