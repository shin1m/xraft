#ifndef XEMMAIX__XRAFT__REGION_H
#define XEMMAIX__XRAFT__REGION_H

#include "xraft.h"

namespace xemmai
{

template<>
struct t_type_of<xraft::t_region> : t_derivable<t_holds<xraft::t_region>>
{
	template<typename T0>
	struct t_as
	{
		template<typename T1>
		static T0 f_call(T1&& a_object)
		{
			xraft::f_application();
			return f_object(std::forward<T1>(a_object))->template f_as<typename t_fundamental<T0>::t_type>();
		}
	};
	using t_library = xemmaix::xraft::t_library;

	template<typename T_library>
	static t_pvalue f_transfer(T_library* a_library, const xraft::t_region& a_value)
	{
		auto object = xemmai::f_new<xraft::t_region>(a_library);
		auto& p = object->template f_as<xraft::t_region>();
		p.f_unite(p, a_value);
		return object;
	}
	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
