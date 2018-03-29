#include "window.h"
#include <cstring>
#include <sys/ioctl.h>
#ifdef XRAFT_TRANSPARENT
#include <X11/Xatom.h>
#endif

t_content::t_code t_content::f_code(t_key a_key)
{
	switch (a_key) {
	case e_key__TAB:
		return t_code::e_TAB;
	case e_key__BACK_SPACE:
		return t_code::e_BACK_SPACE;
	case e_key__FIND:
		return t_code::e_FIND;
	case e_key__INSERT:
		return t_code::e_INSERT;
	case e_key__EXECUTE:
		return t_code::e_EXECUTE;
	case e_key__SELECT:
		return t_code::e_SELECT;
	case e_key__PRIOR:
		return t_code::e_PRIOR;
	case e_key__NEXT:
		return t_code::e_NEXT;
	case e_key__HOME:
		return t_code::e_HOME;
	case e_key__END:
		return t_code::e_END;
	case e_key__DELETE:
		return t_code::e_DELETE;
	case e_key__F1:
		return t_code::e_F1;
	case e_key__F2:
		return t_code::e_F2;
	case e_key__F3:
		return t_code::e_F3;
	case e_key__F4:
		return t_code::e_F4;
	case e_key__F5:
		return t_code::e_F5;
	case e_key__F6:
		return t_code::e_F6;
	case e_key__F7:
		return t_code::e_F7;
	case e_key__F8:
		return t_code::e_F8;
	case e_key__F9:
		return t_code::e_F9;
	case e_key__F10:
		return t_code::e_F10;
	case e_key__F11:
		return t_code::e_F11;
	case e_key__F12:
		return t_code::e_F12;
/*
	case e_key__F13:
		return t_code::e_F13;
	case e_key__F14:
		return t_code::e_F14;
	case e_key__F15:
		return t_code::e_F15;
	case e_key__F16:
		return t_code::e_F16;
	case e_key__F17:
		return t_code::e_F17;
	case e_key__F18:
		return t_code::e_F18;
	case e_key__F19:
		return t_code::e_F19;
	case e_key__F20:
		return t_code::e_F20;
*/
	case e_key__UP:
		return t_code::e_UP;
	case e_key__DOWN:
		return t_code::e_DOWN;
	case e_key__RIGHT:
		return t_code::e_RIGHT;
	case e_key__LEFT:
		return t_code::e_LEFT;
	case e_key__KP_ENTER:
		return t_code::e_KP_ENTER;
	case e_key__KP_F1:
		return t_code::e_KP_F1;
	case e_key__KP_F2:
		return t_code::e_KP_F2;
	case e_key__KP_F3:
		return t_code::e_KP_F3;
	case e_key__KP_F4:
		return t_code::e_KP_F4;
	case e_key__KP_MULTIPLY:
		return t_code::e_KP_MULTIPLY;
	case e_key__KP_ADD:
		return t_code::e_KP_ADD;
	case e_key__KP_SEPARATOR:
		return t_code::e_KP_SEPARATOR;
	case e_key__KP_SUBTRACT:
		return t_code::e_KP_SUBTRACT;
	case e_key__KP_DECIMAL:
		return t_code::e_KP_DECIMAL;
	case e_key__KP_DIVIDE:
		return t_code::e_KP_DIVIDE;
	case e_key__KP_0:
		return t_code::e_KP_0;
	case e_key__KP_1:
		return t_code::e_KP_1;
	case e_key__KP_2:
		return t_code::e_KP_2;
	case e_key__KP_3:
		return t_code::e_KP_3;
	case e_key__KP_4:
		return t_code::e_KP_4;
	case e_key__KP_5:
		return t_code::e_KP_5;
	case e_key__KP_6:
		return t_code::e_KP_6;
	case e_key__KP_7:
		return t_code::e_KP_7;
	case e_key__KP_8:
		return t_code::e_KP_8;
	case e_key__KP_9:
		return t_code::e_KP_9;
	default:
		return t_code::e_NONE;
	}
}

