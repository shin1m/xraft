#ifndef XEMMAIX__XRAFT__INPUT_CONTEXT_H
#define XEMMAIX__XRAFT__INPUT_CONTEXT_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<xraft::t_input_attribute> : t_enum_of<xraft::t_input_attribute, xemmaix::xraft::t_extension>
{
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

template<>
struct t_type_of<xraft::t_input_context> : t_bears<xraft::t_input_context, t_type_of<xraft::t_object>>
{
	static t_pvalue f_composition(t_extension* a_extension, xraft::t_input_context& a_self);
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
