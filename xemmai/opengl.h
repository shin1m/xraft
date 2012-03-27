#ifndef XRAFT__XEMMAI__OPENGL_H
#define XRAFT__XEMMAI__OPENGL_H

#include "window.h"

namespace xemmai
{

template<>
struct t_type_of<t_opengl_format> : t_type_of<xraft::t_object>
{
	static t_transfer f_construct(::xemmai::t_object* a_class, bool a_rgba)
	{
		return xraft::xemmai::t_proxy::f_construct(a_class, new t_opengl_format(a_rgba));
	}
	static t_transfer f_construct(::xemmai::t_object* a_class, bool a_rgba, bool a_double_buffer)
	{
		return xraft::xemmai::t_proxy::f_construct(a_class, new t_opengl_format(a_rgba, a_double_buffer));
	}
	static t_transfer f_construct(::xemmai::t_object* a_class, bool a_rgba, bool a_double_buffer, bool a_alpha)
	{
		return xraft::xemmai::t_proxy::f_construct(a_class, new t_opengl_format(a_rgba, a_double_buffer, a_alpha));
	}
	static t_transfer f_construct(::xemmai::t_object* a_class, bool a_rgba, bool a_double_buffer, bool a_alpha, bool a_depth)
	{
		return xraft::xemmai::t_proxy::f_construct(a_class, new t_opengl_format(a_rgba, a_double_buffer, a_alpha, a_depth));
	}
	static t_transfer f_construct(::xemmai::t_object* a_class, bool a_rgba, bool a_double_buffer, bool a_alpha, bool a_depth, bool a_stencil)
	{
		return xraft::xemmai::t_proxy::f_construct(a_class, new t_opengl_format(a_rgba, a_double_buffer, a_alpha, a_depth, a_stencil));
	}
	static t_transfer f_construct(::xemmai::t_object* a_class, bool a_rgba, bool a_double_buffer, bool a_alpha, bool a_depth, bool a_stencil, bool a_accumulation)
	{
		return xraft::xemmai::t_proxy::f_construct(a_class, new t_opengl_format(a_rgba, a_double_buffer, a_alpha, a_depth, a_stencil, a_accumulation));
	}
	static void f_define(t_extension* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type_of<xraft::t_object>(a_module, a_super)
	{
	}
	virtual t_transfer f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n);
};

template<>
struct t_type_of<t_opengl_widget> : t_type_of<t_widget>
{
	static void f_define(t_extension* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type_of<t_widget>(a_module, a_super)
	{
	}
	virtual t_type* f_derive(::xemmai::t_object* a_this);
	virtual t_transfer f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n);
};

template<>
struct t_type_of<t_opengl_context> : t_type_of<xraft::t_object>
{
	static t_transfer f_construct(::xemmai::t_object* a_class, const xraft::t_pointer<t_opengl_format>& a_format)
	{
		return xraft::xemmai::t_proxy::f_construct(a_class, new t_opengl_context(a_format));
	}
	static t_transfer f_construct(::xemmai::t_object* a_class, const xraft::t_pointer<t_opengl_format>& a_format, const xraft::t_pointer<t_opengl_context>& a_share)
	{
		return xraft::xemmai::t_proxy::f_construct(a_class, new t_opengl_context(a_format, a_share));
	}
	static void f_define(t_extension* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type_of<xraft::t_object>(a_module, a_super)
	{
	}
	virtual t_transfer f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