void t_content::f_send(const char* a_cs, size_t a_n)
{
	while (a_n > 0) {
		ssize_t n = write(v_master, a_cs, a_n);
		if (n == ssize_t(-1)) {
std::fprintf(stderr, "write: %s\n", std::strerror(errno));
			break;
		}
		a_cs += n;
		a_n -= n;
	}
}

void t_content::f_send(const wchar_t* a_cs, size_t a_n)
{
	char mbs[MB_LEN_MAX];
	std::mbstate_t state;
	std::memset(&state, 0, sizeof(std::mbstate_t));
	for (size_t i = 0; i < a_n; ++i) {
		size_t n = std::wcrtomb(mbs, a_cs[i], &state);
		if (n != size_t(-1)) f_send(mbs, n);
	}
	size_t n = std::wcrtomb(mbs, L'\0', &state);
	if (n != size_t(-1)) f_send(mbs, n - 1);
}

bool t_content::f_send(t_modifier a_modifier, t_key a_key)
{
	t_code code = f_code(a_key);
	if (code == t_code::e_NONE) return false;
	size_t i = 0;
	if ((a_modifier & e_modifier__SHIFT) != 0) i |= 1;
	if ((a_modifier & e_modifier__CONTROL) != 0) i |= 2;
	const char* cs = v_buffer.f_code(code, i);
	if (cs) f_send(cs, std::strlen(cs));
	return true;
}

void t_content::operator()()
{
	ssize_t n = read(v_master, v_mbs + v_mbn, sizeof(v_mbs) - v_mbn);
	if (n == ssize_t(-1)) {
std::fprintf(stderr, "read: %s\n", std::strerror(errno));
		return;
	}
	v_mbn += n;
	if (v_mbn > 0) {
		f_invalidate(v_buffer.f_cursor_y(), 1);
		const char* p = v_mbs;
		const char* q = v_mbs + v_mbn;
		while (p < q) {
			wchar_t c;
			size_t n = std::mbrtowc(&c, p, q - p, &v_mbstate);
			if (n == size_t(-2)) {
				std::copy(p, q, v_mbs);
				std::memset(&v_mbstate, 0, sizeof(std::mbstate_t));
				break;
			} else if (n == size_t(-1)) {
				std::memset(&v_mbstate, 0, sizeof(std::mbstate_t));
				++p;
			} else if (n == 0) {
				v_buffer(L'\0');
				++p;
			} else {
				v_buffer(c);
				p += n;
			}
		}
		v_mbn = q - p;
		f_invalidate(v_buffer.f_cursor_y(), 1);
		f_application()->f_input_negotiate();
	} else {
		f_application()->f_exit();
	}
}

void t_content::f_draw_row(t_graphics& a_g, int a_y, const t_row* a_row)
{
	const t_cell* cells = a_row->v_cells;
	unsigned width = f_geometry().v_width;
	if (!a_g.f_invalid(0, a_y, width, v_unit.v_height)) return;
#ifdef XRAFT_TRANSPARENT
	if (v_pixmap != None) {
		a_g.f_draw(0, a_y, v_pixmap, v_origin.v_x, v_origin.v_y + a_y, width, v_unit.v_height);
	} else {
		a_g.f_color(v_pixels[1]);
		a_g.f_fill(0, a_y, width, v_unit.v_height);
	}
#endif
	a_g.f_font(v_font);
	int y = a_y + v_font->f_ascent();
	int u = a_y + v_unit.v_height - 1;
	int x = 0;
	unsigned i = 0;
	while (i < a_row->v_size) {
		unsigned n = 0;
		auto a = cells[i].v_a;
		do {
			wchar_t c = cells[i++].v_c;
			if (c != L'\0') v_cs[n++] = c;
		} while (i < a_row->v_size && cells[i].v_a == a);
		unsigned w = i * v_unit.v_width - x;
		const t_pixel* pixels = v_pixels;
		if (a.v_faint) pixels += 10;
		if (a.v_blink) pixels += 20;
		if (a.v_inverse) pixels += 40;
#ifdef XRAFT_TRANSPARENT
		Display* display = f_application()->f_x11_display();
		if (pixels[a.v_background] != BlackPixel(display, DefaultScreen(display)))
#endif
		{
			a_g.f_color(pixels[a.v_background]);
			a_g.f_fill(x, a_y, w, v_unit.v_height);
		}
		a_g.f_color(pixels[a.v_foreground]);
		a_g.f_draw(x, y, v_cs, n);
		if (a.v_bold) a_g.f_draw(x + 1, y, v_cs, n);
		if (a.v_underlined) a_g.f_draw(x, u, w, 1);
		x += w;
	}
#ifndef XRAFT_TRANSPARENT
	a_g.f_color(v_pixels[1]);
	a_g.f_fill(x, a_y, width - x, v_unit.v_height);
#endif
}

