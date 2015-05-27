#include <xraft/input_context.h>

#include <cstring>
#include <X11/keysym.h>
#include <xraft/ibus.h>
#include <xraft/application.h>
#include <xraft/graphics.h>

namespace xraft
{

t_input_context::t_component::~t_component()
{
	f_hide();
}

void t_input_context::t_component::f_show(int a_x, int a_y, unsigned a_width, unsigned a_height)
{
	Display* display = f_application()->f_x11_display();
	if (v_handle == None) {
		Window root = DefaultRootWindow(display);
		XSetWindowAttributes attributes;
		attributes.event_mask = KeyPressMask;
		attributes.override_redirect = True;
		v_handle = XCreateWindow(display, root, a_x, a_y, a_width, a_height, 0, CopyFromParent, CopyFromParent, CopyFromParent, CWEventMask | CWOverrideRedirect, &attributes);
	} else {
		XMoveResizeWindow(display, v_handle, a_x, a_y, a_width, a_height);
	}
	XMapRaised(display, v_handle);
}

void t_input_context::t_component::f_hide()
{
	if (v_handle == None) return;
	XDestroyWindow(f_application()->f_x11_display(), v_handle);
	v_handle = None;
}

void t_input_context::f_negotiate()
{
	if (!f_application()->v_input_negotiating) return;
	f_application()->v_input_negotiating = false;
	t_pointer<t_input_context> context;
	if (f_application()->v_focus) context = f_application()->v_focus->v_input_context;
	if (context) {
		context->f_preedit();
		t_rectangle spot = f_application()->v_focus->f_on_input_spot();
		t_point point = f_application()->v_focus->f_to_screen(spot);
		spot.v_width = 0;
		context->f_send("SetCursorLocation", DBUS_TYPE_INT32, &point.v_x, DBUS_TYPE_INT32, &point.v_y, DBUS_TYPE_INT32, &spot.v_width, DBUS_TYPE_INT32, &spot.v_height, DBUS_TYPE_INVALID);
	} else {
		f_application()->v_input_first->f_hide();
		f_application()->v_input_middle->f_hide();
		f_application()->v_input_last->f_hide();
	}
}

t_input_context::~t_input_context()
{
	if (v_context.empty()) return;
	dbus::t_connection& bus = f_application()->f_input_bus();
	bus.f_remove_disconnected(this, dbus::f_slot_member<t_input_context, &t_input_context::f_on_disconnected>);
	bus.f_remove_match(v_context.c_str(), "org.freedesktop.IBus.InputContext", "CommitText");
	bus.f_remove_match(v_context.c_str(), "org.freedesktop.IBus.InputContext", "UpdatePreeditText");
	bus.f_send("org.freedesktop.IBus", v_context.c_str(), "org.freedesktop.IBus.Service", "Destroy", DBUS_TYPE_INVALID);
}

void t_input_context::f_text(DBusMessageIter& a_i, std::vector<wchar_t>& a_cs, std::vector<t_input_attribute>& a_as)
{
	ibus::t_variant_parser p0(a_i);
	const char* value;
	p0 >> value;
	a_cs.clear();
	v_converter(value, value + std::strlen(value), std::back_inserter(a_cs));
	a_as.assign(a_cs.size(), e_input_attribute__NONE);
	ibus::t_variant_parser p1(p0);
	if (dbus_message_iter_get_arg_type(p1) != DBUS_TYPE_ARRAY) throw std::runtime_error("dbus_message_iter_get_arg_type must be DBUS_TYPE_ARRAY.");
	DBusMessageIter i;
	for (dbus_message_iter_recurse(p1, &i); dbus_message_iter_get_arg_type(&i) != DBUS_TYPE_INVALID; dbus_message_iter_next(&i)) {
		ibus::t_variant_parser p2(i);
		dbus_uint32_t type;
		dbus_uint32_t value;
		dbus_uint32_t start;
		dbus_uint32_t end;
		p2 >> type >> value >> start >> end;
//std::fprintf(stderr, "attribute: %d, %x, %d, %d\n", type, value, start, end);
		if (start > a_as.size()) start = a_as.size();
		if (end > a_as.size()) end = a_as.size();
		if (start >= end) continue;
		t_input_attribute a = e_input_attribute__NONE;
		switch (type) {
		case ibus::e_attribute_type__UNDERLINE:
			if (value != ibus::e_attribute_underline__NONE) a = e_input_attribute__UNDERLINE;
			break;
		case ibus::e_attribute_type__FOREGROUND:
			a = e_input_attribute__REVERSE;
			break;
		case ibus::e_attribute_type__BACKGROUND:
			a = e_input_attribute__REVERSE;
			break;
		}
		if (a != e_input_attribute__NONE) for (size_t i = start; i < end; ++i) a_as[i] = static_cast<t_input_attribute>(a_as[i] | a);
	}
}

void t_input_context::f_create()
{
	if (!v_context.empty()) return;
	dbus::t_connection& bus = f_application()->f_input_bus();
	if (!bus) return;
	const char* name = "XRAFT";
	dbus::t_message message = bus.f_send("org.freedesktop.IBus", "/org/freedesktop/IBus", "org.freedesktop.IBus", "CreateInputContext", DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID)();
	const char* p;
	message.f_get(DBUS_TYPE_OBJECT_PATH, &p, DBUS_TYPE_INVALID);
	v_context = p;
//std::fprintf(stderr, "CreateInputContext: %s\n", v_context.c_str());
	bus.f_add_disconnected(this, dbus::f_slot_member<t_input_context, &t_input_context::f_on_disconnected>);
	bus.f_add_match(this, dbus::f_slot_member<t_input_context, &t_input_context::f_on_commit_text>, v_context.c_str(), "org.freedesktop.IBus.InputContext", "CommitText");
	bus.f_add_match(this, dbus::f_slot_member<t_input_context, &t_input_context::f_on_update_preedit_text>, v_context.c_str(), "org.freedesktop.IBus.InputContext", "UpdatePreeditText");
	dbus_uint32_t capabilities = ibus::e_capability__PREEDIT_TEXT | ibus::e_capability__FOCUS;
	f_send("SetCapabilities", DBUS_TYPE_UINT32, &capabilities, DBUS_TYPE_INVALID);
}

void t_input_context::f_on_disconnected()
{
	v_context.clear();
}

void t_input_context::f_on_commit_text(dbus::t_message& a_message)
{
	f_application()->v_input_negotiating = true;
	if (!f_application()->v_focus) return;
	std::vector<wchar_t> cs;
	std::vector<t_input_attribute> as;
	DBusMessageIter i;
	dbus_message_iter_init(a_message, &i);
	f_text(i, cs, as);
	f_application()->v_focus->f_on_input_commit(&cs[0], cs.size());
}

void t_input_context::f_on_update_preedit_text(dbus::t_message& a_message)
{
	f_application()->v_input_negotiating = true;
	DBusMessageIter i;
	dbus_message_iter_init(a_message, &i);
	f_text(i, v_cs, v_as);
	dbus_message_iter_next(&i);
	dbus_uint32_t cursor;
	bool visible;
	i >> cursor >> visible;
//std::fprintf(stderr, "preedit: %d, %s\n", cursor, visible ? "true" : "false");
	if (!f_application()->v_focus) return;
	f_application()->v_input_composing = false;
	f_application()->v_focus->f_on_input_compose();
}

dbus::t_reply t_input_context::f_send(const char* a_method, int a_type, ...) const
{
	if (v_context.empty()) return dbus::t_reply();
//std::fprintf(stderr, "send: %s\n", a_method);
	dbus::t_connection& bus = f_application()->f_input_bus();
	va_list list;
	va_start(list, a_type);
	dbus::t_reply reply = bus.f_send_valist("org.freedesktop.IBus", v_context.c_str(), "org.freedesktop.IBus.InputContext", a_method, a_type, list);
	va_end(list);
	return reply;
}

void t_input_context::f_focus()
{
	f_create();
	f_application()->v_input_negotiating = true;
	if (v_context.empty()) return;
	if (f_application()->v_focus && !v_cs.empty()) {
		f_application()->v_input_composing = false;
		f_application()->v_focus->f_on_input_compose();
	}
	if (f_application()->v_active) f_send("FocusIn", DBUS_TYPE_INVALID);
}

void t_input_context::f_blur()
{
	f_application()->v_input_negotiating = true;
	if (v_context.empty()) return;
/*	if (f_application()->v_focus && !v_cs.empty()) {
		f_application()->v_input_composing = false;
		f_application()->v_focus->f_on_input_compose();
	}*/
	if (f_application()->v_active) f_send("FocusOut", DBUS_TYPE_INVALID);
}

void t_input_context::f_process(XKeyEvent& a_xkey)
{
	f_application()->v_input_negotiating = true;
	char cs[16];
	KeySym symbol;
	int n = XLookupString(&a_xkey, cs, sizeof(cs), &symbol, 0);
	if (!v_context.empty()) {
		dbus_uint32_t value = symbol;
		dbus_uint32_t code = a_xkey.keycode - 8;
		dbus_uint32_t state = a_xkey.state;
		if (a_xkey.type == KeyRelease) state |= ibus::e_modifier__RELEASE;
		dbus::t_message message = f_send("ProcessKeyEvent", DBUS_TYPE_UINT32, &value, DBUS_TYPE_UINT32, &code, DBUS_TYPE_UINT32, &state, DBUS_TYPE_INVALID)();
		if (dbus_message_get_type(message) == DBUS_MESSAGE_TYPE_ERROR) {
std::fprintf(stderr, "error: %s\n", dbus_message_get_error_name(message));
		} else {
			dbus_bool_t handled;
			message.f_get(DBUS_TYPE_BOOLEAN, &handled, DBUS_TYPE_INVALID);
			if (handled != FALSE) return;
		}
	}
	if (symbol >= XK_a && symbol <= XK_z) symbol -= XK_a - XK_A;
	if (a_xkey.type == KeyPress)
		f_application()->v_focus->f_on_key_press(static_cast<t_modifier>(a_xkey.state), static_cast<t_key>(symbol), n > 0 ? cs[0] : '\0');
	else
		f_application()->v_focus->f_on_key_release(static_cast<t_modifier>(a_xkey.state), static_cast<t_key>(symbol), n > 0 ? cs[0] : '\0');
}

void t_input_context::f_draw(const t_pointer<t_input_context::t_component>& a_component, const wchar_t* a_cs, const t_input_attribute* a_as, const t_input_context::t_line* a_lines, size_t a_n, unsigned a_width)
{
	t_graphics g(a_component);
	const t_color colors[][3] = {
		{
			t_color(0, 0, 0),
			t_color(255, 255, 255),
			t_color(127, 127, 127)
		}, {
			t_color(255, 255, 255),
			t_color(0, 0, 0),
			t_color(127, 127, 127)
		}, {
			t_color(0, 0, 0),
			t_color(255, 255, 255),
			t_color(0, 0, 0)
		}, {
			t_color(255, 255, 255),
			t_color(0, 0, 0),
			t_color(255, 255, 255)
		}
	};
	t_pointer<t_font> font = f_application()->f_font();
	unsigned h = font->f_height();
	unsigned a = font->f_ascent();
	int y = 0;
	for (size_t i = 0; i < a_n; ++i) {
		const t_line& line = a_lines[i];
		const wchar_t* p = a_cs + line.v_i;
		const t_input_attribute* q = a_as + line.v_i;
		const t_input_attribute* r = a_as + line.v_j;
		int x = 0;
		while (q < r) {
			t_input_attribute b = *q;
			size_t n = 0;
			do ++n; while (++q < r && *q == b);
			const t_color* c = colors[b];
			unsigned w = font->f_width(p, n);
			g.f_color(c[1].f_pixel());
			g.f_fill(x, y, w, h);
			g.f_color(c[0].f_pixel());
			g.f_draw(x, y + a, p, n);
			g.f_color(c[2].f_pixel());
			g.f_fill(x, y + h - 1, w, 1);
			x += w;
			p += n;
		}
		g.f_color(colors[0][1].f_pixel());
		g.f_fill(x, y, a_width - x, h);
		y += h;
	}
}

void t_input_context::f_preedit()
{
	if (!f_application()->v_input_composing) return;
	if (v_cs.empty()) {
		f_application()->v_input_first->f_hide();
		f_application()->v_input_middle->f_hide();
		f_application()->v_input_last->f_hide();
		return;
	}
	int left = f_application()->v_focus->f_to_screen(t_point(0, 0)).v_x;
	int right = left + f_application()->v_focus->f_geometry().v_width;
	t_rectangle rectangle = f_application()->v_focus->f_on_input_spot();
	t_point spot = f_application()->v_focus->f_to_screen(rectangle);
	spot.v_y += rectangle.v_height;
	t_pointer<t_font> font = f_application()->f_font();
	unsigned h = font->f_height();
	spot.v_y -= h;
	std::vector<t_line> lines;
	lines.push_back(t_line(0, spot.v_x));
	size_t i = 0;
	while (true) {
		t_line& line = lines.back();
		while (true) {
			if (v_cs[i] == L'\n') {
				line.v_j = i++;
				break;
			}
			int w = font->f_width(v_cs[i]);
			if (line.v_x + w > right) {
				if (line.v_x <= left) {
					line.v_x += w;
					++i;
				}
				line.v_j = i;
				break;
			}
			line.v_x += w;
			if (++i >= v_cs.size()) {
				line.v_j = i;
				break;
			}
		}
		if (i >= v_cs.size()) break;
		lines.push_back(t_line(i, left));
	}
	if (lines[0].v_x - spot.v_x > 0) {
		f_application()->v_input_first->f_show(spot.v_x, spot.v_y, lines[0].v_x - spot.v_x, h);
		f_draw(f_application()->v_input_first, &v_cs[0], &v_as[0], &lines[0], 1, lines[0].v_x - spot.v_x);
	} else {
		f_application()->v_input_first->f_hide();
	}
	if (lines.size() > 2) {
		f_application()->v_input_middle->f_show(left, spot.v_y + h, right - left, h * (lines.size() - 2));
		f_draw(f_application()->v_input_middle, &v_cs[0], &v_as[0], &lines[1], lines.size() - 2, right - left);
	} else {
		f_application()->v_input_middle->f_hide();
	}
	if (lines.size() > 1 && lines.back().v_x - left > 0) {
		f_application()->v_input_last->f_show(left, spot.v_y + h * (lines.size() - 1), lines.back().v_x - left, h);
		f_draw(f_application()->v_input_last, &v_cs[0], &v_as[0], &lines[lines.size() - 1], 1, lines.back().v_x - left);
	} else {
		f_application()->v_input_last->f_hide();
	}
}

}
