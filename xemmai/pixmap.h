#ifndef XEMMAIX__XRAFT__PIXMAP_H
#define XEMMAIX__XRAFT__PIXMAP_H

#include "drawable.h"
#include <xemmai/bytes.h>

namespace xemmai
{

template<>
struct t_type_of<xraft::t_bitmap> : t_bears<xraft::t_bitmap, t_type_of<xraft::t_drawable>>
{
	static t_pvalue f_construct(t_type* a_class, unsigned a_width, unsigned a_height)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_bitmap(a_width, a_height));
	}
	static t_pvalue f_construct(t_type* a_class, unsigned a_width, unsigned a_height, const t_bytes& a_data)
	{
		return f_construct(a_class, a_width, a_height, a_data, 0);
	}
	static t_pvalue f_construct(t_type* a_class, unsigned a_width, unsigned a_height, const t_bytes& a_data, int a_bytes_per_line)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_bitmap(a_width, a_height, reinterpret_cast<const char*>(&a_data[0]), a_bytes_per_line));
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

template<>
struct t_type_of<xraft::t_pixmap> : t_bears<xraft::t_pixmap, t_type_of<xraft::t_drawable>>
{
	static t_pvalue f_construct(t_type* a_class, unsigned a_width, unsigned a_height)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_pixmap(a_width, a_height));
	}
	static t_pvalue f_construct(t_type* a_class, unsigned a_width, unsigned a_height, bool a_alpha)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_pixmap(a_width, a_height, a_alpha));
	}
	static t_pvalue f_construct(t_type* a_class, unsigned a_width, unsigned a_height, bool a_alpha, const t_bytes& a_data)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_pixmap(a_width, a_height, a_alpha, reinterpret_cast<const char*>(&a_data[0])));
	}
	static t_pvalue f_construct(t_type* a_class, unsigned a_width, unsigned a_height, bool a_alpha, const t_bytes& a_data, int a_bytes_per_line)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xraft::t_pixmap(a_width, a_height, a_alpha, reinterpret_cast<const char*>(&a_data[0]), a_bytes_per_line));
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

#endif
