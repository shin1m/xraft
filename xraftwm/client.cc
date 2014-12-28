#include "common.h"

#include <cstdio>

namespace xraft
{

unsigned t_client::t_size_hints::f_adjust(int a_x) const
{
	if (a_x < v_lower) a_x = v_lower;
	if (a_x > v_upper) a_x = v_upper;
	return ((a_x + v_delta / 2 - v_base) / v_delta) * v_delta + v_base;
}

t_point t_client::f_gravity() const
{
	int dx = 0;
	switch (v_gravity) {
	case NorthGravity:
	case CenterGravity:
	case SouthGravity:
		dx = (v_borders[0] + v_borders[2]) / 2 - v_border_width;
		break;
	case NorthEastGravity:
	case EastGravity:
	case SouthEastGravity:
		dx = v_borders[0] + v_borders[2] - v_border_width * 2;
		break;
	case StaticGravity:
		dx = v_borders[0] - v_border_width;
		break;
	}
	int dy = 0;
	switch (v_gravity) {
	case WestGravity:
	case CenterGravity:
	case EastGravity:
		dy = (v_borders[1] + v_borders[3]) / 2 - v_border_width;
		break;
	case SouthWestGravity:
	case SouthGravity:
	case SouthEastGravity:
		dy = v_borders[1] + v_borders[3] - v_border_width * 2;
		break;
	case StaticGravity:
		dy = v_borders[1] - v_border_width;
		break;
	}
	return t_point(dx, dy);
}

void t_client::f_fetch_wm_hints(CARD32* a_initial_state)
{
	XWMHints* hints = XGetWMHints(f_application()->f_x11_display(), v_client);
	if (hints == NULL) return;
	if ((hints->flags & InputHint) != 0) v_input = hints->input;
	if (a_initial_state && (hints->flags & StateHint) != 0) *a_initial_state = hints->initial_state;
	XFree(hints);
}

void t_client::f_fetch_wm_name()
{
	Display* display = f_application()->f_x11_display();
	XTextProperty name;
	if (XGetWMName(display, v_client, &name) == 0) {
		v_name.clear();
	} else {
		wchar_t** list;
		int count;
		XwcTextPropertyToTextList(display, &name, &list, &count);
		XFree(name.value);
		std::vector<wchar_t> cs;
		for (int i = 0; i < count; ++i) cs.insert(cs.end(), list[i], list[i] + std::wcslen(list[i]));
		v_name.assign(cs.begin(), cs.end());
		XwcFreeStringList(list);
	}
}

void t_client::f_fetch_wm_normal_hints()
{
	XSizeHints* hints = XAllocSizeHints();
	long supplied;
	if (XGetWMNormalHints(f_application()->f_x11_display(), v_client, hints, &supplied) == 0) hints->flags = 0;
	if ((hints->flags & PMinSize) == 0) {
		v_width_hints.v_lower = v_height_hints.v_lower = 0;
	} else {
		v_width_hints.v_lower = hints->min_width;
		v_height_hints.v_lower = hints->min_height;
	}
	if ((hints->flags & PMaxSize) == 0) {
		v_width_hints.v_upper = v_height_hints.v_upper = 0xffff;
	} else {
		v_width_hints.v_upper = hints->max_width;
		v_height_hints.v_upper = hints->max_height;
	}
	if ((hints->flags & PResizeInc) == 0) {
		v_width_hints.v_delta = v_height_hints.v_delta = 1;
	} else {
		v_width_hints.v_delta = hints->width_inc;
		v_height_hints.v_delta = hints->height_inc;
	}
	if ((hints->flags & PBaseSize) == 0) {
		v_width_hints.v_base = v_width_hints.v_lower;
		v_height_hints.v_base = v_height_hints.v_lower;
	} else {
		v_width_hints.v_base = hints->base_width;
		v_height_hints.v_base = hints->base_height;
	}
	if (v_width_hints.v_lower < 1) v_width_hints.v_lower = 1;
	if (v_width_hints.v_lower < static_cast<int>(v_width_hints.v_base)) v_width_hints.v_lower = v_width_hints.v_base;
	if (v_height_hints.v_lower < 1) v_height_hints.v_lower = 1;
	if (v_height_hints.v_lower < static_cast<int>(v_height_hints.v_base)) v_height_hints.v_lower = v_height_hints.v_base;
	if (v_width_hints.v_upper < v_width_hints.v_lower) v_width_hints.v_upper = v_width_hints.v_lower;
	if (v_width_hints.v_upper > 0xffff) v_width_hints.v_upper = 0xffff;
	if (v_height_hints.v_upper < v_height_hints.v_lower) v_height_hints.v_upper = v_height_hints.v_lower;
	if (v_height_hints.v_upper > 0xffff) v_height_hints.v_upper = 0xffff;
	v_gravity = (hints->flags & PWinGravity) == 0 ? NorthWestGravity : hints->win_gravity;
	XFree(hints);
}

void t_client::f_fetch_wm_protocols()
{
	t_application* application = f_application();
	Atom* protocols;
	int count;
	if (XGetWMProtocols(application->f_x11_display(), v_client, &protocols, &count) != 0) {
		for (int i = 0; i < count; ++i) {
			if (protocols[i] == f_root()->WM_TAKE_FOCUS)
				v_take_focus = true;
			else if (protocols[i] == application->WM_DELETE_WINDOW)
				v_delete_window = true;
		}
		XFree(protocols);
	}
}

CARD32 t_client::f_wm_state() const
{
	CARD32 wm_state = WithdrawnState;
	Atom actual_type;
	int actual_format;
	unsigned long nitems;
	unsigned long bytes_after;
	CARD32* cards;
	if (XGetWindowProperty(f_application()->f_x11_display(), v_client, f_root()->WM_STATE, 0l, 2l, False, f_root()->WM_STATE, &actual_type, &actual_format, &nitems, &bytes_after, reinterpret_cast<unsigned char**>(&cards)) == Success) {
		if (nitems > 0) wm_state = cards[0];
		XFree(cards);
	}
	return wm_state;
}

void t_client::f_wm_state__(CARD32 a_wm_state)
{
	Display* display = f_application()->f_x11_display();
	if (a_wm_state == WithdrawnState) {
		XDeleteProperty(display, v_client, f_root()->WM_STATE);
	} else {
		CARD32 cards[] = {a_wm_state, None};
		XChangeProperty(display, v_client, f_root()->WM_STATE, f_root()->WM_STATE, 32, PropModeReplace, reinterpret_cast<unsigned char*>(cards), 2);
	}
}

Status t_client::f_send_wm_protocols(Atom a_atom, Time a_time)
{
	t_application* application = f_application();
	XEvent event;
	event.xclient.type = ClientMessage;
	event.xclient.window = v_client;
	event.xclient.message_type = application->WM_PROTOCOLS;
	event.xclient.format = 32;
	event.xclient.data.l[0] = a_atom;
	event.xclient.data.l[1] = a_time;
	return XSendEvent(application->f_x11_display(), v_client, False, NoEventMask, &event);
}

Status t_client::f_send_configure()
{
	const t_rectangle& geometry = f_geometry();
	XEvent event;
	event.xconfigure.type = ConfigureNotify;
	event.xconfigure.event = v_client;
	event.xconfigure.window = v_client;
	event.xconfigure.x = geometry.v_x + v_borders[0] - v_border_width;
	event.xconfigure.y = geometry.v_y + v_borders[1] - v_border_width;
	event.xconfigure.width = v_width;
	event.xconfigure.height = v_height;
	event.xconfigure.border_width = v_border_width;
	event.xconfigure.above = None;
	event.xconfigure.override_redirect = False;
	return XSendEvent(f_application()->f_x11_display(), v_client, False, StructureNotifyMask, &event);
}

void t_client::f_focus()
{
	t_application* application = f_application();
	Display* display = application->f_x11_display();
	XInstallColormap(display, v_colormap);
	Time time = application->f_x11_time();
	if (v_input == True) {
		XSetInputFocus(display, v_client, RevertToPointerRoot, time);
		if (v_take_focus) f_send_wm_protocols(f_root()->WM_TAKE_FOCUS, time);
	} else {
		XSetInputFocus(display, PointerRoot, RevertToNone, time);
		if (v_take_focus) f_send_wm_protocols(f_root()->WM_TAKE_FOCUS, time);
	}
}

void t_client::f_configure(const XConfigureRequestEvent& a_xconfigurerequest)
{
	unsigned width = v_width;
	unsigned height = v_height;
	t_rectangle geometry = f_geometry();
	t_point gravity = f_gravity();
	geometry.v_x += gravity.v_x;
	geometry.v_y += gravity.v_y;
	if ((a_xconfigurerequest.value_mask & CWX) != 0)
		geometry.v_x = a_xconfigurerequest.x;
	if ((a_xconfigurerequest.value_mask & CWY) != 0)
		geometry.v_y = a_xconfigurerequest.y;
	if ((a_xconfigurerequest.value_mask & CWWidth) != 0)
		v_width = a_xconfigurerequest.width;
	if ((a_xconfigurerequest.value_mask & CWHeight) != 0)
		v_height = a_xconfigurerequest.height;
	if ((a_xconfigurerequest.value_mask & CWBorderWidth) != 0)
		v_border_width = a_xconfigurerequest.border_width;
	gravity = f_gravity();
	geometry.v_x -= gravity.v_x;
	geometry.v_y -= gravity.v_y;
	geometry.v_width = v_width + v_borders[0] + v_borders[2];
	if (v_shaded)
		geometry.v_height = v_borders[1] + v_borders[3];
	else
		geometry.v_height = v_height + v_borders[1] + v_borders[3];
	f_x11_geometry__(geometry);
	Display* display = f_application()->f_x11_display();
	unsigned int mask = a_xconfigurerequest.value_mask & ~CWBorderWidth | CWX | CWY | CWWidth | CWHeight;
	XWindowChanges values;
	values.x = geometry.v_x;
	values.y = geometry.v_y;
	values.width = geometry.v_width;
	values.height = geometry.v_height;
	values.sibling = a_xconfigurerequest.above;
	values.stack_mode = a_xconfigurerequest.detail;
        XConfigureWindow(display, v_handle, mask, &values);
	if ((a_xconfigurerequest.value_mask & CWStackMode) != 0) {
		std::vector<t_pointer<t_widget> >& children = f_root()->f_x11_children();
		switch (a_xconfigurerequest.detail) {
		case Above:
		case Below:
			{
				t_pointer<t_widget> p = this;
				children.erase(std::find(children.begin(), children.end(), p));
				auto i = (a_xconfigurerequest.value_mask & CWSibling) != 0 ? std::find(children.begin(), children.end(), f_root()->f_find(a_xconfigurerequest.above)) : children.end();
				if (i != children.end()) {
					if (a_xconfigurerequest.detail == Above) ++i;
					children.insert(i, p);
				} else if (a_xconfigurerequest.detail == Above) {
					children.push_back(p);
				} else {
					children.insert(children.begin(), p);
				}
			}
			break;
		}
	}
	f_on_move();
	if (v_width != width || v_height != height)
		XResizeWindow(display, v_client, v_width, v_height);
	else
		f_send_configure();
	if (f_root()->v_shape) f_shape();
}

void t_client::f_shape()
{
	Display* display = f_application()->f_x11_display();
	XShapeCombineMask(display, v_handle, ShapeBounding, 0, 0, None, ShapeSet);
	if (!v_shaped || v_shaded) return;
	XRectangle rectangle;
	rectangle.x = v_borders[0];
	rectangle.y = v_borders[1];
	rectangle.width = v_width;
	rectangle.height = v_height;
	XShapeCombineRectangles(display, v_handle, ShapeBounding, 0, 0, &rectangle, 1, ShapeSubtract, YXBanded);
	XShapeCombineShape(display, v_handle, ShapeBounding, v_borders[0], v_borders[1], v_client, ShapeBounding, ShapeUnion);
}

void t_client::f_create()
{
	Display* display = f_application()->f_x11_display();
	f_x11_create(DefaultRootWindow(display), 0, CopyFromParent, CopyFromParent, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | ExposureMask | EnterWindowMask | LeaveWindowMask | SubstructureNotifyMask | SubstructureRedirectMask, true, None);
	f_root()->f_register(v_handle, this);
	XSetWindowBorderWidth(display, v_client, 0);
	XAddToSaveSet(display, v_client);
	XReparentWindow(display, v_client, v_handle, v_borders[0], v_borders[1]);
	XResizeWindow(display, v_client, v_width, v_height);
	if (f_root()->v_shape) {
		XShapeSelectInput(display, v_client, ShapeNotifyMask);
		Bool bounding;
		Bool clip;
		int xy;
		unsigned int wh;
		XShapeQueryExtents(display, v_client, &bounding, &xy, &xy, &wh, &wh, &clip, &xy, &xy, &wh, &wh);
		v_shaped = bounding == True;
		f_shape();
	}
	f_send_configure();
}

void t_client::f_destroy()
{
	Display* display = f_application()->f_x11_display();
	if (this == f_root()->f_active()) f_root()->f_active__(0);
	f_wm_state__(WithdrawnState);
	const t_rectangle& geometry = f_geometry();
	t_point gravity = f_gravity();
	XReparentWindow(display, v_client, DefaultRootWindow(display), geometry.v_x + gravity.v_x, geometry.v_y + gravity.v_y);
	XRemoveFromSaveSet(display, v_client);
	XSetWindowBorderWidth(display, v_client, v_border_width);
	t_widget::f_destroy();
}

void t_client::f_process(XEvent& a_event)
{
	switch (a_event.type) {
	case DestroyNotify:
		f_revoke();
		break;
	case UnmapNotify:
std::printf("UnmapNotify: this=%p, a_event.xunmap.serial=%lu, v_serial=%lu\n", this, a_event.xunmap.serial, v_serial);
		if (a_event.xunmap.serial == v_serial)
			v_serial = 0;
		else
			f_revoke();
		break;
	case MapRequest:
		f_show();
		break;
	case ConfigureRequest:
		f_configure(a_event.xconfigurerequest);
		break;
	case PropertyNotify:
		switch (a_event.xproperty.atom) {
		case XA_WM_HINTS:
			f_fetch_wm_hints(0);
			break;
		case XA_WM_NAME:
			f_fetch_wm_name();
			f_on_name();
			break;
		case XA_WM_NORMAL_HINTS:
			f_fetch_wm_normal_hints();
			break;
		default:
			if (a_event.xproperty.atom == f_application()->WM_PROTOCOLS) {
				f_fetch_wm_protocols();
				f_on_protocols();
			}
		}
		break;
	case ColormapNotify:
		if (a_event.xcolormap.c_new == True) {
			v_colormap = a_event.xcolormap.colormap;
			if (this == f_root()->f_active()) XInstallColormap(f_application()->f_x11_display(), v_colormap);
		}
		break;
	case ClientMessage:
		if (a_event.xclient.message_type == f_root()->WM_CHANGE_STATE && a_event.xclient.format == 32 && a_event.xclient.data.l[0] == IconicState) f_hide();
		break;
	default:
		if (f_root()->v_shape && a_event.type == f_root()->v_shape_event) {
			XShapeEvent& xshape = reinterpret_cast<XShapeEvent&>(a_event);
			if (xshape.kind == ShapeBounding) {
				v_shaped = xshape.shaped == True;
				f_shape();
			}
		} else {
			t_widget::f_process(a_event);
		}
	}
}

void t_client::f_on_activate()
{
}

void t_client::f_on_deactivate()
{
}

void t_client::f_on_name()
{
}

void t_client::f_on_protocols()
{
}

void t_client::f_attach(Window a_window, const XWindowAttributes& a_attributes)
{
	t_application* application = f_application();
	Display* display = application->f_x11_display();
	v_client = a_window;
	v_width = a_attributes.width;
	v_height = a_attributes.height;
	v_border_width = a_attributes.border_width;
	v_colormap = a_attributes.colormap;
	application->f_register(v_client, this);
	f_root()->f_register(v_client, this);
	XSelectInput(display, v_client, ColormapChangeMask | PropertyChangeMask);
	CARD32 wm_state = NormalState;
	f_fetch_wm_hints(&wm_state);
	f_fetch_wm_name();
	f_fetch_wm_normal_hints();
	f_fetch_wm_protocols();
	XGetTransientForHint(display, v_client, &v_transient_for);
	t_point gravity = f_gravity();
	t_widget::f_move(t_rectangle(a_attributes.x - gravity.v_x, a_attributes.y - gravity.v_y, v_width + v_borders[0] + v_borders[2], v_shaded ? v_borders[1] + v_borders[3] : v_height + v_borders[1] + v_borders[3]));
	f_root()->f_add(this);
	switch (wm_state) {
	case NormalState:
		f_show();
		break;
	case IconicState:
		f_hide();
		break;
	default:
		f_show();
	}
}

void t_client::f_revoke()
{
	if (v_next_revoked) return;
	t_application* application = f_application();
	t_root* root = f_root();
	root->f_revoke(v_handle);
	root->f_revoke(v_client);
	application->f_revoke(v_client);
	if (root->v_revokeds) {
		v_next_revoked = root->v_revokeds->v_next_revoked;
		root->v_revokeds->v_next_revoked = this;
	} else {
		application->f_post(root);
		v_next_revoked = this;
	}
	root->v_revokeds = this;
}

void t_client::f_borders__(const unsigned* a_borders)
{
	t_rectangle geometry = f_geometry();
	t_point gravity = f_gravity();
	geometry.v_x += gravity.v_x;
	geometry.v_y += gravity.v_y;
	geometry.v_width -= v_borders[0] + v_borders[2];
	geometry.v_height -= v_borders[1] + v_borders[3];
	std::copy(a_borders, a_borders + 4, v_borders);
	gravity = f_gravity();
	geometry.v_x -= gravity.v_x;
	geometry.v_y -= gravity.v_y;
	geometry.v_width += v_borders[0] + v_borders[2];
	geometry.v_height += v_borders[1] + v_borders[3];
	t_widget::f_move(geometry);
	if (v_handle == None) return;
	f_send_configure();
	if (f_root()->v_shape) f_shape();
}

void t_client::f_move(t_side a_horizontal, int a_x, t_side a_vertical, int a_y)
{
	const t_rectangle& geometry = f_geometry();
	int left = geometry.v_x;
	int right = geometry.v_x + geometry.v_width;
	int top = geometry.v_y;
	int bottom = geometry.v_y + geometry.v_height;
	switch (a_horizontal) {
	case e_side__NEAR:
		left = a_x;
		break;
	case e_side__FAR:
		right = a_x;
		break;
	case e_side__BOTH:
		left = a_x;
		right = a_x + geometry.v_width;
		break;
	}
	switch (a_vertical) {
	case e_side__NEAR:
		top = a_y;
		if (v_shaded) bottom = a_y + geometry.v_height;
		break;
	case e_side__FAR:
		bottom = a_y;
		if (v_shaded) top = a_y - geometry.v_height;
		break;
	case e_side__BOTH:
		top = a_y;
		bottom = a_y + geometry.v_height;
		break;
	}
	unsigned width = v_width;
	unsigned height = v_height;
	v_width = v_width_hints.f_adjust(right - left - v_borders[0] - v_borders[2]);
	if (!v_shaded) v_height = v_height_hints.f_adjust(bottom - top - v_borders[1] - v_borders[3]);
	if (a_horizontal == e_side__NEAR) left = right - v_width - v_borders[0] - v_borders[2];
	if (!v_shaded && a_vertical == e_side__NEAR) top = bottom - v_height - v_borders[1] - v_borders[3];
	t_widget::f_move(t_rectangle(left, top, v_width + v_borders[0] + v_borders[2], v_shaded ? v_borders[1] + v_borders[3] : v_height + v_borders[1] + v_borders[3]));
	if (v_width != width || v_height != height)
		XResizeWindow(f_application()->f_x11_display(), v_client, v_width, v_height);
	else
		f_send_configure();
	if (f_root()->v_shape) f_shape();
}

void t_client::f_show()
{
	f_wm_state__(NormalState);
	XMapWindow(f_application()->f_x11_display(), v_client);
	t_widget::f_show();
}

void t_client::f_hide()
{
	f_wm_state__(IconicState);
	if (v_serial == 0) {
		Display* display = f_application()->f_x11_display();
		v_serial = NextRequest(display);
std::printf("XUnmapWindow: %lu\n", v_serial);
		XUnmapWindow(display, v_client);
	}
	t_widget::f_hide();
}

void t_client::f_shaded__(bool a_shaded)
{
	if (a_shaded == v_shaded) return;
	t_application* application = f_application();
	Display* display = application->f_x11_display();
	v_shaded = a_shaded;
	t_rectangle geometry = f_geometry();
	if (v_shaded) {
		geometry.v_height = v_borders[1] + v_borders[3];
		XMoveWindow(display, v_client, v_borders[0], -v_height);
		XResizeWindow(display, v_handle, geometry.v_width, geometry.v_height);
		if (this == f_root()->f_active()) {
			XInstallColormap(display, DefaultColormap(display, DefaultScreen(display)));
			XSetInputFocus(display, PointerRoot, RevertToNone, application->f_x11_time());
		}
	} else {
		geometry.v_height = v_height + v_borders[1] + v_borders[3];
		XResizeWindow(display, v_handle, geometry.v_width, geometry.v_height);
		XMoveWindow(display, v_client, v_borders[0], v_borders[1]);
		if (this == f_root()->f_active()) f_focus();
	}
	f_x11_geometry__(geometry);
	f_on_move();
	if (f_root()->v_shape) f_shape();
}

}
