#ifndef XRAFT__APPLICATION_H
#define XRAFT__APPLICATION_H

#include "pointer.h"
#include "timer.h"
#include "window.h"

#ifdef XRAFT_X11
#include "application_x11.h"
#endif
#ifdef XRAFT_WIN32
#include "application_win32.h"
#endif

namespace xraft
{

inline t_application* f_application()
{
	if (!t_application::v_instance) throw std::runtime_error("no application instance.");
	return t_application::v_instance;
}

}

#endif
