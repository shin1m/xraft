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
	typedef Drawable t_handle;
#endif
#ifdef XRAFT_WIN32
	typedef HANDLE t_handle;
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