void t_content::f_draw_cursor(t_graphics& a_g, int a_x, int a_y, const t_cell& a_cell)
{
	wchar_t c = a_cell.v_c;
	auto a = a_cell.v_a;
	const t_pixel* pixels = v_pixels;
	if (a.v_faint) pixels += 10;
	if (a.v_blink) pixels += 20;
	if (a.v_inverse) pixels += 40;
	a_g.f_color(pixels[a.v_foreground]);
	if (c == L'\0') {
		unsigned width = v_unit.v_width / 2;
		unsigned height = v_unit.v_height / 2;
		a_x += (v_unit.v_width - width) / 2;
		a_y += (v_unit.v_height - height) / 2;
		if (this == f_application()->f_focus())
			a_g.f_fill(a_x, a_y, width, height);
		else
			a_g.f_draw(a_x, a_y, width - 1, height - 1);
	} else {
		unsigned width = wcwidth(c) * v_unit.v_width;
		if (this == f_application()->f_focus()) {
			a_g.f_fill(a_x, a_y, width, v_unit.v_height);
			a_g.f_color(pixels[a.v_background]);
			int y = a_y + v_font->f_ascent();
			a_g.f_draw(a_x, y, &c, 1);
			if (a.v_bold) a_g.f_draw(a_x + 1, y, &c, 1);
			if (a.v_underlined) a_g.f_draw(a_x, a_y + v_unit.v_height - 1, width, 1);
		} else {
			a_g.f_draw(a_x, a_y, width - 1, v_unit.v_height - 1);
		}
	}
}

void t_content::f_invalidate(int a_y, unsigned a_height)
{
	a_y = (a_y + v_buffer.f_log_size()) * v_unit.v_height - v_position;
	a_height *= v_unit.v_height;
	int bottom = a_y + a_height;
	int height = f_geometry().v_height;
	if (bottom < 0 || a_y >= height) return;
	if (a_y < 0) a_y = 0;
	if (bottom > height) bottom = height;
	f_invalidate(0, a_y, v_buffer.f_width() * v_unit.v_width, bottom - a_y);
}

void t_content::f_scroll(int a_y, unsigned a_height, int a_dy)
{
	a_y = (a_y + v_buffer.f_log_size()) * v_unit.v_height - v_position;
	a_height *= v_unit.v_height;
	int bottom = a_y + a_height;
	int height = f_geometry().v_height;
	if (bottom < 0 || a_y >= height) return;
	if (a_y < 0) a_y = 0;
	if (bottom > height) bottom = height;
#ifdef XRAFT_TRANSPARENT
	f_invalidate(0, a_y, v_buffer.f_width() * v_unit.v_width, bottom - a_y);
#else
	f_scroll(0, a_y, v_buffer.f_width() * v_unit.v_width, bottom - a_y, 0, a_dy * v_unit.v_height);
#endif
}

void t_content::f_log()
{
	v_position += v_unit.v_height;
	f_static_cast<t_pane>(f_parent())->f_invalidate_bar();
}

t_content::~t_content()
{
	delete[] v_cs;
}

