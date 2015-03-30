#ifndef XRAFT__XEMMAI__PIXMAP_H
#define XRAFT__XEMMAI__PIXMAP_H

#include "drawable.h"

#include <xemmai/bytes.h>

namespace xemmai
{

using namespace xraft;

template<>
struct t_type_of<t_bitmap> : t_type_of<t_drawable>
{
	static t_scoped f_construct(::xemmai::t_object* a_class, unsigned a_width, unsigned a_height)
	{
		return xraft::xemmai::t_proxy::f_construct(a_class, new t_bitmap(a_width, a_height));
	}
	static t_scoped f_construct(::xemmai::t_object* a_class, unsigned a_width, unsigned a_height, const t_bytes& a_data)
	{
		return f_construct(a_class, a_width, a_height, a_data, 0);
	}
	static t_scoped f_construct(::xemmai::t_object* a_class, unsigned a_width, unsigned a_height, const t_bytes& a_data, int a_bytes_per_line)
	{
		return xraft::xemmai::t_proxy::f_construct(a_class, new t_bitmap(a_width, a_height, reinterpret_cast<const char*>(&a_data[0]), a_bytes_per_line));
	}
	static void f_define(t_extension* a_extension);

	using t_type_of<t_drawable>::t_type_of;
	virtual t_scoped f_construct(::xemmai::t_object* a_class, t_scoped* a_stack, size_t a_n);
};

template<>
struct t_type_of<t_pixmap> : t_type_of<t_drawable>
{
	static t_scoped f_construct(::xemmai::t_object* a_class, unsigned a_width, unsigned a_height)
	{
		return xraft::xemmai::t_proxy::f_construct(a_class, new t_pixmap(a_width, a_height));
	}
	static t_scoped f_construct(::xemmai::t_object* a_class, unsigned a_width, unsigned a_height, bool a_alpha)
	{
		return xraft::xemmai::t_proxy::f_construct(a_class, new t_pixmap(a_width, a_height, a_alpha));
	}
	static t_scoped f_construct(::xemmai::t_object* a_class, unsigned a_width, unsigned a_height, bool a_alpha, const t_bytes& a_data)
	{
		return xraft::xemmai::t_proxy::f_construct(a_class, new t_pixmap(a_width, a_height, a_alpha, reinterpret_cast<const char*>(&a_data[0])));
	}
	static t_scoped f_construct(::xemmai::t_object* a_class, unsigned a_width, unsigned a_height, bool a_alpha, const t_bytes& a_data, int a_bytes_per_line)
	{
		return xraft::xemmai::t_proxy::f_construct(a_class, new t_pixmap(a_width, a_height, a_alpha, reinterpret_cast<const char*>(&a_data[0]), a_bytes_per_line));
	}
	static void f_define(t_extension* a_extension);

	using t_type_of<t_drawable>::t_type_of;
	virtual t_scoped f_construct(::xemmai::t_object* a_class, t_scoped* a_stack, size_t a_n);
};

}

#endif
