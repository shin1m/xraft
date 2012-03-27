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

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_base(a_module, a_super)
	{
	}
};

template<>
struct t_type_of<t_input_context> : t_type_of<xraft::t_object>
{
	static t_transfer f_composition(t_extension* a_extension, t_input_context& a_self);
	static void f_define(t_extension* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type_of<xraft::t_object>(a_module, a_super)
	{
	}
	virtual t_transfer f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
