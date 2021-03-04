#ifndef XEMMAIX__XRAFT__DRAWABLE_H
#define XEMMAIX__XRAFT__DRAWABLE_H

#include "object.h"

namespace xemmai
{

template<>
struct t_type_of<xraft::t_drawable> : t_bears<xraft::t_drawable, t_type_of<xraft::t_object>>
{
	static void f_paint(xraft::t_drawable& a_self, const t_pvalue& a_callable)
	{
		f_paint(a_self, a_callable, false);
	}
	static void f_paint(xraft::t_drawable& a_self, const t_pvalue& a_callable, bool a_include);
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

}

#endif
