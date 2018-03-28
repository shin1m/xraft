#ifndef XRAFTVT__WINDOW_H
#define XRAFTVT__WINDOW_H

#include "terminal.h"
#include <xraft/window.h>
#include <xraft/graphics.h>

using namespace xraft;

class t_content : public t_widget, t_runnable
{
	friend class t_buffer<t_content>;
	friend class t_terminal<t_content>;
	using t_cell = t_buffer<t_content>::t_cell;
	using t_row = t_buffer<t_content>::t_row;
	using t_code = t_terminal<t_content>::t_code;

	static t_code f_code(t_key a_key);

	int v_master;
	char v_mbs[MB_LEN_MAX * 256];
	size_t v_mbn = 0;
	std::mbstate_t v_mbstate;
	t_pixel v_pixels[80];
	t_pointer<t_font> v_font;
	t_extent v_unit;
#ifdef XRAFT_TRANSPARENT
	Pixmap v_pixmap = None;
	bool v_moved = true;
	t_point v_origin;
#endif
	t_terminal<t_content> v_buffer;
	int v_position = 0;
	wchar_t* v_cs;

	void f_send(const char* a_cs, size_t a_n);
	void f_send(const wchar_t* a_cs, size_t a_n);
	bool f_send(t_modifier a_modifier, t_key a_key);
	virtual void operator()();
	void f_draw_row(t_graphics& a_g, int a_y, const t_row* a_row);
	void f_draw_cursor(t_graphics& a_g, int a_x, int a_y, const t_cell& a_cell);
	void f_invalidate(int a_y, unsigned a_height);
	void f_scroll(int a_y, unsigned a_height, int a_dy);
	void f_log();
	int f_width(wchar_t a_c) const
	{
		return wcwidth(a_c);
	}
	void f_bell()
	{
		XBell(f_application()->f_x11_display(), 0);
	}

protected:
	virtual ~t_content();
	virtual void f_on_move();
	virtual void f_on_paint(t_graphics& a_g);
	virtual void f_on_focus_enter();
	virtual void f_on_focus_leave();
	virtual void f_on_key_press(t_modifier a_modifier, t_key a_key, char a_ascii);
	virtual void f_on_input_commit(const wchar_t* a_cs, size_t a_n);
	virtual t_rectangle f_on_input_spot();
	virtual void f_on_button_press(t_modifier a_modifier, t_button a_button, int a_x, int a_y);

public:
	t_content(unsigned a_log, unsigned a_width, unsigned a_height, int a_master);
#ifdef XRAFT_TRANSPARENT
	void f_move(const t_rectangle& a_geometry)
	{
		t_widget::f_move(a_geometry);
		v_moved = true;
		f_invalidate(0, 0, a_geometry.v_width, a_geometry.v_height);
	}
#endif
	using t_widget::f_invalidate;
	using t_widget::f_scroll;
	const t_extent& f_unit() const
	{
		return v_unit;
	}
	unsigned f_content() const
	{
		return v_buffer.f_log_size() * v_unit.v_height + f_geometry().v_height;
	}
	int f_position()
	{
		return v_position;
	}
	void f_position__(int a_position);
	void f_viewable(int a_y, unsigned a_height)
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
};

struct t_colors
{
	t_color v_base;
	t_color v_lighter0;
	t_color v_lighter1;
	t_color v_lighter2;
	t_color v_lighter3;
	t_color v_darker0;
	t_color v_darker1;
	t_color v_darker2;
	t_color v_darker3;
	t_pixel v_lighters[4];
	t_pixel v_darkers[4];

	t_colors(unsigned char a_red, unsigned char a_green, unsigned char a_blue);
};

class t_pane : public t_frame
{
	enum t_part
	{
		e_part__NONE,
		e_part__BUTTON_UP,
		e_part__BUTTON_DOWN,
		e_part__GAP_UP,
		e_part__GAP_DOWN,
		e_part__THUMB
	};

	static constexpr unsigned f_border_width()
	{
		return 4;
	}
	static constexpr unsigned f_bar_width()
	{
		return 12;
	}

	t_pointer<t_content> v_content;
	t_colors v_face_active{0, 47, 159};
	t_colors v_face_inactive{31, 63, 143};
	t_colors v_gap{79, 79, 79};
	t_color v_text_active{223, 223, 223};
	t_color v_text_inactive{127, 127, 127};
	t_part v_grabbing_part = e_part__NONE;
	int v_grabbing_offset;

protected:
	virtual void f_on_move();
	virtual void f_on_paint(t_graphics& a_g);
	virtual void f_on_focus_enter();
	virtual void f_on_button_press(t_modifier a_modifier, t_button a_button, int a_x, int a_y);
	virtual void f_on_button_release(t_modifier a_modifier, t_button a_button, int a_x, int a_y);
	virtual void f_on_pointer_move(t_modifier a_modifier, int a_x, int a_y);
	virtual void f_on_activate();
	virtual void f_on_deactivate();
	virtual void f_on_close();

public:
	t_pane(const t_pointer<t_content>& a_content) : v_content(a_content)
	{
		f_add(v_content);
		t_rectangle geometry = v_content->f_geometry();
		geometry.v_width += f_bar_width();
		geometry.v_height += f_border_width() * 2;
		f_move(geometry);
	}
	void f_hints(const char* a_name)
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
	void f_invalidate_bar()
	{
		const t_extent& extent = f_geometry();
		f_invalidate(extent.v_width - f_bar_width(), 0, f_bar_width(), extent.v_height);
	}
};

#endif
