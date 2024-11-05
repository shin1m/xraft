#ifndef XRAFT__WINDOW_H
#define XRAFT__WINDOW_H

#include "symbol.h"
#include "drawable.h"
#include "region.h"
#include "input_context.h"

namespace xraft
{

enum t_cross_mode
{
	c_cross_mode__NORMAL,
	c_cross_mode__GRAB,
	c_cross_mode__UNGRAB
};

enum t_cross_detail
{
	c_cross_detail__OUTER,
	c_cross_detail__INNER,
	c_cross_detail__THROUGH
};

}

#ifdef XRAFT_X11
#include "window_x11.h"
#endif
#ifdef XRAFT_WIN32
#include "window_win32.h"
#endif

#endif
