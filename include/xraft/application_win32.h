#ifndef XRAFT__APPLICATION_WIN32_H
#define XRAFT__APPLICATION_WIN32_H

#include <vector>

namespace xraft
{

class t_font;
class t_graphics;

class t_application
{
	friend class t_timer;
	friend class t_input_context;
	friend class t_window;
	friend class t_widget;
	friend class t_shell;
	friend class t_frame;
	friend class t_font;
	friend class t_graphics;
	friend class t_opengl_format;
	friend class t_opengl_widget;
	friend class t_opengl_context;
	friend t_application* f_application();

	static XRAFT__EXPORT t_application* v_instance;

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HINSTANCE v_hinstance;
	int v_wsa;
	ATOM v_atom_application;
	ATOM v_atom_window;
	ATOM v_atom_window_with_dc;
	HWND v_hwnd;
	HDC v_hdc;
	BOOL v_loop;
	MSG v_msg;
	t_window* v_pointed;
	t_cursor v_cursor_arrow;
	t_cursor v_cursor_ibeam;
	t_cursor v_cursor_vslide;
	t_cursor v_cursor_hslide;
	t_pointer<t_font> v_font;
	t_pointer<t_input_context> v_input_context;
	std::vector<t_pointer<t_shell> > v_shells;

	void f_pointed__(const t_pointer<t_window>& a_window, int a_x, int a_y, t_cross_mode a_mode);

protected:
	XRAFT__EXPORT virtual void f_wait();

public:
	XRAFT__EXPORT t_application(std::vector<std::string>& a_arguments);
	XRAFT__EXPORT ~t_application();
	XRAFT__EXPORT void f_run();
	void f_exit()
	{
		PostQuitMessage(0);
	}
	void f_post(t_runnable* a_runnable)
	{
		PostMessage(v_hwnd, WM_USER, reinterpret_cast<WPARAM>(a_runnable), 0);
	}
	t_pointer<t_shell> f_active() const
	{
		HWND hwnd = GetActiveWindow();
		return hwnd == NULL ? 0 : reinterpret_cast<t_shell*>(GetWindowLong(hwnd, 0));
	}
	t_pointer<t_window> f_focus() const
	{
		HWND hwnd = GetFocus();
		return hwnd == NULL ? 0 : reinterpret_cast<t_window*>(GetWindowLong(hwnd, 0));
	}
	void f_focus__(const t_pointer<t_window>& a_window)
	{
		SetFocus(a_window ? a_window->f_hwnd() : NULL);
	}
	t_pointer<t_window> f_pointer_grabber() const
	{
		HWND hwnd = GetCapture();
		return hwnd == NULL ? 0 : reinterpret_cast<t_window*>(GetWindowLong(hwnd, 0));
	}
	XRAFT__EXPORT void f_pointer_grabber__(const t_pointer<t_window>& a_window);
	t_point f_pointer() const
	{
		POINT p;
		GetCursorPos(&p);
		return t_point(p.x, p.y);
	}
	const t_pointer<t_input_context>& f_input_context() const
	{
		return v_input_context;
	}
	t_cursor f_cursor_arrow()
	{
		return v_cursor_arrow;
	}
	t_cursor f_cursor_ibeam()
	{
		return v_cursor_ibeam;
	}
	t_cursor f_cursor_vslide()
	{
		return v_cursor_vslide;
	}
	t_cursor f_cursor_hslide()
	{
		return v_cursor_hslide;
	}
	const t_pointer<t_font>& f_font() const
	{
		return v_font;
	}
	size_t f_count() const
	{
		return v_shells.size();
	}
	const t_pointer<t_shell>& f_at(size_t a_i) const
	{
		return v_shells[a_i];
	}
	XRAFT__EXPORT void f_add(const t_pointer<t_shell>& a_shell, size_t a_i = size_t(-1));
	XRAFT__EXPORT void f_remove(size_t a_i);
	t_extent f_screen() const
	{
		return t_extent(GetSystemMetrics(SM_CXFULLSCREEN), GetSystemMetrics(SM_CYFULLSCREEN));
	}
};

}

#endif
