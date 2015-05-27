#ifndef XRAFT__OPENGL_WIN32_H
#define XRAFT__OPENGL_WIN32_H

namespace xraft
{

class t_opengl_format : public t_object
{
	friend class t_opengl_context;
	friend class t_opengl_widget;

	bool v_rgba;
	bool v_double_buffer;
	bool v_alpha;
	bool v_depth;
	bool v_stencil;
	bool v_accumulation;
	PIXELFORMATDESCRIPTOR v_pfd;
	int v_format;

public:
	XRAFT__EXPORT t_opengl_format(bool a_rgba = false, bool a_double_buffer = false, bool a_alpha = false, bool a_depth = false, bool a_stencil = false, bool a_accumulation = false);
};

class t_opengl_widget : public t_widget
{
	t_pointer<t_opengl_format> v_format;

protected:
	XRAFT__EXPORT virtual void f_create();
	XRAFT__EXPORT virtual void f_destroy();

public:
	explicit t_opengl_widget(const t_pointer<t_opengl_format>& a_format) : v_format(a_format)
	{
	}
	XRAFT__EXPORT virtual void f_on_create();
	XRAFT__EXPORT virtual void f_on_destroy();
	const t_pointer<t_opengl_format>& f_format() const
	{
		return v_format;
	}
	bool f_created() const
	{
		return v_handle != NULL;
	}
};

class t_opengl_context : public t_object
{
	HGLRC v_context;

	XRAFT__EXPORT virtual ~t_opengl_context();

public:
	explicit t_opengl_context(const t_pointer<t_opengl_format>& a_format, const t_pointer<t_opengl_context>& a_share = 0)
	{
		HDC hdc = GetDC(f_application()->v_hwnd);
		SetPixelFormat(hdc, a_format->v_format, &a_format->v_pfd);
		v_context = wglCreateContext(hdc);
		ReleaseDC(f_application()->v_hwnd, hdc);
		if (v_context == NULL) throw t_opengl_exception();
		if (a_share) wglShareLists(a_share->v_context, v_context);
	}
	bool f_make_current(const t_pointer<t_opengl_widget>& a_widget)
	{
		return wglMakeCurrent(GetDC(a_widget->f_hwnd()), v_context) == TRUE;
	}
	void f_flush()
	{
		SwapBuffers(wglGetCurrentDC());
	}
};

}

#endif
