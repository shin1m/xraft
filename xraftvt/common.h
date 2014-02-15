#ifndef COMMON_H
#define COMMON_H

#include <climits>
#include <xraft/window.h>
#include <xraft/graphics.h>

using namespace xraft;

struct t_attribute
{
	unsigned char v_attribute;
	unsigned char v_color;

	t_attribute()
	{
	}
	t_attribute(bool a_bold, bool a_underline, bool a_blink, bool a_reverse, int a_foreground, int a_background) :
	v_attribute((a_bold ? 1 : 0) | (a_underline ? 2 : 0) | (a_blink ? 4 : 0) | (a_reverse ? 8 : 0)), v_color((a_foreground << 4) | (a_background))
	{
	}
	bool operator==(const t_attribute& a_x) const
	{
		return v_attribute == a_x.v_attribute && v_color == a_x.v_color;
	}
	bool f_bold() const
	{
		return (v_attribute & 1) != 0;
	}
	void f_bold__(bool a_bold)
	{
		v_attribute = v_attribute & 0xfe | (a_bold ? 1 : 0);
	}
	bool f_underline() const
	{
		return (v_attribute & 2) != 0;
	}
	void f_underline__(bool a_underline)
	{
		v_attribute = v_attribute & 0xfd | (a_underline ? 2 : 0);
	}
	bool f_blink() const
	{
		return (v_attribute & 4) != 0;
	}
	void f_blink__(bool a_blink)
	{
		v_attribute = v_attribute & 0xfb | (a_blink ? 4 : 0);
	}
	bool f_reverse() const
	{
		return (v_attribute & 8) != 0;
	}
	void f_reverse__(bool a_reverse)
	{
		v_attribute = v_attribute & 0xf7 | (a_reverse ? 8 : 0);
	}
	int f_foreground() const
	{
		return v_color >> 4;
	}
	void f_foreground__(int a_foreground)
	{
		v_color = v_color & 0x0f | (a_foreground << 4);
	}
	int f_background() const
	{
		return v_color & 0x0f;
	}
	void f_background__(int a_background)
	{
		v_color = v_color & 0xf0 | a_background;
	}
};

struct t_cell
{
	wchar_t v_c;
	::t_attribute v_a;

	t_cell()
	{
	}
	t_cell(wchar_t a_c, ::t_attribute a_a) : v_c(a_c), v_a(a_a)
	{
	}
};

struct t_row
{
	unsigned v_size;
	bool v_wrapped;
	t_cell v_cells[1];

	t_row() : v_size(0), v_wrapped(false)
	{
	}
	t_row(unsigned a_size, const t_cell* a_cells);
	void f_split(unsigned a_x);
	void f_expand(unsigned a_size);
	void f_insert(unsigned a_x, unsigned a_n, unsigned a_width);
};

class t_buffer
{
	unsigned v_log_capacity;
	unsigned v_width;
	unsigned v_height;
	char* v_buffer;
	t_row** v_log;
	unsigned v_log_begin;
	unsigned v_log_size;
	t_row** v_normal;
	t_row** v_alternate;
	t_row** v_current;

public:
	t_buffer(unsigned a_log, unsigned a_width, unsigned a_height);
	~t_buffer();
	unsigned f_log_capacity() const
	{
		return v_log_capacity;
	}
	unsigned f_width() const
	{
		return v_width;
	}
	unsigned f_height() const
	{
		return v_height;
	}
	unsigned f_log_size() const
	{
		return v_log_size;
	}
	const t_row* f_log(unsigned a_y) const
	{
		return v_log[(v_log_begin + a_y) % v_log_capacity];
	}
	bool f_alternate() const
	{
		return v_current == v_alternate;
	}
	void f_alternate__(bool a_alternate)
	{
		v_current = a_alternate ? v_alternate : v_normal;
	}
	const t_row* f_at(unsigned a_y)
	{
		return v_current[a_y];
	}
	void f_resize(unsigned a_width, unsigned a_height);
	void f_scroll_log();
	void f_scroll_up(unsigned a_y, unsigned a_height, unsigned a_n);
	void f_scroll_down(unsigned a_y, unsigned a_height, unsigned a_n);
	void f_erase(unsigned a_y, unsigned a_height);
	void f_insert(unsigned a_x, unsigned a_y, unsigned a_n);
	void f_put(unsigned a_x, unsigned a_y, const t_cell& a_cell, bool a_shift);
	void f_erase(unsigned a_x, unsigned a_y, unsigned a_n, bool a_shift);
};