void t_content::f_on_move()
{
	const t_extent& extent = f_geometry();
	unsigned width = v_buffer.f_width();
	unsigned height = v_buffer.f_height();
	v_buffer.f_resize(extent.v_width / v_unit.v_width, extent.v_height / v_unit.v_height);
	if (v_buffer.f_width() == width) {
		if (v_buffer.f_height() == height) return;
	} else {
		delete[] v_cs;
		v_cs = new wchar_t[v_buffer.f_width()];
	}
#ifndef XRAFT_TRANSPARENT
	f_invalidate(v_buffer.f_cursor_y(), 1);
#endif
	struct winsize ws;
	ws.ws_col = v_buffer.f_width();
	ws.ws_row = v_buffer.f_height();
	ws.ws_xpixel = ws.ws_ypixel = 0;
	ioctl(v_master, TIOCSWINSZ, &ws);
}

void t_content::f_on_paint(t_graphics& a_g)
{
#ifdef XRAFT_TRANSPARENT
	if (v_moved) {
		v_origin = f_to_screen(t_point(0, 0));
		v_moved = false;
	}
#endif
	const t_extent& extent = f_geometry();
	t_rectangle bounds = a_g.f_bounds();
	int y = bounds.v_y + v_position;
	int bottom = bounds.v_y + bounds.v_height;
	unsigned i = y / static_cast<int>(v_unit.v_height);
	y = i * v_unit.v_height - v_position;
	while (i < v_buffer.f_log_size()) {
		f_draw_row(a_g, y, v_buffer.f_log(i));
		if ((y += v_unit.v_height) >= bottom) break;
		++i;
	}
	if (y < bottom) {
		i -= v_buffer.f_log_size();
		while (i < v_buffer.f_height()) {
			f_draw_row(a_g, y, v_buffer.f_at(i));
			if ((y += v_unit.v_height) >= bottom) break;
			++i;
		}
	}
#ifdef XRAFT_TRANSPARENT
	if (v_pixmap == None) a_g.f_color(v_pixels[1]);
#else
	a_g.f_color(v_pixels[1]);
#endif
	y = (v_buffer.f_log_size() + v_buffer.f_height()) * v_unit.v_height - v_position;
	if (a_g.f_invalid(0, y, extent.v_width, extent.v_height - y))
#ifdef XRAFT_TRANSPARENT
	{
		if (v_pixmap == None)
			a_g.f_fill(0, y, extent.v_width, extent.v_height - y);
		else
			a_g.f_draw(0, y, v_pixmap, v_origin.v_x, v_origin.v_y + y, extent.v_width, extent.v_height - y);
	}
#else
		a_g.f_fill(0, y, extent.v_width, extent.v_height - y);
#endif
	y = (v_buffer.f_log_size() + v_buffer.f_cursor_y()) * v_unit.v_height - v_position;
	if (a_g.f_invalid(0, y, extent.v_width, v_unit.v_height)) {
		int x = v_buffer.f_cursor_x() * v_unit.v_width;
		const t_row* row = v_buffer.f_at(v_buffer.f_cursor_y());
		if (v_buffer.f_cursor_x() < row->v_size)
			f_draw_cursor(a_g, x, y, row->v_cells[v_buffer.f_cursor_x()]);
		else
			f_draw_cursor(a_g, x, y, {L' ', {false, false, false, false, false, 0, 1}});
	}
}

void t_content::f_on_focus_enter()
{
	f_invalidate(v_buffer.f_cursor_y(), 1);
}

void t_content::f_on_focus_leave()
{
	f_invalidate(v_buffer.f_cursor_y(), 1);
}

void t_content::f_on_key_press(t_modifier a_modifier, t_key a_key, char a_ascii)
{
	if (!f_send(a_modifier, a_key) && a_ascii != '\0') f_send(&a_ascii, 1);
	f_position__(INT_MAX);
}

void t_content::f_on_input_commit(const wchar_t* a_cs, size_t a_n)
{
	f_send(a_cs, a_n);
}

t_rectangle t_content::f_on_input_spot()
{
	return t_rectangle(t_point(v_buffer.f_cursor_x() * v_unit.v_width, (v_buffer.f_log_size() + v_buffer.f_cursor_y()) * v_unit.v_height - v_position), v_unit);
}

