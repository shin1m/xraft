#ifndef XEMMAIX__XRAFT__OPENGL_H
#define XEMMAIX__XRAFT__OPENGL_H

#include "window.h"

namespace xemmai
{

template<>
struct t_type_of<xraft::t_opengl_format> : t_bears<xraft::t_opengl_format, t_type_of<xraft::t_object>>
{
	static t_pvalue f_construct(t_type* a_class, bool a_rgba)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_opengl_format(a_rgba));
	}
	static t_pvalue f_construct(t_type* a_class, bool a_rgba, bool a_double_buffer)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_opengl_format(a_rgba, a_double_buffer));
	}
	static t_pvalue f_construct(t_type* a_class, bool a_rgba, bool a_double_buffer, bool a_alpha)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_opengl_format(a_rgba, a_double_buffer, a_alpha));
	}
	static t_pvalue f_construct(t_type* a_class, bool a_rgba, bool a_double_buffer, bool a_alpha, bool a_depth)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_opengl_format(a_rgba, a_double_buffer, a_alpha, a_depth));
	}
	static t_pvalue f_construct(t_type* a_class, bool a_rgba, bool a_double_buffer, bool a_alpha, bool a_depth, bool a_stencil)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_opengl_format(a_rgba, a_double_buffer, a_alpha, a_depth, a_stencil));
	}
	static t_pvalue f_construct(t_type* a_class, bool a_rgba, bool a_double_buffer, bool a_alpha, bool a_depth, bool a_stencil, bool a_accumulation)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_opengl_format(a_rgba, a_double_buffer, a_alpha, a_depth, a_stencil, a_accumulation));
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

template<>
struct t_type_of<xraft::t_opengl_widget> : xemmaix::xraft::t_derivable<t_bears<xraft::t_opengl_widget, t_type_of<xraft::t_widget>>>
{
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

template<>
struct t_type_of<xraft::t_opengl_context> : t_bears<xraft::t_opengl_context, t_type_of<xraft::t_object>>
{
	static t_pvalue f_construct(t_type* a_class, const xraft::t_pointer<xraft::t_opengl_format>& a_format)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_opengl_context(a_format));
	}
	static t_pvalue f_construct(t_type* a_class, const xraft::t_pointer<xraft::t_opengl_format>& a_format, const xraft::t_pointer<xraft::t_opengl_context>& a_share)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_opengl_context(a_format, a_share));
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