class t_content : public t_widget
{
	t_pixel v_pixels[32];
	t_pointer<t_font> v_font;
	t_extent v_unit;
#ifdef XRAFT_TRANSPARENT
	Pixmap v_pixmap;
	bool v_moved;
	t_point v_origin;
#endif
	t_buffer v_buffer;
	int v_position;
	wchar_t* v_cs;

	void f_draw_row(t_graphics& a_g, int a_y, const t_row* a_row);
	void f_draw_cursor(t_graphics& a_g, int a_x, int a_y, const t_cell& a_cell);
	void f_invalidate(int a_y, unsigned a_height);
	void f_scroll(int a_y, unsigned a_height, int a_dy);

protected:
	::t_attribute v_attribute;
	unsigned v_cursor_x;
	unsigned v_cursor_y;

	virtual ~t_content();
	virtual void f_on_move();
	virtual void f_on_paint(t_graphics& a_g);
	virtual void f_on_focus_enter();
	virtual void f_on_focus_leave();
	virtual t_rectangle f_on_input_spot();
	unsigned f_width() const
	{
		return v_buffer.f_width();
	}
	unsigned f_height() const
	{
		return v_buffer.f_height();
	}
	bool f_alternate() const
	{
		return v_buffer.f_alternate();
	}
	void f_alternate__(bool a_alternate)
	{
		v_buffer.f_alternate__(a_alternate);
		f_invalidate(0, f_height());
	}
	void f_scroll_log();
	void f_scroll_up(unsigned a_y, unsigned a_height, unsigned a_n);
	void f_scroll_down(unsigned a_y, unsigned a_height, unsigned a_n);
	void f_erase(unsigned a_y, unsigned a_height);
	void f_insert(unsigned a_x, unsigned a_y, unsigned a_n);
	void f_put(unsigned a_x, unsigned a_y, wchar_t a_c, bool a_shift);
	void f_erase(unsigned a_x, unsigned a_y, unsigned a_n, bool a_shift);
	void f_invalidate_cursor()
	{
		f_invalidate(v_cursor_y, 1);
	}

public:
	t_content(unsigned a_log, unsigned a_width, unsigned a_height);
#ifdef XRAFT_TRANSPARENT
	void f_move(const t_rectangle& a_geometry);
#endif
	using t_widget::f_invalidate;
	using t_widget::f_scroll;
	const t_extent& f_unit() const
	{
		return v_unit;
	}
	unsigned f_content() const;
	int f_position()
	{
		return v_position;
	}
	void f_position__(int a_position);
	void f_viewable(int a_y, unsigned a_height);
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
	enum t_part {
		e_part__NONE,
		e_part__BUTTON_UP,
		e_part__BUTTON_DOWN,
		e_part__GAP_UP,
		e_part__GAP_DOWN,
		e_part__THUMB
	};

	static unsigned f_border_width()
	{
		return 4;
	}
	static unsigned f_bar_width()
	{
		return 12;
	}

	t_pointer<t_content> v_content;
	t_colors v_face_active;
	t_colors v_face_inactive;
	t_colors v_gap;
	t_color v_text_active;
	t_color v_text_inactive;
	t_part v_grabbing_part;
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
	t_pane(const t_pointer<t_content>& a_content);
	void f_hints(const char* a_name);
	void f_invalidate_bar();
};

