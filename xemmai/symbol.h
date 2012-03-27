#ifndef XRAFT__XEMMAI__SYMBOL_H
#define XRAFT__XEMMAI__SYMBOL_H

#include "xraft.h"

namespace xemmai
{

using namespace xraft;

template<>
struct t_type_of<t_key> : t_enum_of<t_key, xraft::xemmai::t_extension>
{
	static void f_define(t_extension* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_base(a_module, a_super)
	{
	}
};

template<>
struct t_type_of<t_modifier> : t_enum_of<t_modifier, xraft::xemmai::t_extension>
{
	static void f_define(t_extension* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_base(a_module, a_super)
	{
	}
};

template<>
struct t_type_of<t_button> : t_enum_of<t_button, xraft::xemmai::t_extension>
{
	static void f_define(t_extension* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_base(a_module, a_super)
	{
	}
};

}

#endif
