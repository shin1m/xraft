#ifndef XRAFT__APPLICATION_X11_H
#define XRAFT__APPLICATION_X11_H

#include <stdexcept>
#include <vector>
#include <unistd.h>
#include <sys/poll.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>

#include "timer.h"
#include "dbus.h"

namespace xraft
{

class t_pixmap;
class t_font;
class t_graphics;
class t_opengl_widget;

class t_application
{
	friend class t_timer;
	friend class t_input_context;
	friend class t_window;
	friend class t_shell;
	friend class t_frame;
	friend class t_pixmap;
	friend class t_font;
	friend class t_graphics;
	friend class t_opengl_widget;
	friend t_application* f_application();

	struct t_listener
	{
		t_runnable* v_reader;
		t_runnable* v_writer;
	};

	static thread_local t_application* v_instance;

	static void f_terminate(int a_signal);

	XContext XC_SINK;
	Display* v_display;
	GC v_gc;
	GC v_gc_save;
	Pixmap v_pixmap;
	GC v_gc_mask;
	unsigned long v_red_mask;
	unsigned long v_green_mask;
	unsigned long v_blue_mask;
	bool v_allocateds[256];
	unsigned long v_reds[256];
	unsigned long v_greens[256];
	unsigned long v_blues[256];
	XOM v_om;
	XOC v_oc;
	size_t v_loop;
	std::vector<struct pollfd> v_pollfds;
	Time v_time;
	int v_post;
	std::vector<t_listener> v_listeners;
	size_t v_notifier;
	t_timer* v_timer;
	t_window* v_invalids;
	t_shell* v_active;
	t_window* v_focus;
	t_window* v_pointer_grabber;
	t_cursor v_cursor_x;
	t_cursor v_cursor_arrow;
	t_cursor v_cursor_ibeam;
	t_cursor v_cursor_vslide;
	t_cursor v_cursor_hslide;
	t_cursor v_cursor_top;
	t_cursor v_cursor_bottom;
	t_cursor v_cursor_left;
	t_cursor v_cursor_right;
	t_cursor v_cursor_top_left;
	t_cursor v_cursor_top_right;
	t_cursor v_cursor_bottom_left;
	t_cursor v_cursor_bottom_right;
	t_pointer<t_font> v_font;
	dbus::t_connection v_input_bus;
	t_pointer<t_input_context::t_component> v_input_first;
	t_pointer<t_input_context::t_component> v_input_middle;
	t_pointer<t_input_context::t_component> v_input_last;
	bool v_input_negotiating;
	bool v_input_composing;
	t_pointer<t_input_context> v_input_context;
	std::vector<t_pointer<t_shell> > v_shells;
	int v_x;
	int v_y;
	unsigned int v_width;
	unsigned int v_height;
	int v_gravity;

	void f_allocate(unsigned char a_i);
	void f_active__(const t_pointer<t_shell>& a_shell);
	t_cursor& f_cursor(t_cursor& a_cursor, unsigned int a_shape)
	{
		if (a_cursor == None) a_cursor = XCreateFontCursor(v_display, a_shape);
		return a_cursor;
	}
	dbus::t_connection& f_input_bus();
	void f_run_post();
	void f_loop();

protected:
	virtual void f_wait();

public:
	Atom WM_PROTOCOLS;
	Atom WM_DELETE_WINDOW;

	Display* f_x11_display()
	{
		return v_display;
	}
	unsigned long f_x11_red(unsigned char a_i)
	{
		if (!v_allocateds[a_i]) f_allocate(a_i);
		return v_reds[a_i];
	}
	unsigned long f_x11_green(unsigned char a_i)
	{
		if (!v_allocateds[a_i]) f_allocate(a_i);
		return v_greens[a_i];
	}
	unsigned long f_x11_blue(unsigned char a_i)
	{
		if (!v_allocateds[a_i]) f_allocate(a_i);
		return v_blues[a_i];
	}