class t_terminal : public t_content, protected t_runnable
{
	enum t_code
	{
		e_code__TAB, e_code__BACK_SPACE,
		e_code__FIND, e_code__INSERT, e_code__EXECUTE, e_code__SELECT,
		e_code__PRIOR, e_code__NEXT, e_code__HOME, e_code__END,
		e_code__DELETE,
		e_code__F1, e_code__F2, e_code__F3, e_code__F4,
		e_code__F5, e_code__F6, e_code__F7, e_code__F8,
		e_code__F9, e_code__F10, e_code__F11, e_code__F12,
		e_code__F13, e_code__F14, e_code__F15, e_code__F16,
		e_code__F17, e_code__F18, e_code__F19, e_code__F20,
		e_code__UP, e_code__DOWN, e_code__RIGHT, e_code__LEFT,
		e_code__KP_ENTER,
		e_code__KP_F1, e_code__KP_F2, e_code__KP_F3, e_code__KP_F4,
		e_code__KP_MULTIPLY, e_code__KP_ADD, e_code__KP_SEPARATOR,
		e_code__KP_SUBTRACT, e_code__KP_DECIMAL, e_code__KP_DIVIDE,
		e_code__KP_0, e_code__KP_1, e_code__KP_2, e_code__KP_3,
		e_code__KP_4, e_code__KP_5, e_code__KP_6, e_code__KP_7,
		e_code__KP_8, e_code__KP_9,
		e_code__NONE
	};
	enum t_csi
	{
		e_csi__PRIMARY, e_csi__SECONDARY, e_csi__PRIVATE
	};

	typedef void (t_terminal::*t_state)(wchar_t a_c);

	static const char* v_codes[e_code__NONE][4];

	static t_code f_code(t_key a_key);

	int v_master;
	char v_mbs[MB_LEN_MAX * 256];
	size_t v_mbn;
	std::mbstate_t v_mbstate;
	bool v_mode_insert;
	bool v_mode_origin;
	bool v_mode_wraparound;
	bool v_mode_application_keypad;
	bool v_mode_application_cursor;
	::t_attribute v_saved_attribute;
	unsigned v_saved_cursor_x;
	unsigned v_saved_cursor_y;
	unsigned char* v_tab_stops;
	unsigned v_region_begin;
	unsigned v_region_size;
	t_state v_state;
	t_csi v_csi;
	int v_parameters[16];
	int v_parameters_size;

	void f_scroll();
	void f_back_space();
	void f_tab_stop_forward(unsigned a_n);
	void f_tab_set();
	void f_tab_clear();
	void f_line_feed();
	void f_carriage_return();
	void f_save_cursor();
	void f_restore_cursor();
	void f_index();
	void f_reverse_index();
	void f_next_line();
	void f_cursor_up();
	void f_cursor_down();
	void f_cursor_forward();
	void f_cursor_backward();
	void f_cursor_next_line();
	void f_cursor_preceding_line();
	void f_cursor_character_absolute();
	void f_cursor_position();
	void f_erase_display();
	void f_erase_line();
	void f_erase_character();
	void f_insert_line();
	void f_delete_line();
	void f_insert_character();
	void f_delete_character();
	void f_attribute();
	void f_mode(bool a_mode);
	void f_scroll_region();
	void f_put(wchar_t a_c);
	void f_control_character(wchar_t a_c);
	void f_control_sequence(wchar_t a_c);
	void f_state_default(wchar_t a_c);
	void f_state_escape(wchar_t a_c);
	void f_state_escape_sharp(wchar_t a_c);
	void f_state_csi(wchar_t a_c);
	void f_state_csi_parameter(wchar_t a_c);
	void f_state_csi_parameter_digit(wchar_t a_c);
	void f_send(const char* a_cs, size_t a_n);
	void f_send(const wchar_t* a_cs, size_t a_n);
	bool f_send(t_modifier a_modifier, t_key a_key);

protected:
	virtual ~t_terminal();
	virtual void f_on_move();
	virtual void f_on_key_press(t_modifier a_modifier, t_key a_key, char a_ascii);
	virtual void f_on_input_commit(const wchar_t* a_cs, size_t a_n);
	virtual void operator()();

public:
	t_terminal(unsigned a_log, unsigned a_width, unsigned a_height, int a_master);
	int f_master() const
	{
		return v_master;
	}
};

#endif
