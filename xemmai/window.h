#ifndef XEMMAIX__XRAFT__WINDOW_H
#define XEMMAIX__XRAFT__WINDOW_H

#include "graphics.h"
#include "input_context.h"
#include "drawable.h"
#include "symbol.h"

namespace xemmai
{

template<>
struct t_type_of<xraft::t_cross_mode> : t_enum_of<xraft::t_cross_mode, xemmaix::xraft::t_extension>
{
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

template<>
struct t_type_of<xraft::t_cross_detail> : t_enum_of<xraft::t_cross_detail, xemmaix::xraft::t_extension>
{
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

template<>
struct t_type_of<xraft::t_window> : t_type_of<xraft::t_drawable>
{
	static void f_add(xraft::t_window& a_self, const xraft::t_pointer<xraft::t_widget>& a_widget)
	{
		if (!a_widget) t_throwable::f_throw(L"widget must not be null.");
		a_self.f_add(a_widget);
	}
	static void f_add(xraft::t_window& a_self, const xraft::t_pointer<xraft::t_widget>& a_widget, size_t a_i)
	{
		if (!a_widget) t_throwable::f_throw(L"widget must not be null.");
		a_self.f_add(a_widget, a_i);
	}
	static void f_define(t_extension* a_extension);

	using t_type_of<xraft::t_drawable>::t_type_of;
};

template<>
struct t_type_of<xraft::t_widget> : t_type_of<xraft::t_window>
{
	static void f_define(t_extension* a_extension);

	using t_type_of<xraft::t_window>::t_type_of;
	virtual t_type* f_derive();
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
};

template<>
struct t_type_of<xraft::t_shell> : t_type_of<xraft::t_window>
{
	static void f_define(t_extension* a_extension);

	using t_type_of<xraft::t_window>::t_type_of;
	virtual t_type* f_derive();
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
};

template<>
struct t_type_of<xraft::t_frame> : t_type_of<xraft::t_shell>
{
	static void f_define(t_extension* a_extension);

	using t_type_of<xraft::t_shell>::t_type_of;
	virtual t_type* f_derive();
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
};

}

#endif
