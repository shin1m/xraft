#include <xraft/opengl.h>

#include <X11/Xatom.h>

namespace xraft
{

t_opengl_format::~t_opengl_format()
{
	XFree(v_visual);
	if (v_owns_colormap) XFreeColormap(f_application()->f_x11_display(), v_colormap);
}

t_opengl_format::t_opengl_format(bool a_rgba, bool a_double_buffer, bool a_alpha, bool a_depth, bool a_stencil, bool a_accumulation) :
v_rgba(a_rgba),
v_double_buffer(a_double_buffer),
v_alpha(a_alpha),
v_depth(a_depth),
v_stencil(a_stencil),
v_accumulation(a_accumulation),
v_owns_colormap(false),
v_colormap(None)
{
	Display* display = f_application()->f_x11_display();
	int screen = DefaultScreen(display);
	VisualID defaultvid = XVisualIDFromVisual(DefaultVisual(display, screen));
	while (true) {
		int attributes[32];
		size_t n = 0;
		if (a_rgba) {
			attributes[n++] = GLX_RGBA;
			attributes[n++] = GLX_RED_SIZE;
			attributes[n++] = 1;
			attributes[n++] = GLX_GREEN_SIZE;
			attributes[n++] = 1;
			attributes[n++] = GLX_BLUE_SIZE;
			attributes[n++] = 1;
			if (a_alpha) {
				attributes[n++] = GLX_ALPHA_SIZE;
				attributes[n++] = 1;
			}
			if (a_accumulation) {
				attributes[n++] = GLX_ACCUM_RED_SIZE;
				attributes[n++] = 1;
				attributes[n++] = GLX_ACCUM_GREEN_SIZE;
				attributes[n++] = 1;
				attributes[n++] = GLX_ACCUM_BLUE_SIZE;
				attributes[n++] = 1;
				if (a_alpha) {
					attributes[n++] = GLX_ACCUM_ALPHA_SIZE;
					attributes[n++] = 1;
				}
			}
		} else {
			attributes[n++] = GLX_BUFFER_SIZE;
			attributes[n++] = 1;
		}
		if (a_double_buffer) attributes[n++] = GLX_DOUBLEBUFFER;
		if (a_depth) {
			attributes[n++] = GLX_DEPTH_SIZE;
			attributes[n++] = 1;
		}
		if (a_stencil) {
			attributes[n++] = GLX_STENCIL_SIZE;
			attributes[n++] = 1;
		}
		attributes[n] = None;
		v_visual = glXChooseVisual(display, screen, attributes);
		if (v_visual != NULL) break;
		if (a_double_buffer == v_double_buffer) {
			a_double_buffer ^= true;
			continue;
		}
		if (a_accumulation) {
			a_accumulation = false;
		} else if (a_stencil) {
			a_stencil = false;
		} else if (a_depth) {
			a_depth = false;
		} else if (a_alpha) {
			a_alpha = false;
		} else {
			XVisualInfo info;
			info.visualid = defaultvid;
			int nitems;
			v_visual = XGetVisualInfo(display, VisualIDMask, &info, &nitems);
			if (v_visual == NULL) throw t_opengl_exception();
			break;
		}
		a_double_buffer = v_double_buffer;
	}
	VisualID vid = XVisualIDFromVisual(v_visual->visual);
	if (vid == defaultvid) return;
	XStandardColormap* colormaps;
	int count;
	if (XGetRGBColormaps(display, DefaultRootWindow(display), &colormaps, &count, XA_RGB_DEFAULT_MAP) != 0) {
		for (int i = 0; i < count; ++i) {
			if (colormaps[i].visualid == vid) {
				v_colormap = colormaps[i].colormap;
				break;
			}
		}
		XFree(colormaps);
	}
	if (v_colormap == None) {
		v_owns_colormap = true;
		v_colormap = XCreateColormap(display, DefaultRootWindow(display), v_visual->visual, AllocNone);
	}
}

void t_opengl_widget::f_create()
{
	f_x11_create(v_parent->v_handle, 0, v_format->v_visual->depth, v_format->v_visual->visual, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | EnterWindowMask | ExposureMask, true, v_format->v_colormap);
	f_on_create();
}

void t_opengl_widget::f_destroy()
{
	t_application* application = f_application();
	if (this == application->v_focus) application->f_focus__(0);
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
	glXDestroyContext(f_application()->f_x11_display(), v_context);
}

}