void t_content::f_on_button_press(t_modifier a_modifier, t_button a_button, int a_x, int a_y)
{
	switch (a_button) {
	case e_button__4:
		f_position__(v_position - v_unit.v_height * 4);
		break;
	case e_button__5:
		f_position__(v_position + v_unit.v_height * 4);
		break;
	}
}

t_content::t_content(unsigned a_log, unsigned a_width, unsigned a_height, int a_master) :
v_font(f_application()->f_font()), v_unit(v_font->f_width(), v_font->f_height()),
v_buffer(*this, a_log, a_width, a_height), v_master(a_master), v_cs(new wchar_t[a_width])
{
	const t_color colors[] = {
#ifdef XRAFT_TRANSPARENT
		t_color(255, 255, 255),
		t_color(0, 0, 0),
		t_color(0, 0, 0),
		t_color(255, 63, 63),
		t_color(63, 255, 63),
		t_color(255, 191, 63),
		t_color(63, 127, 255),
		t_color(255, 63, 255),
		t_color(63, 255, 255),
		t_color(255, 255, 255),

		t_color(127, 127, 127),
		t_color(0, 0, 0),
		t_color(63, 63, 63),
		t_color(127, 31, 31),
		t_color(31, 127, 31),
		t_color(127, 95, 31),
		t_color(31, 63, 127),
		t_color(127, 31, 127),
		t_color(31, 127, 127),
		t_color(127, 127, 127),

		t_color(255, 255, 255),
		t_color(0, 0, 0),
		t_color(0, 0, 0),
		t_color(255, 63, 63),
		t_color(63, 255, 63),
		t_color(255, 191, 63),
		t_color(63, 127, 255),
		t_color(255, 63, 255),
		t_color(63, 255, 255),
		t_color(255, 255, 255),

		t_color(127, 127, 127),
		t_color(0, 0, 0),
		t_color(63, 63, 63),
		t_color(127, 31, 31),
		t_color(31, 127, 31),
		t_color(127, 95, 31),
		t_color(31, 63, 127),
		t_color(127, 31, 127),
		t_color(31, 127, 127),
		t_color(127, 127, 127),

		t_color(0, 0, 0),
		t_color(255, 255, 255),
		t_color(0, 0, 0),
		t_color(255, 63, 63),
		t_color(63, 255, 63),
		t_color(255, 191, 63),
		t_color(63, 127, 255),
		t_color(255, 63, 255),
		t_color(63, 255, 255),
		t_color(255, 255, 255),

		t_color(0, 0, 0),
		t_color(127, 127, 127),
		t_color(63, 63, 63),
		t_color(127, 31, 31),
		t_color(31, 127, 31),
		t_color(127, 95, 31),
		t_color(31, 63, 127),
		t_color(127, 31, 127),
		t_color(31, 127, 127),
		t_color(127, 127, 127),

		t_color(0, 0, 0),
		t_color(255, 255, 255),
		t_color(0, 0, 0),
		t_color(255, 63, 63),
		t_color(63, 255, 63),
		t_color(255, 191, 63),
		t_color(63, 127, 255),
		t_color(255, 63, 255),
		t_color(63, 255, 255),
		t_color(255, 255, 255),

		t_color(0, 0, 0),
		t_color(127, 127, 127),
		t_color(63, 63, 63),
		t_color(127, 31, 31),
		t_color(31, 127, 31),
		t_color(127, 95, 31),
		t_color(31, 63, 127),
		t_color(127, 31, 127),
		t_color(31, 127, 127),
		t_color(127, 127, 127),
#else
		t_color("black"), t_color("white"),
		t_color("black"), t_color("red"),
		t_color("green"), t_color("goldenrod"),
		t_color("blue"), t_color("magenta"),
		t_color("cyan"), t_color("white"),

		t_color("gray"), t_color("white"),
		t_color("dim gray"), t_color("dark red"),
		t_color("dark green"), t_color("dark goldenrod"),
		t_color("dark blue"), t_color("dark magenta"),
		t_color("dark cyan"), t_color("gray"),

		t_color("black"), t_color("white"),
		t_color("black"), t_color("red"),
		t_color("green"), t_color("goldenrod"),
		t_color("blue"), t_color("magenta"),
		t_color("cyan"), t_color("white"),

		t_color("gray"), t_color("white"),
		t_color("dim gray"), t_color("dark red"),
		t_color("dark green"), t_color("dark goldenrod"),
		t_color("dark blue"), t_color("dark magenta"),
		t_color("dark cyan"), t_color("gray"),

		t_color("white"), t_color("black"),
		t_color("black"), t_color("red"),
		t_color("green"), t_color("goldenrod"),
		t_color("blue"), t_color("magenta"),
		t_color("cyan"), t_color("white"),

		t_color("white"), t_color("gray"),
		t_color("dim gray"), t_color("dark red"),
		t_color("dark green"), t_color("dark goldenrod"),
		t_color("dark blue"), t_color("dark magenta"),
		t_color("dark cyan"), t_color("gray"),

		t_color("white"), t_color("black"),
		t_color("black"), t_color("red"),
		t_color("green"), t_color("goldenrod"),
		t_color("blue"), t_color("magenta"),
		t_color("cyan"), t_color("white"),

		t_color("white"), t_color("gray"),
		t_color("dim gray"), t_color("dark red"),
		t_color("dark green"), t_color("dark goldenrod"),
		t_color("dark blue"), t_color("dark magenta"),
		t_color("dark cyan"), t_color("gray"),
#endif
	};
	for (size_t i = 0; i < sizeof(colors) / sizeof(t_color); ++i) v_pixels[i] = colors[i].f_pixel();
#ifdef XRAFT_TRANSPARENT
	Display* display = f_application()->f_x11_display();
	Atom XRAFT_BACKGROUND = XInternAtom(display, "XRAFT_BACKGROUND", True);
	if (XRAFT_BACKGROUND != None) {
		Atom actual_type;
		int actual_format;
		unsigned long nitems;
		unsigned long bytes_after;
		unsigned char* prop;
		if (XGetWindowProperty(display, DefaultRootWindow(display), XRAFT_BACKGROUND, 0, 1, False, XA_PIXMAP, &actual_type, &actual_format, &nitems, &bytes_after, &prop) == Success) {
			if (actual_type == XA_PIXMAP && actual_format == 32 && nitems == 1) v_pixmap = *reinterpret_cast<Pixmap*>(prop);
			XFree(prop);
		}
	}
#endif
	f_move(t_rectangle(0, 0, a_width * v_unit.v_width, a_height * v_unit.v_height));
	f_cursor__(f_application()->f_cursor_ibeam());
	f_input_context__(new t_input_context());
	std::memset(&v_mbstate, 0, sizeof(std::mbstate_t));
	f_application()->f_poll(v_master, this, 0);
}

