#ifndef XEMMAIX__XRAFT__OPENGL_H
#define XEMMAIX__XRAFT__OPENGL_H

#include "window.h"

namespace xemmai
{

template<>
struct t_type_of<xraft::t_opengl_format> : t_type_of<xraft::t_object>
{
	static t_scoped f_construct(xemmai::t_object* a_class, bool a_rgba)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_opengl_format(a_rgba));
	}
	static t_scoped f_construct(xemmai::t_object* a_class, bool a_rgba, bool a_double_buffer)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_opengl_format(a_rgba, a_double_buffer));
	}
	static t_scoped f_construct(xemmai::t_object* a_class, bool a_rgba, bool a_double_buffer, bool a_alpha)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_opengl_format(a_rgba, a_double_buffer, a_alpha));
	}
	static t_scoped f_construct(xemmai::t_object* a_class, bool a_rgba, bool a_double_buffer, bool a_alpha, bool a_depth)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_opengl_format(a_rgba, a_double_buffer, a_alpha, a_depth));
	}
	static t_scoped f_construct(xemmai::t_object* a_class, bool a_rgba, bool a_double_buffer, bool a_alpha, bool a_depth, bool a_stencil)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_opengl_format(a_rgba, a_double_buffer, a_alpha, a_depth, a_stencil));
	}
	static t_scoped f_construct(xemmai::t_object* a_class, bool a_rgba, bool a_double_buffer, bool a_alpha, bool a_depth, bool a_stencil, bool a_accumulation)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_opengl_format(a_rgba, a_double_buffer, a_alpha, a_depth, a_stencil, a_accumulation));
	}
	static void f_define(t_extension* a_extension);

	using t_type_of<xraft::t_object>::t_type_of;
	virtual t_scoped f_construct(xemmai::t_object* a_class, t_stacked* a_stack, size_t a_n);
};

template<>
struct t_type_of<xraft::t_opengl_widget> : t_type_of<xraft::t_widget>
{
	static void f_define(t_extension* a_extension);

	using t_type_of<xraft::t_widget>::t_type_of;
	virtual t_type* f_derive(xemmai::t_object* a_this);
	virtual t_scoped f_construct(xemmai::t_object* a_class, t_stacked* a_stack, size_t a_n);
};

template<>
struct t_type_of<xraft::t_opengl_context> : t_type_of<xraft::t_object>
{
	static t_scoped f_construct(xemmai::t_object* a_class, const xraft::t_pointer<xraft::t_opengl_format>& a_format)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_opengl_context(a_format));
	}
	static t_scoped f_construct(xemmai::t_object* a_class, const xraft::t_pointer<xraft::t_opengl_format>& a_format, const xraft::t_pointer<xraft::t_opengl_context>& a_share)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_opengl_context(a_format, a_share));
	}
	static void f_define(t_extension* a_extension);

	using t_type_of<xraft::t_object>::t_type_of;
	virtual t_scoped f_construct(xemmai::t_object* a_class, t_stacked* a_stack, size_t a_n);
};

}

#endif