	t_application(std::vector<std::string>& a_arguments);
	~t_application();
	t_sink* f_find(Window a_window) const
	{
		t_sink* p;
		return XFindContext(v_display, a_window, XC_SINK, reinterpret_cast<XPointer*>(&p)) == XCSUCCESS ? p : 0;
	}
	void f_dispatch(XEvent& a_event);
	void f_register(Window a_window, t_sink* a_sink)
	{
		XSaveContext(v_display, a_window, XC_SINK, reinterpret_cast<XPointer>(a_sink));
	}
	void f_revoke(Window a_window)
	{
		XDeleteContext(v_display, a_window, XC_SINK);
	}
	void f_run();
	void f_exit()
	{
		if (v_loop > 0) --v_loop;
	}
	Time f_x11_time() const
	{
		return v_time;
	}
	void f_poll(int a_descriptor, t_runnable* a_reader, t_runnable* a_writer);
	void f_poll_reader(int a_descriptor, t_runnable* a_reader);
	void f_poll_writer(int a_descriptor, t_runnable* a_writer);
	void f_post(t_runnable* a_runnable)
	{
		write(v_post, &a_runnable, sizeof(t_runnable*));
	}
	t_pointer<t_shell> f_active() const
	{
		return v_active;
	}
	t_pointer<t_window> f_focus() const
	{
		return v_focus;
	}
	void f_focus__(const t_pointer<t_window>& a_window);
	t_pointer<t_window> f_pointer_grabber() const
	{
		return v_pointer_grabber;
	}
	void f_pointer_grabber__(const t_pointer<t_window>& a_window);
	t_point f_pointer() const
	{
		Window root;
		Window child;
		int root_x;
		int root_y;
		int win_x;
		int win_y;
		unsigned int mask;
		XQueryPointer(v_display, DefaultRootWindow(v_display), &root, &child, &root_x, &root_y, &win_x, &win_y, &mask);
		return t_point(root_x, root_y);
	}
	const t_pointer<t_input_context>& f_input_context() const
	{
		return v_input_context;
	}
	t_cursor f_cursor_x()
	{
		return f_cursor(v_cursor_x, XC_X_cursor);
	}
	t_cursor f_cursor_arrow()
	{
		return f_cursor(v_cursor_arrow, XC_left_ptr);
	}
	t_cursor f_cursor_ibeam()
	{
		return f_cursor(v_cursor_ibeam, XC_xterm);
	}
	t_cursor f_cursor_vslide()
	{
		return f_cursor(v_cursor_vslide, XC_sb_v_double_arrow);
	}
	t_cursor f_cursor_hslide()
	{
		return f_cursor(v_cursor_hslide, XC_sb_h_double_arrow);
	}
	t_cursor f_cursor_top()
	{
		return f_cursor(v_cursor_top, XC_top_side);
	}
	t_cursor f_cursor_bottom()
	{
		return f_cursor(v_cursor_bottom, XC_bottom_side);
	}
	t_cursor f_cursor_left()
	{
		return f_cursor(v_cursor_left, XC_left_side);
	}
	t_cursor f_cursor_right()
	{
		return f_cursor(v_cursor_right, XC_right_side);
	}
	t_cursor f_cursor_top_left()
	{
		return f_cursor(v_cursor_top_left, XC_top_left_corner);
	}
	t_cursor f_cursor_top_right()
	{
		return f_cursor(v_cursor_top_right, XC_top_right_corner);
	}
	t_cursor f_cursor_bottom_left()
	{
		return f_cursor(v_cursor_bottom_left, XC_bottom_left_corner);
	}
	t_cursor f_cursor_bottom_right()
	{
		return f_cursor(v_cursor_bottom_right, XC_bottom_right_corner);
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
	void f_add(const t_pointer<t_shell>& a_shell, size_t a_i = size_t(-1));
	void f_remove(size_t a_i);
	t_extent f_screen() const
	{
		int screen = DefaultScreen(v_display);
		return t_extent(DisplayWidth(v_display, screen), DisplayHeight(v_display, screen));
	}
	void f_input_negotiate()
	{
		v_input_negotiating = true;
	}
};

namespace dbus
{

class t_bridge
{
	struct t_watch : t_runnable
	{
		DBusConnection* v_connection;
		DBusWatch* v_watch;
		unsigned int v_flags;

		t_watch(DBusConnection* a_connection, DBusWatch* a_watch, unsigned int a_flags) : v_connection(a_connection), v_watch(a_watch), v_flags(a_flags)
		{
		}
		virtual void operator()();
	};
	struct t_timeout : t_timer
	{
		DBusTimeout* v_timeout;

		t_timeout(DBusTimeout* a_timeout) : v_timeout(a_timeout)
		{
		}
		virtual void operator()();
	};

	static thread_local dbus_int32_t v_slot;

	static void f_free(void* a_data);
	static dbus_bool_t f_add(DBusWatch* a_watch, void* a_data);
	static void f_remove(DBusWatch* a_watch, void* a_data);
	static void f_toggle(DBusWatch* a_watch, void* a_data);
	static dbus_bool_t f_add(DBusTimeout* a_timeout, void* a_data);
	static void f_remove(DBusTimeout* a_timeout, void* a_data);
	static void f_toggle(DBusTimeout* a_timeout, void* a_data);

	DBusConnection* v_connection;
	std::map<DBusWatch*, std::pair<t_watch, t_watch> > v_watches;
	std::map<DBusTimeout*, t_pointer<t_timeout> > v_timeouts;

	~t_bridge()
	{
		dbus_connection_free_data_slot(&v_slot);
	}

public:
	t_bridge(DBusConnection* a_connection);
};

}

}

#endif
