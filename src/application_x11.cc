#include <xraft/application.h>

#include <signal.h>
#include <xraft/ibus.h>
#include <xraft/graphics.h>
#include <X11/XKBlib.h>

namespace xraft
{

thread_local t_application* t_application::v_instance;

void t_application::f_terminate(int a_signal)
{
	if (v_instance) v_instance->v_loop = 0;
}

void t_application::f_allocate(unsigned char a_i)
{
	XColor color;
	color.red = color.green = color.blue = 0x0101 * a_i;
	XAllocColor(v_display, DefaultColormap(v_display, DefaultScreen(v_display)), &color);
	v_allocateds[a_i] = true;
	v_reds[a_i] = color.pixel & v_red_mask;
	v_greens[a_i] = color.pixel & v_green_mask;
	v_blues[a_i] = color.pixel & v_blue_mask;
}

void t_application::f_active__(const t_pointer<t_shell>& a_shell)
{
	if (a_shell == v_active) return;
	t_pointer<t_shell> active = v_active;
	v_active = a_shell;
	if (active) active->f_on_deactivate();
	if (v_active) v_active->f_on_activate();
}

dbus::t_connection& t_application::f_input_bus()
{
	if (!v_input_bus) {
		try {
			v_input_bus.f_open(ibus::f_address().c_str());
		} catch (std::exception& e) {
			std::fprintf(stderr, "%s\n", e.what());
		}
		if (v_input_bus) new dbus::t_bridge(v_input_bus);
	}
	return v_input_bus;
}

void t_application::f_run_post()
{
	t_runnable* runnable;
	size_t m = 0;
	do m += read(v_pollfds[1].fd, reinterpret_cast<char*>(&runnable) + m, sizeof(t_runnable*) - m); while (m < sizeof(t_runnable*));
	(*runnable)();
}

void t_application::f_loop()
{
	size_t current = ++v_loop;
	while (true) {
		t_input_context::f_negotiate();
		if (v_loop < current) return;
		t_window::f_validate();
		XSync(v_display, False);
		int n = XPending(v_display);
		if (n <= 0) {
			f_wait();
			if (v_loop < current) return;
			n = XPending(v_display);
		}
		while (true) {
			if (n > 0) {
				XEvent event;
				XNextEvent(v_display, &event);
				f_dispatch(event);
				if (v_loop < current) return;
			}
			if (v_pollfds[1].revents == POLLIN) {
				f_run_post();
				++n;
				if (v_loop < current) return;
			}
			while (v_notifier < v_listeners.size()) {
				int fd = v_pollfds[v_notifier + 2].fd;
				short revents = v_pollfds[v_notifier + 2].revents;
				if ((revents & POLLIN) != 0) {
					t_runnable* reader = v_listeners[v_notifier].v_reader;
					if (reader) {
						(*reader)();
						++n;
						if (v_loop < current) break;
						if (v_notifier >= v_listeners.size()) break;
					}
				}
				if (v_pollfds[v_notifier + 2].fd != fd) continue;
				if ((revents & POLLOUT) != 0) {
					t_runnable* writer = v_listeners[v_notifier].v_writer;
					if (writer) {
						(*writer)();
						++n;
						if (v_loop < current) break;
					}
				}
				++v_notifier;
			}
			v_notifier = 0;
			if (v_loop < current) return;
			if (v_timer) {
				timeval tv;
				gettimeofday(&tv, NULL);
				if (timercmp(&v_timer->v_time, &tv, <=)) {
					t_timer* p = v_timer;
					v_timer = p->v_next;
					if (p->v_single) {
						p->v_queued = false;
						p->v_next = 0;
					} else {
						p->f_add();
					}
					(*p)();
					if (v_loop < current) return;
					++n;
				}
			}
			if (n <= 0) break;
			poll(&v_pollfds[1], v_pollfds.size() - 1, 0);
			if (v_loop < current) return;
			n = XPending(v_display);
		}
	}
}

void t_application::f_wait()
{
	if (v_timer) {
		timeval tv;
		gettimeofday(&tv, NULL);
		timersub(&v_timer->v_time, &tv, &tv);
		int timeout = tv.tv_sec * 1000 + tv.tv_usec / 1000;
		poll(&v_pollfds[0], v_pollfds.size(), timeout < 0 ? 0 : timeout);
	} else {
		poll(&v_pollfds[0], v_pollfds.size(), -1);
	}
}

t_application::t_application(std::vector<std::string>& a_arguments) : v_pollfds(2)
{
	if (v_instance) throw std::runtime_error("application instance already exists.");
#if 0
	struct sigaction action;
	action.sa_flags = 0;
	action.sa_handler = f_terminate;
	sigaction(SIGTERM, &action, NULL);
	sigaction(SIGINT, &action, NULL);
	sigaction(SIGHUP, &action, NULL);
#endif
	XSetLocaleModifiers("");
	XC_SINK = XUniqueContext();
	v_display = XOpenDisplay(NULL);
	{
		int opcode;
		int event;
		int error;
		int major = XkbMajorVersion;
		int minor = XkbMinorVersion;
		if (XkbQueryExtension(v_display, &opcode, &event, &error, &major, &minor)) {
			Bool supported;
			XkbSetDetectableAutoRepeat(v_display, True, &supported);
		}
	}
	int screen = DefaultScreen(v_display);
	{
		auto end = a_arguments.end();
		auto i = a_arguments.begin();
		auto j = i;
		while (i != end) {
			if (*i == "-geometry") {
				if (++i == end) break;
				int value = XParseGeometry(i->c_str(), &v_x, &v_y, &v_width, &v_height);
				v_gravity = NorthWestGravity;
				if ((value & XNegative) != 0) {
					v_x += DisplayWidth(v_display, screen) - v_width;
					v_gravity += 2;
				}
				if ((value & YNegative) != 0) {
					v_y += DisplayHeight(v_display, screen) - v_height;
					v_gravity += 6;
				}
			} else {
				*j = *i;
				++j;
			}
			++i;
		}
		a_arguments.erase(j, end);
	}
	Window root = RootWindow(v_display, screen);
	v_gc = DefaultGC(v_display, screen);
	v_gc_save = XCreateGC(v_display, root, 0, NULL);
	v_pixmap = XCreatePixmap(v_display, root, 1, 1, 1);
	v_gc_mask = XCreateGC(v_display, v_pixmap, 0, NULL);
	XVisualInfo vinfo_template;
	vinfo_template.visualid = XVisualIDFromVisual(DefaultVisual(v_display, screen));
	int nitems;
	XVisualInfo* vinfos = XGetVisualInfo(v_display, VisualIDMask, &vinfo_template, &nitems);
	v_red_mask = vinfos[0].red_mask;
	v_green_mask = vinfos[0].green_mask;
	v_blue_mask = vinfos[0].blue_mask;
	std::fill_n(v_allocateds, 256, false);
	XFree(vinfos);
	v_om = XOpenOM(v_display, NULL, NULL, NULL);
	v_oc = XCreateOC(v_om, XNBaseFontName, "-*-fixed-medium-r-*-*-14-*-*-*-*-*-*-*", NULL);
	char* names[] = {const_cast<char*>("WM_PROTOCOLS"), const_cast<char*>("WM_DELETE_WINDOW")};
	Atom atoms[2];
	XInternAtoms(v_display, names, 2, False, atoms);
	WM_PROTOCOLS = atoms[0];
	WM_DELETE_WINDOW = atoms[1];
	v_pollfds[0].fd = ConnectionNumber(v_display);
	int fds[2];
	pipe(fds);
	v_pollfds[1].fd = fds[0];
	v_post = fds[1];
	v_pollfds[0].events = v_pollfds[1].events = POLLIN;
	v_instance = this;
	v_font = new t_font("-*-fixed-medium-r-*-*-14-*-*-*-*-*-*-*");
	v_input_first = new t_input_context::t_component();
	v_input_middle = new t_input_context::t_component();
	v_input_last = new t_input_context::t_component();
	v_input_context = new t_input_context();
}

t_application::~t_application()
{
	for (const auto& p : v_shells) p->f_destroy();
	v_shells.clear();
	v_input_context = 0;
	v_input_first = 0;
	v_input_middle = 0;
	v_input_last = 0;
	v_font = 0;
	v_instance = 0;
	if (v_cursor_x != None) XFreeCursor(v_display, v_cursor_x);
	if (v_cursor_arrow != None) XFreeCursor(v_display, v_cursor_arrow);
	if (v_cursor_ibeam != None) XFreeCursor(v_display, v_cursor_ibeam);
	if (v_cursor_vslide != None) XFreeCursor(v_display, v_cursor_vslide);
	if (v_cursor_hslide != None) XFreeCursor(v_display, v_cursor_hslide);
	if (v_cursor_top != None) XFreeCursor(v_display, v_cursor_top);
	if (v_cursor_bottom != None) XFreeCursor(v_display, v_cursor_bottom);
	if (v_cursor_left != None) XFreeCursor(v_display, v_cursor_left);
	if (v_cursor_right != None) XFreeCursor(v_display, v_cursor_right);
	if (v_cursor_top_left != None) XFreeCursor(v_display, v_cursor_top_left);
	if (v_cursor_top_right != None) XFreeCursor(v_display, v_cursor_top_right);
	if (v_cursor_bottom_left != None) XFreeCursor(v_display, v_cursor_bottom_left);
	if (v_cursor_bottom_right != None) XFreeCursor(v_display, v_cursor_bottom_right);
	close(v_pollfds[1].fd);
	close(v_post);
	XDestroyOC(v_oc);
	XCloseOM(v_om);
	XFreeGC(v_display, v_gc_mask);
	XFreePixmap(v_display, v_pixmap);
	XFreeGC(v_display, v_gc_save);
	XCloseDisplay(v_display);
}

void t_application::f_dispatch(XEvent& a_event)
{
	switch (a_event.type) {
	case KeyPress:
	case KeyRelease:
		v_time = a_event.xkey.time;
		break;
	case ButtonPress:
	case ButtonRelease:
		v_time = a_event.xbutton.time;
		break;
	case MotionNotify:
		v_time = a_event.xmotion.time;
		break;
	case EnterNotify:
	case LeaveNotify:
		v_time = a_event.xcrossing.time;
		break;
	case PropertyNotify:
		v_time = a_event.xproperty.time;
		break;
	default:
		v_time = CurrentTime;
	}
	t_sink* p = f_find(a_event.xany.window);
	if (p) p->f_process(a_event);
}

void t_application::f_run()
{
	f_loop();
	if (v_loop > 0) return;
	while (true) {
		poll(&v_pollfds[1], 1, 0);
		if (v_pollfds[1].revents != POLLIN) break;
		f_run_post();
	}
}

void t_application::f_poll(int a_descriptor, t_runnable* a_reader, t_runnable* a_writer)
{
	size_t i = 2;
	while (i < v_pollfds.size() && v_pollfds[i].fd != a_descriptor) ++i;
	if (a_reader || a_writer) {
		if (i >= v_pollfds.size()) {
			struct pollfd fd;
			fd.fd = a_descriptor;
			v_pollfds.push_back(fd);
			v_listeners.push_back(t_listener());
		}
		v_pollfds[i].events = v_pollfds[i].revents = 0;
		if (a_reader) v_pollfds[i].events |= POLLIN;
		if (a_writer) v_pollfds[i].events |= POLLOUT;
		i -= 2;
		v_listeners[i].v_reader = a_reader;
		v_listeners[i].v_writer = a_writer;
	} else if (i < v_pollfds.size()) {
		v_pollfds.erase(v_pollfds.begin() + i);
		i -= 2;
		v_listeners.erase(v_listeners.begin() + i);
		if (v_notifier > i) --v_notifier;
	}
}

void t_application::f_poll_reader(int a_descriptor, t_runnable* a_reader)
{
	size_t i = 2;
	while (i < v_pollfds.size() && v_pollfds[i].fd != a_descriptor) ++i;
	if (a_reader) {
		if (i >= v_pollfds.size()) {
			struct pollfd fd;
			fd.fd = a_descriptor;
			v_pollfds.push_back(fd);
			v_pollfds[i].events = 0;
			v_listeners.push_back(t_listener());
		}
		v_pollfds[i].events |= POLLIN;
		v_pollfds[i].revents = 0;
		i -= 2;
		v_listeners[i].v_reader = a_reader;
	} else if (i < v_pollfds.size()) {
		v_pollfds[i].events &= ~POLLIN;
		v_pollfds[i].revents = 0;
		if (v_pollfds[i].events == 0) {
			v_pollfds.erase(v_pollfds.begin() + i);
			i -= 2;
			v_listeners.erase(v_listeners.begin() + i);
			if (v_notifier > i) --v_notifier;
		} else {
			i -= 2;
			v_listeners[i].v_reader = 0;
		}
	}
}

void t_application::f_poll_writer(int a_descriptor, t_runnable* a_writer)
{
	size_t i = 2;
	while (i < v_pollfds.size() && v_pollfds[i].fd != a_descriptor) ++i;
	if (a_writer) {
		if (i >= v_pollfds.size()) {
			struct pollfd fd;
			fd.fd = a_descriptor;
			v_pollfds.push_back(fd);
			v_pollfds[i].events = 0;
			v_listeners.push_back(t_listener());
		}
		v_pollfds[i].events |= POLLOUT;
		v_pollfds[i].revents = 0;
		i -= 2;
		v_listeners[i].v_writer = a_writer;
	} else if (i < v_pollfds.size()) {
		v_pollfds[i].events &= ~POLLOUT;
		v_pollfds[i].revents = 0;
		if (v_pollfds[i].events == 0) {
			v_pollfds.erase(v_pollfds.begin() + i);
			i -= 2;
			v_listeners.erase(v_listeners.begin() + i);
			if (v_notifier > i) --v_notifier;
		} else {
			i -= 2;
			v_listeners[i].v_writer = 0;
		}
	}
}

void t_application::f_focus__(const t_pointer<t_window>& a_window)
{
	if (a_window == v_focus) return;
	if (v_focus && v_focus->v_input_context) v_focus->v_input_context->f_blur();
	t_pointer<t_window> focus = v_focus;
	v_focus = a_window;
	if (v_focus && v_focus->v_input_context) v_focus->v_input_context->f_focus();
	if (focus) focus->f_on_focus_leave();
	if (v_focus) v_focus->f_on_focus_enter();
}

void t_application::f_pointer_grabber__(const t_pointer<t_window>& a_window)
{
	if (a_window == v_pointer_grabber) return;
	if (a_window) {
		if (XGrabPointer(v_display, a_window->v_handle, False, ButtonPressMask | ButtonReleaseMask | PointerMotionMask, GrabModeAsync, GrabModeAsync, None, a_window->v_cursor, v_time) != GrabSuccess) return;
	} else {
		XUngrabPointer(v_display, v_time);
	}
	v_pointer_grabber = a_window;
}

void t_application::f_add(const t_pointer<t_shell>& a_shell, size_t a_i)
{
	if (a_i == size_t(-1)) a_i = v_shells.size();
	if (a_i == 0 && v_gravity != ForgetGravity) {
		a_shell->v_geometry = t_rectangle(v_x, v_y, v_width, v_height);
		a_shell->v_gravity = v_gravity;
	} else {
		a_shell->v_gravity = ForgetGravity;
	}
	if (a_shell->v_handle != None) {
		size_t i = 0;
		while (v_shells[i] != a_shell) ++i;
		v_shells.erase(v_shells.begin() + i);
		if (a_i > i) --a_i;
	} else {
		a_shell->f_create();
	}
	if (a_i < v_shells.size()) {
		XWindowChanges values;
		values.sibling = v_shells[a_i]->v_handle;
		values.stack_mode = Below;
		XConfigureWindow(v_display, a_shell->v_handle, CWSibling | CWStackMode, &values);
	} else {
		XRaiseWindow(v_display, a_shell->v_handle);
	}
	v_shells.insert(v_shells.begin() + a_i, a_shell);
}

void t_application::f_remove(size_t a_i)
{
	auto i = v_shells.begin() + a_i;
	(*i)->f_destroy();
	v_shells.erase(i);
}

namespace dbus
{

void t_bridge::t_watch::operator()()
{
	while (true) {
		bool b = dbus_watch_handle(v_watch, v_flags) != FALSE;
		while (dbus_connection_dispatch(v_connection) == DBUS_DISPATCH_DATA_REMAINS);
		if (b) break;
	}
}

void t_bridge::t_timeout::operator()()
{
	dbus_timeout_handle(v_timeout);
}

thread_local dbus_int32_t t_bridge::v_slot = -1;

void t_bridge::f_free(void* a_data)
{
//std::fprintf(stderr, "f_free(t_bridge)\n");
	delete static_cast<t_bridge*>(a_data);
}

dbus_bool_t t_bridge::f_add(DBusWatch* a_watch, void* a_data)
{
	auto p = static_cast<t_bridge*>(a_data);
	auto i = p->v_watches.emplace(a_watch, std::make_pair(t_watch(p->v_connection, a_watch, DBUS_WATCH_READABLE), t_watch(p->v_connection, a_watch, DBUS_WATCH_WRITABLE))).first;
	int fd = dbus_watch_get_unix_fd(a_watch);
	if (dbus_watch_get_enabled(a_watch) != FALSE) {
		unsigned int flags = dbus_watch_get_flags(a_watch);
		if ((flags & DBUS_WATCH_READABLE) != 0) f_application()->f_poll_reader(fd, &i->second.first);
		if ((flags & DBUS_WATCH_WRITABLE) != 0) f_application()->f_poll_writer(fd, &i->second.second);
	}
	return TRUE;
}

void t_bridge::f_remove(DBusWatch* a_watch, void* a_data)
{
	auto p = static_cast<t_bridge*>(a_data);
	f_application()->f_poll(dbus_watch_get_unix_fd(a_watch), 0, 0);
	p->v_watches.erase(a_watch);
}

void t_bridge::f_toggle(DBusWatch* a_watch, void* a_data)
{
	int fd = dbus_watch_get_unix_fd(a_watch);
	unsigned int flags = dbus_watch_get_flags(a_watch);
	if (dbus_watch_get_enabled(a_watch) == FALSE) {
		if ((flags & DBUS_WATCH_READABLE) != 0) f_application()->f_poll_reader(fd, 0);
		if ((flags & DBUS_WATCH_WRITABLE) != 0) f_application()->f_poll_writer(fd, 0);
	} else {
		auto p = static_cast<t_bridge*>(a_data);
		auto i = p->v_watches.find(a_watch);
		if ((flags & DBUS_WATCH_READABLE) != 0) f_application()->f_poll_reader(fd, &i->second.first);
		if ((flags & DBUS_WATCH_WRITABLE) != 0) f_application()->f_poll_writer(fd, &i->second.second);
	}
}

dbus_bool_t t_bridge::f_add(DBusTimeout* a_timeout, void* a_data)
{
	auto p = static_cast<t_bridge*>(a_data);
	auto i = p->v_timeouts.emplace(a_timeout, new t_timeout(a_timeout)).first;
	if (dbus_timeout_get_enabled(a_timeout) != FALSE) i->second->f_start(dbus_timeout_get_interval(a_timeout));
	return TRUE;
}

void t_bridge::f_remove(DBusTimeout* a_timeout, void* a_data)
{
	auto p = static_cast<t_bridge*>(a_data);
	auto i = p->v_timeouts.lower_bound(a_timeout);
	i->second->f_stop();
	p->v_timeouts.erase(i);
}

void t_bridge::f_toggle(DBusTimeout* a_timeout, void* a_data)
{
	auto p = static_cast<t_bridge*>(a_data);
	auto i = p->v_timeouts.find(a_timeout);
	if (dbus_timeout_get_enabled(a_timeout) == FALSE)
		i->second->f_stop();
	else
		i->second->f_start(dbus_timeout_get_interval(a_timeout));
}

t_bridge::t_bridge(DBusConnection* a_connection) : v_connection(a_connection)
{
	if (dbus_connection_allocate_data_slot(&v_slot) == FALSE) throw std::runtime_error("dbus_connection_allocate_data_slot failed.");
	if (dbus_connection_set_data(v_connection, v_slot, this, f_free) == FALSE) throw std::runtime_error("dbus_connection_set_data failed.");
	if (dbus_connection_set_watch_functions(v_connection, f_add, f_remove, f_toggle, this, NULL) == FALSE) throw std::runtime_error("dbus_connection_set_watch_functions failed.");
	if (dbus_connection_set_timeout_functions(v_connection, f_add, f_remove, f_toggle, this, NULL) == FALSE) throw std::runtime_error("dbus_connection_set_timeout_functions failed.");
}

}

}
