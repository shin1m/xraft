#ifndef XRAFT__OPENGL_H
#define XRAFT__OPENGL_H

#include "application.h"

namespace xraft
{

class t_opengl_exception
{
};

}

#ifdef XRAFT_X11
#include "opengl_x11.h"
#endif
#ifdef XRAFT_WIN32
#include "opengl_win32.h"
#endif

#endif
