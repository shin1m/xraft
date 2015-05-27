#include <xraft/opengl.h>

namespace xraft
{

t_opengl_format::t_opengl_format(bool a_rgba, bool a_double_buffer, bool a_alpha, bool a_depth, bool a_stencil, bool a_accumulation) :
v_rgba(a_rgba),
v_double_buffer(a_double_buffer),
v_alpha(a_alpha),
v_depth(a_depth),
v_stencil(a_stencil),
v_accumulation(a_accumulation)
{
	v_pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	v_pfd.nVersion = 1;
	v_pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL;
	if (v_double_buffer) v_pfd.dwFlags |= PFD_DOUBLEBUFFER;
	v_pfd.iPixelType = v_rgba ? PFD_TYPE_RGBA : PFD_TYPE_COLORINDEX;
	v_pfd.cColorBits = 24;
	v_pfd.cRedBits = 0;
	v_pfd.cRedShift = 0;
	v_pfd.cGreenBits = 0;
	v_pfd.cGreenShift = 0;
	v_pfd.cBlueBits = 0;
	v_pfd.cBlueShift = 0;
	v_pfd.cAlphaBits = v_alpha ? 8 : 0;
	v_pfd.cAlphaShift = 0;
	v_pfd.cAccumBits = v_accumulation ? 24 : 0;
	v_pfd.cAccumRedBits = 0;
	v_pfd.cAccumGreenBits = 0;
	v_pfd.cAccumBlueBits = 0;
	v_pfd.cAccumAlphaBits = 0;
	v_pfd.cDepthBits = v_depth ? 32 : 0;
	v_pfd.cStencilBits = v_stencil ? 8 : 0;
	v_pfd.cAuxBuffers = 0;
	v_pfd.iLayerType = PFD_MAIN_PLANE;
	v_pfd.bReserved = 0;
	v_pfd.dwLayerMask = 0;
	v_pfd.dwVisibleMask = 0;
	v_pfd.dwDamageMask = 0;
	v_format = ChoosePixelFormat(f_application()->v_hdc, &v_pfd);
	if (v_format == 0) throw t_opengl_exception();
}

void t_opengl_widget::f_create()
{
	t_window::f_create(f_parent()->f_hwnd(), f_application()->v_atom_window_with_dc, 0, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS);
	SetPixelFormat(GetDC(f_hwnd()), v_format->v_format, &v_format->v_pfd);
	f_on_create();
}

void t_opengl_widget::f_destroy()
{
	if (this == f_application()->f_focus()) f_application()->f_focus__(0);
	f_on_destroy();
	t_widget::f_destroy();
}

void t_opengl_widget::f_on_create()
{
}

void t_opengl_widget::f_on_destroy()
{
}

t_opengl_context::~t_opengl_context()
{
	if (v_context != NULL) wglDeleteContext(v_context);
}

}
