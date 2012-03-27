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
	e_cross_mode__NORMAL,
	e_cross_mode__GRAB,
	e_cross_mode__UNGRAB
};

enum t_cross_detail
{
	e_cross_detail__OUTER,
	e_cross_detail__INNER,
	e_cross_detail__THROUGH
};

}

#ifdef XRAFT_X11
#include "window_x11.h"
#endif
#ifdef XRAFT_WIN32
#include "window_win32.h"
#endif

#endif
