#ifndef XRAFT__REGION_H
#define XRAFT__REGION_H

#include "geometry.h"

#ifdef XRAFT_X11
#include "region_x11.h"
#endif
#ifdef XRAFT_WIN32
#include "region_win32.h"
#endif

#endif