void t_content::f_position__(int a_position)
{
	const t_extent& extent = f_geometry();
	unsigned content = f_content();
	if (a_position < 0)
		a_position = 0;
	else if (a_position > static_cast<int>(content - extent.v_height))
		a_position = content - extent.v_height;
	if (a_position == v_position) return;
#ifdef XRAFT_TRANSPARENT
	f_invalidate(0, 0, extent.v_width, extent.v_height);
#else
	f_scroll(0, 0, extent.v_width, extent.v_height, 0, v_position - a_position);
#endif
	v_position = a_position;
	f_static_cast<t_pane>(f_parent())->f_invalidate_bar();
}

t_colors::t_colors(unsigned char a_red, unsigned char a_green, unsigned char a_blue) :
v_base(a_red, a_green, a_blue),
v_lighter0(a_red + (0xff - a_red) / 4, a_green + (0xff - a_green) / 4, a_blue + (0xff - a_blue) / 4),
v_lighter1(a_red + (0xff - a_red) / 2, a_green + (0xff - a_green) / 2, a_blue + (0xff - a_blue) / 2),
v_lighter2(a_red + (0xff - a_red) / 4, a_green + (0xff - a_green) / 4, a_blue + (0xff - a_blue) / 4),
v_lighter3(a_red + (0xff - a_red) / 8, a_green + (0xff - a_green) / 8, a_blue + (0xff - a_blue) / 8),
v_darker0(a_red * 15 / 16, a_green * 15 / 16, a_blue * 15 / 16),
v_darker1(a_red * 3 / 4, a_green * 3 / 4, a_blue * 3 / 4),
v_darker2(a_red / 2, a_green / 2, a_blue / 2),
v_darker3(0, 0, 0)
{
	v_lighters[0] = v_lighter0.f_pixel();
	v_lighters[1] = v_lighter1.f_pixel();
	v_lighters[2] = v_lighter2.f_pixel();
	v_lighters[3] = v_lighter3.f_pixel();
	v_darkers[0] = v_darker0.f_pixel();
	v_darkers[1] = v_darker1.f_pixel();
	v_darkers[2] = v_darker2.f_pixel();
	v_darkers[3] = v_darker3.f_pixel();
}

