#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <X11/Xatom.h>
#include <X11/Xmd.h>
#include <X11/extensions/shape.h>
#include <xraft/application.h>
#include <xraft/graphics.h>
#include <xraft/window.h>

namespace xraft
{

class t_root;

enum t_side
{
	e_side__NONE,
	e_side__NEAR,
	e_side__FAR,
	e_side__BOTH
};

class t_client : public t_widget
{
	friend class t_root;

	struct t_size_hints
	{
		int v_lower;
		int v_upper;
		unsigned v_delta;
		unsigned v_base;

		unsigned f_in_delta(unsigned a_x) const
		{
			return (a_x - v_base) / v_delta;
		}
		unsigned f_adjust(int a_x) const;
	};

	unsigned v_borders[4]{0, 0, 0, 0};
	t_client* v_next_revoked = nullptr;
	Window v_client = None;
	unsigned v_width = 0;
	unsigned v_height = 0;
	unsigned v_border_width = 0;
	Colormap v_colormap = None;
	Bool v_input = False;
	std::wstring v_name;
	t_size_hints v_width_hints;
	t_size_hints v_height_hints;
	int v_gravity = ForgetGravity;
	bool v_take_focus = false;
	bool v_delete_window = false;
	Window v_transient_for;
	bool v_shaped = false;
	bool v_shaded = false;
	unsigned long v_serial = 0;

	t_point f_gravity() const;
	void f_fetch_wm_hints(CARD32* a_initial_state);
	void f_fetch_wm_name();
	void f_fetch_wm_normal_hints();
	void f_fetch_wm_protocols();
	CARD32 f_wm_state() const;
	void f_wm_state__(CARD32 a_wm_state);
	Status f_send_wm_protocols(Atom a_atom, Time a_time = CurrentTime);
	Status f_send_configure();
	void f_focus();
	void f_configure(const XConfigureRequestEvent& a_xconfigurerequest);
	void f_shape();

protected:
	virtual void f_create();
	virtual void f_destroy();
	virtual void f_process(XEvent& a_event);

public:
	virtual void f_on_activate();
	virtual void f_on_deactivate();
	virtual void f_on_name();
	virtual void f_on_protocols();

public:
	t_client()
	{
		f_x11_visible__(false);
	}
	void f_attach(Window a_window, const XWindowAttributes& a_attributes);
	void f_revoke();
	const unsigned* f_borders() const
	{
		return v_borders;
	}
	void f_borders__(const unsigned* a_borders);
	void f_move(t_side a_horizontal, int a_x, t_side a_vertical, int a_y);
	void f_show();
	void f_hide();
	const std::wstring& f_name() const
	{
		return v_name;
	}
	t_extent f_delta() const
	{
		return t_extent(v_width_hints.v_delta, v_height_hints.v_delta);
	}
	t_extent f_base() const
	{
		return t_extent(v_width_hints.v_base, v_height_hints.v_base);
	}
	t_extent f_extent() const
	{
		return t_extent(v_width_hints.f_in_delta(v_width), v_height_hints.f_in_delta(v_height));
	}
	bool f_closable() const
	{
		return v_delete_window;
	}
	void f_close()
	{
		f_send_wm_protocols(f_application()->WM_DELETE_WINDOW, f_application()->f_x11_time());
	}
	bool f_shaded() const
	{
		return v_shaded;
	}
	void f_shaded__(bool a_shaded);
};

class t_root : public t_window, t_runnable
{
	friend class t_client;
	friend t_root* f_root();

	static t_root* v_instance;

	static int f_handle(Display* a_display, XErrorEvent* a_xerror);

	Bool v_shape;
	int v_shape_event;
	XContext XC_CLIENT;
	Atom WM_STATE;
	Atom WM_CHANGE_STATE;
	Atom WM_TAKE_FOCUS;
	Atom WM_COLORMAP_WINDOWS;
	Atom WM_Sn;
	t_client* v_revokeds;
	t_client* v_active;

	t_pointer<t_client> f_find(Window a_window) const
	{
		t_client* p;
		return XFindContext(f_application()->f_x11_display(), a_window, XC_CLIENT, reinterpret_cast<XPointer*>(&p)) == XCSUCCESS ? p : 0;
	}
	void f_register(Window a_window, t_client* a_client)
	{
		XSaveContext(f_application()->f_x11_display(), a_window, XC_CLIENT, reinterpret_cast<XPointer>(a_client));
	}
	void f_revoke(Window a_window)
	{
		XDeleteContext(f_application()->f_x11_display(), a_window, XC_CLIENT);
	}
	virtual void operator()();

protected:
	virtual void f_create();
	virtual void f_process(XEvent& a_event);
	virtual t_pointer<t_client> f_on_client() = 0;

public:
	t_root();
	virtual ~t_root();
	void f_run();
	t_pointer<t_client> f_active() const
	{
		return v_active;
	}
	void f_active__(const t_pointer<t_client>& a_client);
	void f_background(t_pixel a_pixel)
	{
		XSetWindowBackground(f_application()->f_x11_display(), v_handle, a_pixel);
		XClearWindow(f_application()->f_x11_display(), v_handle);
	}
	void f_background(const t_pointer<t_pixmap>& a_pixmap)
	{
		XSetWindowBackgroundPixmap(f_application()->f_x11_display(), v_handle, a_pixmap ? a_pixmap->f_handle() : None);
		XClearWindow(f_application()->f_x11_display(), v_handle);
	}
	void f_share_background(const t_pointer<t_pixmap>& a_pixmap)
	{
		Display* display = f_application()->f_x11_display();
		Atom XRAFT_BACKGROUND = XInternAtom(display, "XRAFT_BACKGROUND", False);
		Pixmap handle = a_pixmap->f_handle();
		XChangeProperty(display, v_handle, XRAFT_BACKGROUND, XA_PIXMAP, 32, PropModeReplace, reinterpret_cast<unsigned char*>(&handle), 1);
	}
};

inline t_root* f_root()
{
	if (!t_root::v_instance) throw std::runtime_error("no root instance.");
	return t_root::v_instance;
}

}

#endif
