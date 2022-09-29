#ifndef XRAFT__DRAWABLE_H
#define XRAFT__DRAWABLE_H

#include "object.h"

#ifdef XRAFT_X11
#include <X11/Xlib.h>
#endif
#ifdef XRAFT_WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <windowsx.h>
#include <shellapi.h>
#endif

namespace xraft
{

class t_graphics;

class t_drawable : public t_object
{
#ifdef XRAFT_X11
	using t_handle = Drawable;
#endif
#ifdef XRAFT_WIN32
	using t_handle = HANDLE;
#endif
	friend class t_graphics;

protected:
#ifdef XRAFT_X11
	t_handle v_handle = None;
#endif
#ifdef XRAFT_WIN32
	t_handle v_handle = NULL;
#endif

public:
	t_handle f_handle() const
	{
		return v_handle;
	}
};

}

#endif