void t_pane::f_on_move()
{
	const t_extent& extent = f_geometry();
	unsigned width = extent.v_width - f_bar_width();
	unsigned height = extent.v_height - f_border_width() * 2;
	v_content->f_move(t_rectangle(0, f_border_width(), width, height));
	f_invalidate(0, 0, extent.v_width, extent.v_height);
}

void t_pane::f_on_paint(t_graphics& a_g)
{
	const t_extent& extent = f_geometry();
	unsigned bar = f_bar_width();
	const t_colors& face = this == f_application()->f_active() ? v_face_active : v_face_inactive;
	if (a_g.f_invalid(0, 0, extent.v_width - bar, extent.v_height)) {
		int y0 = extent.v_height - f_border_width();
		int w = extent.v_width - bar;
		for (unsigned i = 0; i < f_border_width(); ++i) {
			a_g.f_color(face.v_darkers[i]);
			a_g.f_fill(0, i, w, 1);
			a_g.f_color(face.v_lighters[i]);
			a_g.f_fill(0, y0 + i, w, 1);
		}
	}
	if (a_g.f_invalid(extent.v_width - bar, 0, bar, extent.v_height)) {
		int x = extent.v_width - bar;
		unsigned viewport = v_content->f_geometry().v_height;
		unsigned content = v_content->f_content();
		a_g.f_color(face.v_base.f_pixel());
		if (content <= viewport) {
			a_g.f_fill(x, 0, bar, extent.v_height);
			return;
		}
		if (extent.v_height > bar * 3) {
			unsigned gap_end = extent.v_height - bar;
			unsigned gap = gap_end - bar;
			unsigned thumb = static_cast<double>(gap) * viewport / content;
			if (thumb < bar) thumb = bar;
			unsigned thumb_begin = bar + static_cast<unsigned>(static_cast<double>(gap - thumb) * v_content->f_position() / (content - viewport));
			unsigned thumb_end = thumb_begin + thumb;
			a_g.f_fill(x, 0, bar, bar);
			a_g.f_fill(x, gap_end, bar, extent.v_height - gap_end);
			a_g.f_fill(x, thumb_begin, bar, thumb);
			unsigned h0 = thumb_begin - bar;
			unsigned h1 = gap_end - thumb_end;
			a_g.f_color(v_gap.v_base.f_pixel());
			a_g.f_fill(x + 4, bar, bar - 8, h0);
			a_g.f_fill(x + 4, thumb_end, bar - 8, h1);
			for (size_t i = 0; i < 4; ++i) {
				a_g.f_color(v_gap.v_darkers[3 - i]);
				a_g.f_fill(x + i, bar, 1, h0);
				a_g.f_fill(x + i, thumb_end, 1, h1);
				a_g.f_color(v_gap.v_lighters[3 - i]);
				a_g.f_fill(x + bar - 4 + i, bar, 1, h0);
				a_g.f_fill(x + bar - 4 + i, thumb_end, 1, h1);
			}
		} else {
			a_g.f_fill(x, 0, bar, extent.v_height);
		}
		a_g.f_color((this == f_application()->f_active() ? v_text_active : v_text_inactive).f_pixel());
		t_point points[3] = {
			t_point(x + 1, bar - 3),
			t_point(x + bar / 2, bar / 2 - 2),
			t_point(x + bar - 1, bar - 3)
		};
		a_g.f_fill(points, 3);
		++points[0].v_x;
		--points[2].v_x;
		points[0].v_y = points[2].v_y = extent.v_height - bar + 3;
		points[1].v_y = points[0].v_y + bar / 2 - 2;
		a_g.f_fill(points, 3);
	}
}

