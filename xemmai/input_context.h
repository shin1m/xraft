#ifndef XRAFT__XEMMAI__INPUT_CONTEXT_H
#define XRAFT__XEMMAI__INPUT_CONTEXT_H

#include "object.h"

namespace xemmai
{

using namespace xraft;

template<>
struct t_type_of<t_input_attribute> : t_enum_of<t_input_attribute, xraft::xemmai::t_extension>
{
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

template<>
struct t_type_of<t_input_context> : t_type_of<xraft::t_object>
{
	static t_scoped f_composition(t_extension* a_extension, t_input_context& a_self);
	static void f_define(t_extension* a_extension);

	using t_type_of<xraft::t_object>::t_type_of;
	virtual t_scoped f_construct(::xemmai::t_object* a_class, t_scoped* a_stack, size_t a_n);
};

}

#endif
