#ifndef XRAFT__WINDOW_X11_H
#define XRAFT__WINDOW_X11_H

#include <string>
#include <X11/Xresource.h>
#include <X11/Xos.h>

namespace xraft
{

class t_application;
class t_input_context;
class t_widget;
class t_shell;

struct t_sink
{
	virtual void f_process(XEvent& a_event) = 0;
};

typedef Cursor t_cursor;

class t_window : public t_drawable, protected t_sink
{
	friend class t_application;
	friend class t_widget;
	friend class t_shell;
	friend class t_frame;
	friend class t_input_context;
	friend class t_opengl_widget;

	static void f_validate();

	std::vector<t_pointer<t_widget>> v_children;
	t_rectangle v_geometry{0, 0, 0, 0};
	bool v_visible = false;
	t_window* v_next_invalid = nullptr;
	t_region v_region;
	t_pointer<t_input_context> v_input_context;
	t_cursor v_cursor;

	void f_queue_invalid();

protected:
	void f_x11_create(Window a_parent, unsigned int a_border_width, int a_depth, Visual* a_visual, long a_event_mask, bool a_override_redirect, Colormap a_colormap);
	std::vector<t_pointer<t_widget>>& f_x11_children()
	{
		return v_children;
	}
	void f_x11_geometry__(const t_rectangle& a_geometry)
	{
		v_geometry = a_geometry;
	}
	void f_x11_visible__(bool a_visible)
	{
		v_visible = a_visible;
	}
	t_window();
	virtual ~t_window();
	virtual void f_create() = 0;
	virtual void f_destroy();
	virtual void f_process(XEvent& a_event);

public:
	virtual void f_on_move();
	virtual void f_on_show();
	virtual void f_on_hide();
	virtual void f_on_paint(t_graphics& a_g);
	virtual void f_on_focus_enter();
	virtual void f_on_focus_leave();
	virtual void f_on_key_press(t_modifier a_modifier, t_key a_key, char a_ascii);
	virtual void f_on_key_release(t_modifier a_modifier, t_key a_key, char a_ascii);
	virtual void f_on_input_compose();
	virtual void f_on_input_commit(const wchar_t* a_cs, size_t a_n);
	virtual t_rectangle f_on_input_spot();
	virtual void f_on_button_press(t_modifier a_modifier, t_button a_button, int a_x, int a_y);
	virtual void f_on_button_release(t_modifier a_modifier, t_button a_button, int a_x, int a_y);
	virtual void f_on_pointer_enter(t_modifier a_modifier, int a_x, int a_y, t_cross_mode a_mode, t_cross_detail a_detail);
	virtual void f_on_pointer_leave(t_modifier a_modifier, int a_x, int a_y, t_cross_mode a_mode, t_cross_detail a_detail);
	virtual void f_on_pointer_move(t_modifier a_modifier, int a_x, int a_y);

public:
	size_t f_count() const
	{
		return v_children.size();
	}
	const t_pointer<t_widget>& f_at(size_t a_i) const
	{
		return v_children[a_i];
	}
	virtual void f_add(const t_pointer<t_widget>& a_widget, size_t a_i = size_t(-1));
	virtual void f_remove(size_t a_i);
	const t_rectangle& f_geometry() const
	{
		return v_geometry;
	}
	void f_move(const t_rectangle& a_geometry);
	bool f_visible() const
	{
		return v_visible;
	}
	void f_show();
	void f_hide();
	void f_invalidate(int a_x, int a_y, unsigned a_width, unsigned a_height)
	{
		f_queue_invalid();
		v_region.f_unite(v_region, a_x, a_y, a_width, a_height);
	}
	void f_scroll(int a_x, int a_y, unsigned a_width, unsigned a_height, int a_dx, int a_dy);
	t_point f_from_screen(const t_point& a_point) const;
	t_point f_to_screen(const t_point& a_point) const;
	const t_pointer<t_input_context>& f_input_context() const
	{
		return v_input_context;
	}
	void f_input_context__(const t_pointer<t_input_context>& a_input_context);
	t_cursor f_cursor() const
	{
		return v_cursor;
	}
	void f_cursor__(t_cursor a_cursor);
};

class t_widget : public t_window
{
	friend class t_window;
	friend class t_opengl_widget;

	t_window* v_parent = nullptr;

protected:
	virtual void f_create();

public:
	t_widget()
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
	friend class t_frame;
	friend class t_input_context;

	static const long v_mask = ButtonPressMask | ButtonReleaseMask | PointerMotionMask | KeyPressMask | KeyReleaseMask | EnterWindowMask | LeaveWindowMask | FocusChangeMask | StructureNotifyMask | ExposureMask;

protected:
	int v_gravity;

	virtual void f_create();
	virtual void f_destroy();
	virtual void f_process(XEvent& a_event);

public:
	virtual void f_on_activate();
	virtual void f_on_deactivate();
};

class t_frame : public t_shell
{
	std::wstring v_caption;

protected:
	virtual void f_create();
	virtual void f_process(XEvent& a_event);

public:
	t_frame()
	{
		v_geometry = t_rectangle(0, 0, 574, 436);
	}
	virtual void f_on_close();
	const std::wstring& f_caption() const
	{
		return v_caption;
	}
	void f_caption__(const std::wstring& a_caption);
};

}

#endif