void t_pane::f_on_focus_enter()
{
	f_application()->f_focus__(v_content);
}

void t_pane::f_on_button_press(t_modifier a_modifier, t_button a_button, int a_x, int a_y)
{
	const t_extent& extent = f_geometry();
	int bar = f_bar_width();
	if (a_x < static_cast<int>(extent.v_width) - bar) return;
	unsigned viewport = v_content->f_geometry().v_height;
	unsigned content = v_content->f_content();
	if (v_grabbing_part != e_part__NONE || content <= viewport) return;
	int position = v_content->f_position();
	if (static_cast<int>(extent.v_height) > bar * 2) {
		int gap_end = extent.v_height - bar;
		int gap = gap_end - bar;
		if (gap > bar) {
			int thumb = static_cast<double>(gap) * viewport / content;
			if (thumb < bar) thumb = bar;
			int thumb_begin = bar + static_cast<int>(static_cast<double>(gap - thumb) * position / (content - viewport));
			if (a_y < 0) {
			} else if (a_y < bar) {
				v_grabbing_part = e_part__BUTTON_UP;
			} else if (a_y < thumb_begin) {
				v_grabbing_part = e_part__GAP_UP;
			} else if (a_y < thumb_begin + thumb) {
				v_grabbing_part = e_part__THUMB;
				v_grabbing_offset = a_y - thumb_begin + bar;
			} else if (a_y < gap_end) {
				v_grabbing_part = e_part__GAP_DOWN;
			} else if (a_y < static_cast<int>(extent.v_height)) {
				v_grabbing_part = e_part__BUTTON_DOWN;
			}
		} else if (a_y >= 0 && a_y < bar) {
			v_grabbing_part = e_part__BUTTON_UP;
		} else if (a_y >= gap_end && a_y < static_cast<int>(extent.v_height)) {
			v_grabbing_part = e_part__BUTTON_DOWN;
		}
	} else {
		v_grabbing_part = a_y < static_cast<int>(extent.v_height) / 2 ? e_part__BUTTON_UP : e_part__BUTTON_DOWN;
	}
	switch (v_grabbing_part) {
	case e_part__BUTTON_UP:
		v_content->f_position__(position - v_content->f_unit().v_height);
		break;
	case e_part__GAP_UP:
		v_content->f_position__(position - viewport);
		break;
	case e_part__GAP_DOWN:
		v_content->f_position__(position + viewport);
		break;
	case e_part__BUTTON_DOWN:
		v_content->f_position__(position + v_content->f_unit().v_height);
		break;
	}
}

void t_pane::f_on_button_release(t_modifier a_modifier, t_button a_button, int a_x, int a_y)
{
	v_grabbing_part = e_part__NONE;
}

void t_pane::f_on_pointer_move(t_modifier a_modifier, int a_x, int a_y)
{
	if (v_grabbing_part != e_part__THUMB) return;
	a_y -= v_grabbing_offset;
	unsigned content = v_content->f_content();
	unsigned viewport = v_content->f_geometry().v_height;
	unsigned gap = f_geometry().v_height - f_bar_width() * 2;
	unsigned thumb = static_cast<double>(gap) * viewport / content;
	gap -= thumb < f_bar_width() ? f_bar_width() : thumb;
	v_content->f_position__(static_cast<double>(a_y) * (content - viewport) / gap);
}

void t_pane::f_on_activate()
{
	const t_extent& extent = f_geometry();
	f_invalidate(0, 0, extent.v_width, extent.v_height);
}

void t_pane::f_on_deactivate()
{
	const t_extent& extent = f_geometry();
	f_invalidate(0, 0, extent.v_width, extent.v_height);
}

void t_pane::f_on_close()
{
	f_application()->f_exit();
}
