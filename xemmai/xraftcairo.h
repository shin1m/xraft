#ifndef XRAFT__XEMMAI__XRAFTCAIRO_H
#define XRAFT__XEMMAI__XRAFTCAIRO_H

#include "window.h"
#include "pixmap.h"

#include <cairo/xemmai/context.h>
#include <xraft/cairo.h>

namespace xraft
{

namespace xemmai
{

using ::xemmai::t_object;
using ::xemmai::t_scan;
using ::xemmai::t_transfer;
using ::xemmai::t_slot;

class t_xraftcairo : public ::xemmai::t_extension
{
	template<typename T, typename T_super> friend class t_define;

	t_slot v_module_cairo;
	cairo::xemmai::t_extension* v_cairo;

public:
	t_xraftcairo(t_object* a_module, const t_transfer& a_cairo);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	t_object* f_type() const
	{
		return v_cairo->f_type<T>();
	}
	template<typename T>
	t_transfer f_as(const T& a_value) const
	{
		return v_cairo->f_as(a_value);
	}
};

}

}

#endif
