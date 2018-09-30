#ifndef XRAFT__WINDOW_WIN32_H
#define XRAFT__WINDOW_WIN32_H

#include <vector>

namespace xraft
{

class t_application;
class t_input_context;
class t_widget;
class t_shell;
class t_frame;
class t_opengl_context;
class t_opengl_widget;

//typedef HCURSOR t_cursor;
typedef intptr_t t_cursor;

class t_window : public t_drawable
{
	friend class t_application;
	friend class t_widget;
	friend class t_shell;
	friend class t_frame;
	friend class t_opengl_context;
	friend class t_opengl_widget;

	static int v_delta;

	static char f_ascii(UINT a_key)
	{
		BYTE state[256];
		GetKeyboardState(state);
		WORD c;
		return ToAscii(a_key, MapVirtualKey(a_key, MAPVK_VK_TO_VSC), state, &c, 0) == 1 ? c : '\0';
	}
	static t_modifier f_modifier()
	{
		int modifier = 0;
		if (GetKeyState(VK_SHIFT) != 0) modifier |= e_modifier__SHIFT;
		if (GetKeyState(VK_CAPITAL) != 0) modifier |= e_modifier__LOCK;
		if (GetKeyState(VK_CONTROL) != 0) modifier |= e_modifier__CONTROL;
		if (GetKeyState(VK_MENU) != 0) modifier |= e_modifier__ALTERNATE;
		return static_cast<t_modifier>(modifier);
	}
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	std::vector<t_pointer<t_widget>> v_children;
	t_rectangle v_geometry;
	bool v_visible;
	t_pointer<t_input_context> v_input_context;
	t_cursor v_cursor;

	void f_create(HWND a_parent, ATOM a_atom, DWORD dwExStyle, DWORD dwStyle);

protected:
	XRAFT__EXPORT t_window();
	XRAFT__EXPORT virtual ~t_window();
	HWND f_hwnd() const
	{
		return static_cast<HWND>(v_handle);
	}
	virtual void f_create() = 0;
	XRAFT__EXPORT virtual void f_destroy();
	XRAFT__EXPORT virtual LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	XRAFT__EXPORT virtual void f_on_move();
	XRAFT__EXPORT virtual void f_on_show();
	XRAFT__EXPORT virtual void f_on_hide();
	XRAFT__EXPORT virtual void f_on_paint(t_graphics& a_g);
	XRAFT__EXPORT virtual void f_on_focus_enter();
	XRAFT__EXPORT virtual void f_on_focus_leave();
	XRAFT__EXPORT virtual void f_on_key_press(t_modifier a_modifier, t_key a_key, char a_ascii);
	XRAFT__EXPORT virtual void f_on_key_release(t_modifier a_modifier, t_key a_key, char a_ascii);
	XRAFT__EXPORT virtual void f_on_input_compose();
	XRAFT__EXPORT virtual void f_on_input_commit(const wchar_t* a_cs, size_t a_n);
	XRAFT__EXPORT virtual t_rectangle f_on_input_spot();
	XRAFT__EXPORT virtual void f_on_button_press(t_modifier a_modifier, t_button a_button, int a_x, int a_y);
	XRAFT__EXPORT virtual void f_on_button_release(t_modifier a_modifier, t_button a_button, int a_x, int a_y);
	XRAFT__EXPORT virtual void f_on_pointer_enter(t_modifier a_modifier, int a_x, int a_y, t_cross_mode a_mode, t_cross_detail a_detail);
	XRAFT__EXPORT virtual void f_on_pointer_leave(t_modifier a_modifier, int a_x, int a_y, t_cross_mode a_mode, t_cross_detail a_detail);
	XRAFT__EXPORT virtual void f_on_pointer_move(t_modifier a_modifier, int a_x, int a_y);

public:
	size_t f_count() const
	{
		return v_children.size();
	}
	const t_pointer<t_widget>& f_at(size_t a_i) const
	{
		return v_children[a_i];
	}
	XRAFT__EXPORT virtual void f_add(const t_pointer<t_widget>& a_widget, size_t a_i = size_t(-1));
	XRAFT__EXPORT virtual void f_remove(size_t a_i);
	const t_rectangle& f_geometry() const
	{
		return v_geometry;
	}
	void f_move(const t_rectangle& a_geometry)
	{
		if (a_geometry == v_geometry) return;
		v_geometry = a_geometry;
		if (v_handle != NULL) MoveWindow(f_hwnd(), a_geometry.v_x, a_geometry.v_y, a_geometry.v_width, a_geometry.v_height, TRUE);
		f_on_move();
	}
	bool f_visible() const
	{
		return v_visible;
	}
	void f_show()
	{
		if (v_visible) return;
		v_visible = true;
		if (v_handle != NULL) ShowWindow(f_hwnd(), SW_SHOW);
		f_on_show();
	}
	void f_hide()
	{
		if (!v_visible) return;
		v_visible = false;
		if (v_handle != NULL) ShowWindow(f_hwnd(), SW_HIDE);
		f_on_hide();
	}
	XRAFT__EXPORT void f_invalidate(int a_x, int a_y, unsigned a_width, unsigned a_height);
	XRAFT__EXPORT void f_scroll(int a_x, int a_y, unsigned a_width, unsigned a_height, int a_dx, int a_dy);
	t_point f_from_screen(const t_point& a_point) const
	{
		POINT p;
		p.x = a_point.v_x;
		p.y = a_point.v_y;
		ScreenToClient(f_hwnd(), &p);
		return t_point(p.x, p.y);
	}
	t_point f_to_screen(const t_point& a_point) const
	{
		POINT p;
		p.x = a_point.v_x;
		p.y = a_point.v_y;
		ClientToScreen(f_hwnd(), &p);
		return t_point(p.x, p.y);
	}
	const t_pointer<t_input_context>& f_input_context() const
	{
		return v_input_context;
	}
	XRAFT__EXPORT void f_input_context__(const t_pointer<t_input_context>& a_input_context);
	t_cursor f_cursor() const
	{
		return v_cursor;
	}
	void f_cursor__(t_cursor a_cursor)
	{
		v_cursor = a_cursor;
	}
};

class t_widget : public t_window
{
	friend class t_window;

	t_window* v_parent;

protected:
	XRAFT__EXPORT virtual void f_create();

public:
	t_widget() : v_parent(0)
	{
		v_visible = true;
	}
	t_pointer<t_window> f_parent() const
	{
		return v_parent;
	}
};

class t_shell : public t_window
{
	friend class t_application;

protected:
	XRAFT__EXPORT virtual void f_create();
	XRAFT__EXPORT virtual LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDropFiles(HDROP hDrop);

public:
	XRAFT__EXPORT virtual void f_on_activate();
	XRAFT__EXPORT virtual void f_on_deactivate();
};

class t_frame : public t_shell
{
	std::wstring v_caption;

protected:
	XRAFT__EXPORT virtual void f_create();
	XRAFT__EXPORT virtual LRESULT OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	t_frame()
	{
		v_geometry = t_rectangle(0, 0, 574, 436);
	}
	XRAFT__EXPORT virtual void f_on_close();
	std::wstring_view f_caption() const
	{
		return v_caption;
	}
	XRAFT__EXPORT void f_caption__(std::wstring_view a_caption);
};

}

#endif
