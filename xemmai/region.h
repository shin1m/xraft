#ifndef XRAFT__XEMMAI__REGION_H
#define XRAFT__XEMMAI__REGION_H

#include "xraft.h"

namespace xemmai
{

using namespace xraft;

template<>
struct t_type_of<t_region> : t_type
{
	typedef xraft::xemmai::t_extension t_extension;

	template<typename T_extension>
	static t_scoped f_transfer(T_extension* a_extension, const t_region& a_value)
	{
		t_scoped object = t_object::f_allocate(a_extension->template f_type<t_region>());
		t_region* p = new t_region();
		p->f_unite(*p, a_value);
		object.f_pointer__(p);
		return object;
	}
	static void f_define(t_extension* a_extension);

	using t_type::t_type;
	virtual t_type* f_derive(::xemmai::t_object* a_this);
	virtual void f_finalize(::xemmai::t_object* a_this);
	virtual t_scoped f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
