#ifndef XRAFT__OPENGL_X11_H
#define XRAFT__OPENGL_X11_H

#include <GL/glx.h>

namespace xraft
{

class t_opengl_widget;
class t_opengl_context;

class t_opengl_format : public t_object
{
	friend class t_opengl_widget;
	friend class t_opengl_context;

	bool v_rgba;
	bool v_double_buffer;
	bool v_alpha;
	bool v_depth;
	bool v_stencil;
	bool v_accumulation;
	XVisualInfo* v_visual;
	bool v_owns_colormap = false;
	Colormap v_colormap = None;

	virtual ~t_opengl_format();

public:
	t_opengl_format(bool a_rgba = false, bool a_double_buffer = false, bool a_alpha = false, bool a_depth = false, bool a_stencil = false, bool a_accumulation = false);
};

class t_opengl_widget : public t_widget
{
	friend class t_opengl_context;

	t_pointer<t_opengl_format> v_format;

protected:
	virtual void f_create();
	virtual void f_destroy();

public:
	explicit t_opengl_widget(const t_pointer<t_opengl_format>& a_format) : v_format(a_format)
	{
	}
	virtual void f_on_create();
	virtual void f_on_destroy();
	const t_pointer<t_opengl_format>& f_format() const
	{
		return v_format;
	}
	bool f_created() const
	{
		return v_handle != None;
	}
};

class t_opengl_context : public t_object
{
	GLXContext v_context;

	virtual ~t_opengl_context();

public:
	explicit t_opengl_context(const t_pointer<t_opengl_format>& a_format, const t_pointer<t_opengl_context>& a_share = 0) : v_context(glXCreateContext(f_application()->f_x11_display(), a_format->v_visual, a_share ? a_share->v_context : NULL, True))
	{
		if (v_context == NULL) throw t_opengl_exception();
	}
	bool f_make_current(const t_pointer<t_opengl_widget>& a_widget)
	{
		return glXMakeCurrent(f_application()->f_x11_display(), a_widget->v_handle, v_context) == True;
	}
	void f_flush()
	{
		glXSwapBuffers(f_application()->f_x11_display(), glXGetCurrentDrawable());
	}
};

}

#endif
