#ifndef XRAFT__INPUT_CONTEXT_H
#define XRAFT__INPUT_CONTEXT_H

#include "pointer.h"
#include "object.h"

namespace xraft
{

enum t_input_attribute
{
	c_input_attribute__NONE,
	c_input_attribute__REVERSE,
	c_input_attribute__UNDERLINE,
};

}

#ifdef XRAFT_X11
#include "input_context_x11.h"
#endif
#ifdef XRAFT_WIN32
#include "input_context_win32.h"
#endif

#endif
