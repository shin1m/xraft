#ifndef XRAFT__XEMMAI__GEOMETRY_H
#define XRAFT__XEMMAI__GEOMETRY_H

#include "xraft.h"

namespace xemmai
{

using namespace xraft;

template<>
struct t_type_of<t_point> : t_type
{
	typedef xraft::xemmai::t_extension t_extension;

	template<typename T_extension, typename T>
	static t_transfer f_transfer(T_extension* a_extension, T a_value)
	{
		t_transfer object = t_object::f_allocate(a_extension->template f_type<typename t_fundamental<T>::t_type>());
		object.f_pointer__(new t_point(a_value));
		return object;
	}
	static int f_x(t_point& a_self)
	{
		return a_self.v_x;
	}
	static void f_x__(t_point& a_self, int a_value)
	{
		a_self.v_x = a_value;
	}
	static int f_y(t_point& a_self)
	{
		return a_self.v_y;
	}
	static void f_y__(t_point& a_self, int a_value)
	{
		a_self.v_y = a_value;
	}
	static void f_define(t_extension* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(::xemmai::t_object* a_this);
	virtual void f_finalize(::xemmai::t_object* a_this);
	virtual t_transfer f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n);
};

template<>
struct t_type_of<t_extent> : t_type
{
	typedef xraft::xemmai::t_extension t_extension;

	template<typename T_extension, typename T>
	static t_transfer f_transfer(T_extension* a_extension, T a_value)
	{
		t_transfer object = t_object::f_allocate(a_extension->template f_type<typename t_fundamental<T>::t_type>());
		object.f_pointer__(new t_extent(a_value));
		return object;
	}
	static unsigned f_width(t_extent& a_self)
	{
		return a_self.v_width;
	}
	static void f_width__(t_extent& a_self, unsigned a_value)
	{
		a_self.v_width = a_value;
	}
	static unsigned f_height(t_extent& a_self)
	{
		return a_self.v_height;
	}
	static void f_height__(t_extent& a_self, unsigned a_value)
	{
		a_self.v_height = a_value;
	}
	static void f_define(t_extension* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(::xemmai::t_object* a_this);
	virtual void f_finalize(::xemmai::t_object* a_this);
	virtual t_transfer f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n);
};

template<>
struct t_type_of<t_rectangle> : t_type
{
	typedef xraft::xemmai::t_extension t_extension;

	template<typename T_extension, typename T>
	static t_transfer f_transfer(T_extension* a_extension, T a_value)
	{
		t_transfer object = t_object::f_allocate(a_extension->template f_type<typename t_fundamental<T>::t_type>());
		object.f_pointer__(new t_rectangle(a_value));
		return object;
	}
	static int f_x(t_rectangle& a_self)
	{
		return a_self.v_x;
	}
	static void f_x__(t_rectangle& a_self, int a_value)
	{
		a_self.v_x = a_value;
	}
	static int f_y(t_rectangle& a_self)
	{
		return a_self.v_y;
	}
	static void f_y__(t_rectangle& a_self, int a_value)
	{
		a_self.v_y = a_value;
	}
	static unsigned f_width(t_rectangle& a_self)
	{
		return a_self.v_width;
	}
	static void f_width__(t_rectangle& a_self, unsigned a_value)
	{
		a_self.v_width = a_value;
	}
	static unsigned f_height(t_rectangle& a_self)
	{
		return a_self.v_height;
	}
	static void f_height__(t_rectangle& a_self, unsigned a_value)
	{
		a_self.v_height = a_value;
	}
	static void f_define(t_extension* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
	}
	virtual t_type* f_derive(::xemmai::t_object* a_this);
	virtual void f_finalize(::xemmai::t_object* a_this);
	virtual t_transfer f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
