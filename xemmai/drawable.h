#ifndef XRAFT__XEMMAI__DRAWABLE_H
#define XRAFT__XEMMAI__DRAWABLE_H

#include "object.h"

namespace xemmai
{

using namespace xraft;

template<>
struct t_type_of<t_drawable> : t_type_of<xraft::t_object>
{
	static void f_paint(t_drawable& a_self, const t_value& a_callable)
	{
		f_paint(a_self, a_callable, false);
	}
	static void f_paint(t_drawable& a_self, const t_value& a_callable, bool a_include);
	static void f_define(t_extension* a_extension);

	using t_type_of<xraft::t_object>::t_type_of;
};

}

#endif
