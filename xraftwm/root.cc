#include "common.h"

#include <cstdio>
#include <cstdlib>
#include <typeinfo>

namespace xraft
{

t_root* t_root::v_instance;

int t_root::f_handle(Display* a_display, XErrorEvent* a_xerror)
{
	char buffer[255];
	XGetErrorText(a_display, a_xerror->error_code, buffer, sizeof(buffer));
	std::fprintf(stderr, "%s\n", buffer);
	if (a_xerror->error_code == BadAccess && a_xerror->resourceid == DefaultRootWindow(a_display)) std::exit(1);
	t_application* application;
	try {
		application = f_application();
	} catch (std::runtime_error&) {
		return 0;
	}
	t_root* root = static_cast<t_root*>(application->f_find(DefaultRootWindow(a_display)));
	if (root) {
		t_pointer<t_client> client = root->f_find(a_xerror->resourceid);
		if (client) {
			client->f_revoke();
			return 0;
		}
	}
	t_sink* p = application->f_find(a_xerror->resourceid);
	if (p) {
		std::fprintf(stderr, "exit: resource id=%d, type=%s\n", a_xerror->resourceid, typeid(*p).name());
		std::exit(1);
	}
	return 0;
}

void t_root::operator()()
{
	if (!v_revokeds) return;
	t_client* revokeds = v_revokeds->v_next_revoked;
	v_revokeds = 0;
	t_client* client = revokeds;
	do {
		t_client* next = client->v_next_revoked;
		size_t i = 0;
		while (f_at(i) != client) ++i;
		f_remove(i);
		client = next;
	} while (client != revokeds);
}

void t_root::f_create()
{
}

void t_root::f_process(XEvent& a_event)
{
	switch (a_event.type) {
	case ConfigureRequest:
		{
			t_pointer<t_client> client = f_find(a_event.xconfigurerequest.window);
			if (client) {
				client->f_process(a_event);
			} else {
				XWindowChanges values;
				values.x = a_event.xconfigurerequest.x;
				values.y = a_event.xconfigurerequest.y;
				values.width = a_event.xconfigurerequest.width;
				values.height = a_event.xconfigurerequest.height;
				values.border_width = a_event.xconfigurerequest.border_width;
				values.sibling = a_event.xconfigurerequest.above;
				values.stack_mode = a_event.xconfigurerequest.detail;
				XConfigureWindow(f_application()->f_x11_display(), a_event.xconfigurerequest.window, a_event.xconfigurerequest.value_mask, &values);
			}
		}
		break;
	case MapRequest:
		{
			t_pointer<t_client> client = f_find(a_event.xmaprequest.window);
			if (client) {
				client->f_process(a_event);
			} else {
				client = f_on_client();
				XWindowAttributes attributes;
				XGetWindowAttributes(f_application()->f_x11_display(), a_event.xmaprequest.window, &attributes);
				client->f_attach(a_event.xmaprequest.window, attributes);
			}
		}
		break;
	default:
		t_window::f_process(a_event);
	}
}

t_root::t_root()
{
	if (v_instance) throw std::runtime_error("root instance already exists.");
	XSetErrorHandler(f_handle);
	t_application* application = f_application();
	Display* display = application->f_x11_display();
	int shape_error;
	v_shape = XShapeQueryExtension(display, &v_shape_event, &shape_error);
	XC_CLIENT = XUniqueContext();
	char wm_sn[32];
	std::sprintf(wm_sn, "WM_S%d", DefaultScreen(display));
	char* names[] = {
		const_cast<char*>("WM_STATE"),
		const_cast<char*>("WM_CHANGE_STATE"),
		const_cast<char*>("WM_TAKE_FOCUS"),
		const_cast<char*>("WM_COLORMAP_WINDOWS"),
		wm_sn
	};
	Atom atoms[sizeof(names) / sizeof(char*)];
	XInternAtoms(display, names, sizeof(atoms) / sizeof(Atom), False, atoms);
	WM_STATE = atoms[0];
	WM_CHANGE_STATE = atoms[1];
	WM_TAKE_FOCUS = atoms[2];
	WM_COLORMAP_WINDOWS = atoms[3];
	WM_Sn = atoms[4];
	v_handle = DefaultRootWindow(display);
	application->f_register(v_handle, this);
	f_x11_geometry__(t_rectangle(t_point(0, 0), application->f_screen()));
	v_instance = this;
}

t_root::~t_root()
{
	v_instance = 0;
	f_application()->f_revoke(v_handle);
}

void t_root::f_run()
{
	t_application* application = f_application();
	Display* display = application->f_x11_display();
	XSelectInput(display, v_handle, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask | ExposureMask | SubstructureRedirectMask);
	XSetSelectionOwner(display, WM_Sn, v_handle, CurrentTime);
	Window root;
	Window parent;
	Window* children;
	unsigned int nchildren;
	XQueryTree(display, v_handle, &root, &parent, &children, &nchildren);
	for (unsigned int i = 0; i < nchildren; ++i) {
		XWindowAttributes attributes;
		XGetWindowAttributes(display, children[i], &attributes);
		if (attributes.override_redirect == True) continue;
		if (attributes.map_state == IsViewable) {
			t_pointer<t_client> client = f_on_client();
			client->f_attach(children[i], attributes);
		}
	}
	XFree(children);
	application->f_run();
	XInstallColormap(display, DefaultColormap(display, DefaultScreen(display)));
	XSetInputFocus(display, PointerRoot, RevertToPointerRoot, CurrentTime);
	XSetSelectionOwner(display, WM_Sn, None, CurrentTime);
	size_t i = 0;
	while (i < f_count()) {
		t_pointer<t_client> client = f_dynamic_cast<t_client>(f_at(i));
		if (client)
			f_remove(i);
		else
			++i;
	}
}

void t_root::f_active__(const t_pointer<t_client>& a_client)
{
	if (a_client == v_active) return;
	t_application* application = f_application();
	Display* display = application->f_x11_display();
	Time time = application->f_x11_time();
	t_client* active = v_active;
	v_active = a_client;
	if (active) {
		if (!active->v_shaded) {
			XInstallColormap(display, DefaultColormap(display, DefaultScreen(display)));
//			XSetInputFocus(display, PointerRoot, RevertToNone, time);
		}
		active->f_on_deactivate();
	}
	if (v_active) {
		if (v_active->v_shaded)
			XSetInputFocus(display, PointerRoot, RevertToNone, time);
		else
			v_active->f_focus();
		v_active->f_on_activate();
	} else {
		XInstallColormap(display, DefaultColormap(display, DefaultScreen(display)));
		XSetInputFocus(display, PointerRoot, RevertToNone, time);
	}
}

}
