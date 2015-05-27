#include <xraft/window.h>

#include <X11/keysym.h>
#include <xraft/graphics.h>
#include <xraft/input_context.h>

namespace xraft
{

void t_window::f_validate()
{
	t_window*& invalids = f_application()->v_invalids;
	if (!invalids) return;
	t_window* window = invalids->v_next_invalid;
	do {
		if (!window->v_region.f_empty()) {
			t_graphics g(window);
			g.f_clip(window->v_region);
			window->f_on_paint(g);
			window->v_region.f_clear();
		}
		invalids->v_next_invalid = window->v_next_invalid;
		window->v_next_invalid = 0;
		window = invalids->v_next_invalid;
	} while (window);
	invalids = 0;
}

void t_window::f_queue_invalid()
{
	if (v_next_invalid) return;
	t_window*& invalids = f_application()->v_invalids;
	if (invalids) {
		v_next_invalid = invalids->v_next_invalid;
		invalids->v_next_invalid = this;
	} else {
		v_next_invalid = this;
	}
	invalids = this;
}

void t_window::f_x11_create(Window a_parent, unsigned int a_border_width, int a_depth, Visual* a_visual, long a_event_mask, bool a_override_redirect, Colormap a_colormap)
{
	unsigned long valuemask = CWEventMask | CWOverrideRedirect | CWCursor;
	XSetWindowAttributes attributes;
	attributes.event_mask = a_event_mask;
	attributes.override_redirect = a_override_redirect ? True : False;
	attributes.cursor = v_cursor;
	if (a_colormap != None) {
		valuemask |= CWColormap;
		attributes.colormap = a_colormap;
	}
	t_application* application = f_application();
	Display* display = application->f_x11_display();
	v_handle = XCreateWindow(display, a_parent, v_geometry.v_x, v_geometry.v_y, v_geometry.v_width > 0 ? v_geometry.v_width : 1, v_geometry.v_height > 0 ? v_geometry.v_height : 1, a_border_width, a_depth, InputOutput, a_visual, valuemask, &attributes);
	application->f_register(v_handle, this);
	for (const auto& p : v_children) p->f_create();
	if (v_visible && v_geometry.v_width > 0 && v_geometry.v_height > 0) XMapWindow(display, v_handle);
}

t_window::t_window() : v_cursor(f_application()->f_cursor_arrow())
{
}

t_window::~t_window()
{
	for (const auto& p : v_children) p->v_parent = 0;
}

void t_window::f_destroy()
{
	t_application* application = f_application();
	if (this == application->v_focus) application->f_focus__(0);
	if (this == application->v_pointer_grabber) application->v_pointer_grabber = 0;
	for (const auto& p : v_children) p->f_destroy();
	if (v_next_invalid) {
		t_window*& invalids = application->v_invalids;
		if (v_next_invalid == this) {
			invalids = 0;
		} else {
			t_window* p = invalids;
			while (p->v_next_invalid != this) p = p->v_next_invalid;
			p->v_next_invalid = v_next_invalid;
			if (this == invalids) invalids = p;
		}
		v_next_invalid = 0;
		v_region.f_clear();
	}
	application->f_revoke(v_handle);
	XDestroyWindow(application->f_x11_display(), v_handle);
	v_handle = None;
}

void t_window::f_process(XEvent& a_event)
{
	switch (a_event.type) {
	case ButtonPress:
		f_application()->v_pointer_grabber = this;
		f_on_button_press(static_cast<t_modifier>(a_event.xbutton.state), static_cast<t_button>(a_event.xbutton.button), a_event.xbutton.x, a_event.xbutton.y);
		break;
	case ButtonRelease:
		f_on_button_release(static_cast<t_modifier>(a_event.xbutton.state), static_cast<t_button>(a_event.xbutton.button), a_event.xbutton.x, a_event.xbutton.y);
		f_application()->v_pointer_grabber = 0;
		break;
	case MotionNotify:
		f_on_pointer_move(static_cast<t_modifier>(a_event.xmotion.state), a_event.xmotion.x, a_event.xmotion.y);
		break;
	case EnterNotify:
	case LeaveNotify:
		{
			t_cross_mode mode;
			switch (a_event.xcrossing.mode) {
			case NotifyGrab:
				mode = e_cross_mode__GRAB;
				break;
			case NotifyUngrab:
				mode = e_cross_mode__UNGRAB;
				break;
			default:
				mode = e_cross_mode__NORMAL;
			}
			t_cross_detail detail;
			switch (a_event.xcrossing.detail) {
			case NotifyVirtual:
			case NotifyNonlinearVirtual:
				detail = e_cross_detail__THROUGH;
				break;
			case NotifyInferior:
				detail = e_cross_detail__INNER;
				break;
			default:
				detail = e_cross_detail__OUTER;
			}
			if (a_event.type == EnterNotify)
				f_on_pointer_enter(static_cast<t_modifier>(a_event.xcrossing.state), a_event.xcrossing.x, a_event.xcrossing.y, mode, detail);
			else
				f_on_pointer_leave(static_cast<t_modifier>(a_event.xcrossing.state), a_event.xcrossing.x, a_event.xcrossing.y, mode, detail);
		}
		break;
	case Expose:
		f_invalidate(a_event.xexpose.x, a_event.xexpose.y, a_event.xexpose.width, a_event.xexpose.height);
		break;
	}
}

void t_window::f_on_move()
{
}

void t_window::f_on_show()
{
}

void t_window::f_on_hide()
{
}

void t_window::f_on_paint(t_graphics& a_g)
{
}

void t_window::f_on_focus_enter()
{
}

void t_window::f_on_focus_leave()
{
}

void t_window::f_on_key_press(t_modifier a_modifier, t_key a_key, char a_ascii)
{
}

void t_window::f_on_key_release(t_modifier a_modifier, t_key a_key, char a_ascii)
{
}

void t_window::f_on_input_compose()
{
	f_application()->v_input_composing = true;
}

void t_window::f_on_input_commit(const wchar_t* a_cs, size_t a_n)
{
}

t_rectangle t_window::f_on_input_spot()
{
	return t_rectangle(0, 0, 0, f_application()->f_font()->f_height());
}

void t_window::f_on_button_press(t_modifier a_modifier, t_button a_button, int a_x, int a_y)
{
}

void t_window::f_on_button_release(t_modifier a_modifier, t_button a_button, int a_x, int a_y)
{
}

void t_window::f_on_pointer_enter(t_modifier a_modifier, int a_x, int a_y, t_cross_mode a_mode, t_cross_detail a_detail)
{
}

void t_window::f_on_pointer_leave(t_modifier a_modifier, int a_x, int a_y, t_cross_mode a_mode, t_cross_detail a_detail)
{
}

void t_window::f_on_pointer_move(t_modifier a_modifier, int a_x, int a_y)
{
}

void t_window::f_add(const t_pointer<t_widget>& a_widget, size_t a_i)
{
	if (a_i == size_t(-1)) a_i = v_children.size();
	if (a_widget->v_parent) {
		std::vector<t_pointer<t_widget>>& children = a_widget->v_parent->v_children;
		size_t i = 0;
		while (children[i] != a_widget) ++i;
		children.erase(children.begin() + i);
		if (a_widget->v_parent == this && a_i > i) --a_i;
	}
	Display* display = f_application()->f_x11_display();
	if (a_widget->v_parent != this) {
		a_widget->v_parent = this;
		if (a_widget->v_handle == None) {
			if (v_handle != None) a_widget->f_create();
		} else if (v_handle == None) {
			a_widget->f_destroy();
		} else {
			XReparentWindow(display, a_widget->v_handle, v_handle, a_widget->v_geometry.v_x, a_widget->v_geometry.v_y);
		}
	}
	if (v_handle != None) {
		if (a_i < v_children.size()) {
			XWindowChanges values;
			values.sibling = v_children[a_i]->v_handle;
			values.stack_mode = Below;
			XConfigureWindow(display, a_widget->v_handle, CWSibling | CWStackMode, &values);
		} else {
			XRaiseWindow(display, a_widget->v_handle);
		}
	}
	v_children.insert(v_children.begin() + a_i, a_widget);
}

void t_window::f_remove(size_t a_i)
{
	auto i = v_children.begin() + a_i;
	const t_pointer<t_widget>& widget = *i;
	if (v_handle != None) widget->f_destroy();
	widget->v_parent = 0;
	v_children.erase(i);
}

void t_window::f_move(const t_rectangle& a_geometry)
{
	if (a_geometry == v_geometry) return;
	bool b = v_geometry.v_width > 0 && v_geometry.v_height > 0;
	v_geometry = a_geometry;
	if (v_handle != None) {
		Display* display = f_application()->f_x11_display();
		XMoveResizeWindow(display, v_handle, v_geometry.v_x, v_geometry.v_y, v_geometry.v_width > 0 ? v_geometry.v_width : 1, v_geometry.v_height > 0 ? v_geometry.v_height : 1);
		if (v_visible && v_geometry.v_width > 0 && v_geometry.v_height > 0) {
			if (!b) XMapWindow(display, v_handle);
		} else {
			if (b) XUnmapWindow(display, v_handle);
		}
	}
	f_on_move();
}

void t_window::f_show()
{
	if (v_visible) return;
	v_visible = true;
	if (v_handle != None && v_geometry.v_width > 0 && v_geometry.v_height > 0) XMapWindow(f_application()->f_x11_display(), v_handle);
	f_on_show();
}

void t_window::f_hide()
{
	if (!v_visible) return;
	v_visible = false;
	if (v_handle != None) XUnmapWindow(f_application()->f_x11_display(), v_handle);
	f_on_hide();
}

void t_window::f_scroll(int a_x, int a_y, unsigned a_width, unsigned a_height, int a_dx, int a_dy)
{
	if (a_dx == 0 && a_dy == 0) return;
//	if (!v_region.v_region) v_region.v_region = XCreateRegion();
	Region region = v_region.v_region;
	if (XRectInRegion(region, a_x, a_y, a_width, a_height) == RectangleIn) return;
	XRectangle rectangle;
	int x0;
	int x1;
	unsigned w;
	if (a_dx > 0) {
		if (a_dx >= static_cast<int>(a_width)) {
			rectangle.x = a_x;
			rectangle.y = a_y;
			rectangle.width = a_width;
			rectangle.height = a_height;
			XUnionRectWithRegion(&rectangle, region, region);
			if (!v_region.f_empty()) f_queue_invalid();
			return;
		}
		x0 = a_x;
		x1 = a_x + a_dx;
		w = a_width - a_dx;
	} else {
		if (-a_dx >= static_cast<int>(a_width)) {
			rectangle.x = a_x;
			rectangle.y = a_y;
			rectangle.width = a_width;
			rectangle.height = a_height;
			XUnionRectWithRegion(&rectangle, region, region);
			if (!v_region.f_empty()) f_queue_invalid();
			return;
		}
		x0 = a_x - a_dx;
		x1 = a_x;
		w = a_width + a_dx;
	}
	int y0;
	int y1;
	unsigned h;
	if (a_dy > 0) {
		if (a_dy >= static_cast<int>(a_height)) {
			rectangle.x = a_x;
			rectangle.y = a_y;
			rectangle.width = a_width;
			rectangle.height = a_height;
			XUnionRectWithRegion(&rectangle, region, region);
			if (!v_region.f_empty()) f_queue_invalid();
			return;
		}
		y0 = a_y;
		y1 = a_y + a_dy;
		h = a_height - a_dy;
	} else {
		if (-a_dy >= static_cast<int>(a_height)) {
			rectangle.x = a_x;
			rectangle.y = a_y;
			rectangle.width = a_width;
			rectangle.height = a_height;
			XUnionRectWithRegion(&rectangle, region, region);
			if (!v_region.f_empty()) f_queue_invalid();
			return;
		}
		y0 = a_y - a_dy;
		y1 = a_y;
		h = a_height + a_dy;
	}
	t_application* application = f_application();
	Display* display = application->f_x11_display();
	XCopyArea(display, v_handle, v_handle, application->v_gc, x0, y0, w, h, x1, y1);
	XEvent event;
	while (true) {
		XWindowEvent(display, v_handle, ExposureMask, &event);
		if (event.type != Expose) break;
		rectangle.x = event.xexpose.x;
		rectangle.y = event.xexpose.y;
		rectangle.width = event.xexpose.width;
		rectangle.height = event.xexpose.height;
		XUnionRectWithRegion(&rectangle, region, region);
	}
	rectangle.x = a_x;
	rectangle.y = a_y;
	rectangle.width = a_width;
	rectangle.height = a_height;
	Region r0 = XCreateRegion();
	XUnionRectWithRegion(&rectangle, r0, r0);
	Region r1 = XCreateRegion();
	XIntersectRegion(region, r0, r1);
	XSubtractRegion(region, r0, region);
	XOffsetRegion(r1, a_dx, a_dy);
	XIntersectRegion(r1, r0, r1);
	XUnionRegion(region, r1, region);
	XDestroyRegion(r1);
	r1 = XCreateRegion();
	rectangle.x += a_dx;
	rectangle.y += a_dy;
	XUnionRectWithRegion(&rectangle, r1, r1);
	XSubtractRegion(r0, r1, r0);
	XDestroyRegion(r1);
	XUnionRegion(region, r0, region);
	XDestroyRegion(r0);
	while (event.type == GraphicsExpose) {
		rectangle.x = event.xgraphicsexpose.x;
		rectangle.y = event.xgraphicsexpose.y;
		rectangle.width = event.xgraphicsexpose.width;
		rectangle.height = event.xgraphicsexpose.height;
		XUnionRectWithRegion(&rectangle, region, region);
		if (event.xgraphicsexpose.count <= 0) break;
		XWindowEvent(display, v_handle, ExposureMask, &event);
	}
	if (!v_region.f_empty()) f_queue_invalid();
}

t_point t_window::f_from_screen(const t_point& a_point) const
{
	Display* display = f_application()->f_x11_display();
	int dest_x;
	int dest_y;
	Window child;
	XTranslateCoordinates(display, DefaultRootWindow(display), v_handle, a_point.v_x, a_point.v_y, &dest_x, &dest_y, &child);
	return t_point(dest_x, dest_y);
}

t_point t_window::f_to_screen(const t_point& a_point) const
{
	Display* display = f_application()->f_x11_display();
	int dest_x;
	int dest_y;
	Window child;
	XTranslateCoordinates(display, v_handle, DefaultRootWindow(display), a_point.v_x, a_point.v_y, &dest_x, &dest_y, &child);
	return t_point(dest_x, dest_y);
}

void t_window::f_input_context__(const t_pointer<t_input_context>& a_input_context)
{
	if (a_input_context == v_input_context) return;
	if (this == f_application()->v_focus) {
		if (v_input_context) v_input_context->f_blur();
		v_input_context = a_input_context;
		if (v_input_context) v_input_context->f_focus();
	} else {
		v_input_context = a_input_context;
	}
}

void t_window::f_cursor__(t_cursor a_cursor)
{
	if (a_cursor == v_cursor) return;
	v_cursor = a_cursor;
	if (v_handle == None) return;
	t_application* application = f_application();
	Display* display = application->f_x11_display();
	if (this == application->v_pointer_grabber) XChangeActivePointerGrab(display, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, v_cursor, application->v_time);
	XDefineCursor(display, v_handle, v_cursor);
}

void t_widget::f_create()
{
	f_x11_create(v_parent->v_handle, 0, CopyFromParent, CopyFromParent, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask | ExposureMask, true, None);
}

void t_shell::f_create()
{
	Display* display = f_application()->f_x11_display();
	f_x11_create(DefaultRootWindow(display), 0, CopyFromParent, CopyFromParent, v_mask, true, None);
	if (v_gravity != ForgetGravity) {
		XSizeHints hints;
		hints.flags = PWinGravity;
		hints.win_gravity = v_gravity;
		XSetWMNormalHints(display, v_handle, &hints);
	}
}

void t_shell::f_destroy()
{
	t_application* application = f_application();
	if (this == application->v_focus) application->f_focus__(0);
	if (this == application->v_active) application->f_active__(0);
	t_window::f_destroy();
}

void t_shell::f_process(XEvent& a_event)
{
	t_application* application = f_application();
	switch (a_event.type) {
	case KeyPress:
	case KeyRelease:
		if (application->v_focus) {
			if (application->v_focus->v_input_context) {
				application->v_focus->v_input_context->f_process(a_event.xkey);
			} else {
				char cs[16];
				KeySym symbol;
				int n = XLookupString(&a_event.xkey, cs, sizeof(cs), &symbol, 0);
				if (symbol >= XK_a && symbol <= XK_z) symbol -= XK_a - XK_A;
				if (a_event.type == KeyPress)
					application->v_focus->f_on_key_press(static_cast<t_modifier>(a_event.xkey.state), static_cast<t_key>(symbol), n > 0 ? cs[0] : '\0');
				else
					application->v_focus->f_on_key_release(static_cast<t_modifier>(a_event.xkey.state), static_cast<t_key>(symbol), n > 0 ? cs[0] : '\0');
			}
		}
		break;
	case FocusIn:
		switch (a_event.xfocus.detail) {
		case NotifyAncestor:
		case NotifyInferior:
		case NotifyNonlinear:
			if (this != application->v_active) {
				application->f_active__(this);
				application->f_focus__(this);
			}
			break;
		}
		break;
	case FocusOut:
		switch (a_event.xfocus.detail) {
		case NotifyAncestor:
		case NotifyInferior:
		case NotifyNonlinear:
			application->f_focus__(0);
			application->f_active__(0);
			break;
		}
		break;
	default:
		t_window::f_process(a_event);
	}
}

void t_shell::f_on_activate()
{
}

void t_shell::f_on_deactivate()
{
}

void t_frame::f_create()
{
	switch (v_gravity) {
	case NorthGravity:
	case CenterGravity:
	case SouthGravity:
	case StaticGravity:
		v_geometry.v_x -= 1;
		break;
	case NorthEastGravity:
	case EastGravity:
	case SouthEastGravity:
		v_geometry.v_x -= 2;
		break;
	}
	switch (v_gravity) {
	case WestGravity:
	case CenterGravity:
	case EastGravity:
	case StaticGravity:
		v_geometry.v_y -= 1;
		break;
	case SouthWestGravity:
	case SouthGravity:
	case SouthEastGravity:
		v_geometry.v_y -= 2;
		break;
	}
	t_application* application = f_application();
	Display* display = application->f_x11_display();
	f_x11_create(DefaultRootWindow(display), 1, CopyFromParent, CopyFromParent, v_mask, false, None);
	XSetWMProtocols(display, v_handle, &application->WM_DELETE_WINDOW, 1);
	std::vector<wchar_t> cs(v_caption.begin(), v_caption.end());
	cs.push_back(L'\0');
	wchar_t* list[] = {&cs[0]};
	XTextProperty name;
	XwcTextListToTextProperty(display, list, 1, XTextStyle, &name);
	XSizeHints hints;
	if (v_gravity == ForgetGravity) {
		hints.flags = 0;
	} else {
		hints.flags = PWinGravity;
		hints.win_gravity = v_gravity;
	}
	XWMHints wmhints;
	wmhints.flags = InputHint;
	wmhints.input = True;
	XSetWMProperties(display, v_handle, &name, &name, NULL, 0, &hints, &wmhints, NULL);
	XFree(name.value);
}

void t_frame::f_process(XEvent& a_event)
{
	switch (a_event.type) {
	case ConfigureNotify:
		v_geometry.v_x = a_event.xconfigure.x + 1;
		v_geometry.v_y = a_event.xconfigure.y + 1;
		v_geometry.v_width = a_event.xconfigure.width;
		v_geometry.v_height = a_event.xconfigure.height;
		f_on_move();
		break;
	case ClientMessage:
		if (a_event.xclient.message_type == f_application()->WM_PROTOCOLS && static_cast<Atom>(a_event.xclient.data.l[0]) == f_application()->WM_DELETE_WINDOW) f_on_close();
		break;
	default:
		t_shell::f_process(a_event);
	}
}

void t_frame::f_on_close()
{
}

void t_frame::f_caption__(const std::wstring& a_caption)
{
	v_caption = a_caption;
	if (v_handle != None) {
		Display* display = f_application()->f_x11_display();
		std::vector<wchar_t> cs(v_caption.begin(), v_caption.end());
		cs.push_back(L'\0');
		wchar_t* list[] = {&cs[0]};
		XTextProperty name;
		XwcTextListToTextProperty(display, list, 1, XTextStyle, &name);
		XSetWMProperties(display, v_handle, &name, &name, NULL, 0, NULL, NULL, NULL);
		XFree(name.value);
	}
}

}
