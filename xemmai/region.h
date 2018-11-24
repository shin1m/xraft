#ifndef XEMMAIX__XRAFT__REGION_H
#define XEMMAIX__XRAFT__REGION_H

#include "xraft.h"

namespace xemmai
{

template<>
struct t_type_of<xraft::t_region> : t_derivable<t_holds<xraft::t_region>>
{
	typedef xemmaix::xraft::t_extension t_extension;

	template<typename T_extension>
	static t_scoped f_transfer(T_extension* a_extension, const xraft::t_region& a_value)
	{
		auto object = xemmai::f_new<xraft::t_region>(a_extension, false);
		auto& p = object->template f_as<xraft::t_region>();
		p.f_unite(p, a_value);
		return object;
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
};

}

#endif
