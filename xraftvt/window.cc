#include "common.h"

#include <cstdio>
#include <cstdlib>
#ifdef XRAFT_TRANSPARENT
#include <X11/Xatom.h>
#endif

void t_content::f_draw_row(t_graphics& a_g, int a_y, const t_row* a_row)
{
	const t_cell* cells = a_row->v_cells;
	unsigned width = f_geometry().v_width;
	if (!a_g.f_invalid(0, a_y, width, v_unit.v_height)) return;
#ifdef XRAFT_TRANSPARENT
	if (v_pixmap != None) {
		a_g.f_draw(0, a_y, v_pixmap, v_origin.v_x, v_origin.v_y + a_y, width, v_unit.v_height);
	} else {
		a_g.f_color(v_pixels[7]);
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
		::t_attribute a = cells[i].v_a;
		do {
			wchar_t c = cells[i++].v_c;
			if (c != L'\0') v_cs[n++] = c;
		} while (i < a_row->v_size && cells[i].v_a == a);
		unsigned w = i * v_unit.v_width - x;
		const t_pixel* pixels = v_pixels;
		if (a.f_blink()) pixels += 8;
		if (a.f_reverse()) pixels += 16;
#ifdef XRAFT_TRANSPARENT
		Display* display = f_application()->f_x11_display();
		if (pixels[a.f_background()] != BlackPixel(display, DefaultScreen(display)))
#endif
		{
			a_g.f_color(pixels[a.f_background()]);
			a_g.f_fill(x, a_y, w, v_unit.v_height);
		}
		a_g.f_color(pixels[a.f_foreground()]);
		a_g.f_draw(x, y, v_cs, n);
		if (a.f_bold()) a_g.f_draw(x + 1, y, v_cs, n);
		if (a.f_underline()) a_g.f_draw(x, u, w, 1);
		x += w;
	}
#ifndef XRAFT_TRANSPARENT
	a_g.f_color(v_pixels[7]);
	a_g.f_fill(x, a_y, width - x, v_unit.v_height);
#endif
}

void t_content::f_draw_cursor(t_graphics& a_g, int a_x, int a_y, const t_cell& a_cell)
{
	wchar_t c = a_cell.v_c;
	::t_attribute a = a_cell.v_a;
	const t_pixel* pixels = v_pixels;
	if (a.f_blink()) pixels += 8;
	if (a.f_reverse()) pixels += 16;
	a_g.f_color(pixels[a.f_foreground()]);
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
			a_g.f_color(pixels[a.f_background()]);
			int y = a_y + v_font->f_ascent();
			a_g.f_draw(a_x, y, &c, 1);
			if (a.f_bold()) a_g.f_draw(a_x + 1, y, &c, 1);
			if (a.f_underline()) a_g.f_draw(a_x, a_y + v_unit.v_height - 1, width, 1);
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
	if (v_cursor_x >= v_buffer.f_width()) v_cursor_x = v_buffer.f_width() - 1;
	if (v_cursor_y >= v_buffer.f_height()) v_cursor_y = v_buffer.f_height() - 1;
#ifndef XRAFT_TRANSPARENT
	f_invalidate(v_cursor_y, 1);
#endif
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
	if (v_pixmap == None) a_g.f_color(v_pixels[7]);
#else
	a_g.f_color(v_pixels[7]);
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
	y = (v_buffer.f_log_size() + v_cursor_y) * v_unit.v_height - v_position;
	if (a_g.f_invalid(0, y, extent.v_width, v_unit.v_height)) {
		int x = v_cursor_x * v_unit.v_width;
		const t_row* row = v_buffer.f_at(v_cursor_y);
		if (v_cursor_x < row->v_size)
			f_draw_cursor(a_g, x, y, row->v_cells[v_cursor_x]);
		else
			f_draw_cursor(a_g, x, y, t_cell(L' ', ::t_attribute(false, false, false, false, 0, 7)));
	}
}

void t_content::f_on_focus_enter()
{
	f_invalidate(v_cursor_y, 1);
}

void t_content::f_on_focus_leave()
{
	f_invalidate(v_cursor_y, 1);
}

