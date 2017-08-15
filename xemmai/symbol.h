#ifndef XEMMAIX__XRAFT__SYMBOL_H
#define XEMMAIX__XRAFT__SYMBOL_H

#include "xraft.h"

namespace xemmai
{

template<>
struct t_type_of<xraft::t_key> : t_enum_of<xraft::t_key, xemmaix::xraft::t_extension>
{
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

template<>
struct t_type_of<xraft::t_modifier> : t_enum_of<xraft::t_modifier, xemmaix::xraft::t_extension>
{
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

template<>
struct t_type_of<xraft::t_button> : t_enum_of<xraft::t_button, xemmaix::xraft::t_extension>
{
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

}

#endif
