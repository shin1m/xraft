#ifndef XRAFT__GRAPHICS_H
#define XRAFT__GRAPHICS_H

#include "object.h"
#include "application.h"
#include "region.h"
#include "pixmap.h"

#ifdef XRAFT_X11
#include "graphics_x11.h"
#endif
#ifdef XRAFT_WIN32
#include "graphics_win32.h"
#endif

#endif