t_rectangle t_content::f_on_input_spot()
{
	return t_rectangle(t_point(v_cursor_x * v_unit.v_width, (v_buffer.f_log_size() + v_cursor_y) * v_unit.v_height - v_position), v_unit);
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

void t_content::f_scroll_log()
{
	f_invalidate(v_cursor_y, 1);
	if (v_buffer.f_log_size() < v_buffer.f_log_capacity()) {
		v_position += v_unit.v_height;
		f_static_cast<t_pane>(f_parent())->f_invalidate_bar();
	}
	v_buffer.f_scroll_log();
	const t_extent& extent = f_geometry();
#ifdef XRAFT_TRANSPARENT
	f_invalidate(0, 0, extent.v_width, extent.v_height);
#else
	f_scroll(0, 0, extent.v_width, extent.v_height, 0, -v_unit.v_height);
	f_invalidate(v_cursor_y, 1);
#endif
}

void t_content::f_scroll_up(unsigned a_y, unsigned a_height, unsigned a_n)
{
	v_buffer.f_scroll_up(a_y, a_height, a_n);
	if (v_cursor_y >= a_y && v_cursor_y < a_y + static_cast<int>(a_height)) {
		f_invalidate(v_cursor_y, 1);
		f_scroll(a_y, a_height, -a_n);
		f_invalidate(v_cursor_y, 1);
	} else {
		f_scroll(a_y, a_height, -a_n);
	}
}

void t_content::f_scroll_down(unsigned a_y, unsigned a_height, unsigned a_n)
{
	v_buffer.f_scroll_down(a_y, a_height, a_n);
	if (v_cursor_y >= a_y && v_cursor_y < a_y + static_cast<int>(a_height)) {
		f_invalidate(v_cursor_y, 1);
		f_scroll(a_y, a_height, a_n);
		f_invalidate(v_cursor_y, 1);
	} else {
		f_scroll(a_y, a_height, a_n);
	}
}

void t_content::f_erase(unsigned a_y, unsigned a_height)
{
	v_buffer.f_erase(a_y, a_height);
	f_invalidate(a_y, a_height);
}

void t_content::f_insert(unsigned a_x, unsigned a_y, unsigned a_n)
{
	v_buffer.f_insert(a_x, a_y, a_n);
	f_invalidate(a_y, 1);
}

void t_content::f_put(unsigned a_x, unsigned a_y, wchar_t a_c, bool a_shift)
{
	v_buffer.f_put(a_x, a_y, t_cell(a_c, v_attribute), a_shift);
	f_invalidate(a_y, 1);
}

void t_content::f_erase(unsigned a_x, unsigned a_y, unsigned a_n, bool a_shift)
{
	v_buffer.f_erase(a_x, a_y, a_n, a_shift);
	f_invalidate(a_y, 1);
}

t_content::t_content(unsigned a_log, unsigned a_width, unsigned a_height) :
v_font(f_application()->f_font()), v_unit(v_font->f_width(), v_font->f_height()),
#ifdef XRAFT_TRANSPARENT
v_pixmap(None), v_moved(true),
#endif
v_buffer(a_log, a_width, a_height), v_position(0), v_cs(new wchar_t[a_width]),
v_attribute(false, false, false, false, 0, 7), v_cursor_x(0), v_cursor_y(0)
{
	const t_color colors[] = {
#ifdef XRAFT_TRANSPARENT
		t_color(255, 255, 255),
		t_color(255, 63, 63),
		t_color(63, 255, 63),
		t_color(255, 191, 63),
		t_color(63, 127, 255),
		t_color(255, 63, 255),
		t_color(63, 255, 255),
		t_color(0, 0, 0),
		t_color(255, 255, 255),
		t_color(255, 63, 63),
		t_color(63, 255, 63),
		t_color(255, 191, 63),
		t_color(63, 127, 255),
		t_color(255, 63, 255),
		t_color(63, 255, 255),
		t_color(0, 0, 0),
		t_color(0, 0, 0),
		t_color(255, 63, 63),
		t_color(63, 255, 63),
		t_color(255, 191, 63),
		t_color(63, 127, 255),
		t_color(255, 63, 255),
		t_color(63, 255, 255),
		t_color(255, 255, 255),
		t_color(0, 0, 0),
		t_color(255, 63, 63),
		t_color(63, 255, 63),
		t_color(255, 191, 63),
		t_color(63, 127, 255),
		t_color(255, 63, 255),
		t_color(63, 255, 255),
		t_color(255, 255, 255),
#else
		t_color("black"), t_color("red"),
		t_color("green"), t_color("yellow"),
		t_color("blue"), t_color("magenta"),
		t_color("cyan"), t_color("white"),
		t_color("black"), t_color("red"),
		t_color("green"), t_color("yellow"),
		t_color("blue"), t_color("magenta"),
		t_color("cyan"), t_color("white"),
		t_color("white"), t_color("red"),
		t_color("green"), t_color("yellow"),
		t_color("blue"), t_color("magenta"),
		t_color("cyan"), t_color("black"),
		t_color("white"), t_color("red"),
		t_color("green"), t_color("yellow"),
		t_color("blue"), t_color("magenta"),
		t_color("cyan"), t_color("black"),
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
}

#ifdef XRAFT_TRANSPARENT
void t_content::f_move(const t_rectangle& a_geometry)
{
	t_widget::f_move(a_geometry);
	v_moved = true;
	f_invalidate(0, 0, a_geometry.v_width, a_geometry.v_height);
}
#endif

unsigned t_content::f_content() const
{
	return v_buffer.f_log_size() * v_unit.v_height + f_geometry().v_height;
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

void t_content::f_viewable(int a_y, unsigned a_height)
{
	unsigned height = f_geometry().v_height;
	if (a_y < v_position || a_height > height) {
		f_position__(a_y);
	} else {
		int bottom0 = v_position + height;
		int bottom1 = a_y + a_height;
		if (bottom1 > bottom0) f_position__(bottom1 - height);
	}
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

t_pane::t_pane(const t_pointer<t_content>& a_content) :
v_content(a_content),
v_face_active(0, 47, 159), v_face_inactive(31, 63, 143),
v_gap(79, 79, 79),
v_text_active(223, 223, 223), v_text_inactive(127, 127, 127),
v_grabbing_part(e_part__NONE)
{
	f_add(v_content);
	t_rectangle geometry = v_content->f_geometry();
	geometry.v_width += f_bar_width();
	geometry.v_height += f_border_width() * 2;
	f_move(geometry);
}

void t_pane::f_hints(const char* a_name)
{
	const t_extent& unit = v_content->f_unit();
	XSizeHints hints;
	hints.flags = PMinSize | PResizeInc | PBaseSize;
	hints.min_width = f_bar_width() + unit.v_width;
	hints.min_height = f_border_width() * 2 + unit.v_height;
	hints.width_inc = unit.v_width;
	hints.height_inc = unit.v_height;
	hints.base_width = f_bar_width();
	hints.base_height = f_border_width() * 2;
	XmbSetWMProperties(f_application()->f_x11_display(), v_handle, a_name, a_name, NULL, 0, &hints, NULL, NULL);
}

void t_pane::f_invalidate_bar()
{
	const t_extent& extent = f_geometry();
	f_invalidate(extent.v_width - f_bar_width(), 0, f_bar_width(), extent.v_height);
}
