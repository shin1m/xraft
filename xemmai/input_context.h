#ifndef XEMMAIX__XRAFT__INPUT_CONTEXT_H
#define XEMMAIX__XRAFT__INPUT_CONTEXT_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<xraft::t_input_attribute> : t_enum_of<xraft::t_input_attribute, xemmaix::xraft::t_library>
{
	static t_object* f_define(t_library* a_library);

	using t_base::t_base;
};

template<>
struct t_type_of<xraft::t_input_context> : xemmaix::xraft::t_bears_pointer<xraft::t_input_context, t_type_of<xraft::t_object>>
{
	static t_pvalue f_composition(t_library* a_library, xraft::t_input_context& a_self);
	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
