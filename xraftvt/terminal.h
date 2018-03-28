#ifndef XRAFTVT__TERMINAL_H
#define XRAFTVT__TERMINAL_H

#include "buffer.h"
#include <cstdio>
#include <cwctype>

template<typename T_host>
class t_terminal : public t_buffer<T_host>
{
	using t_buffer<T_host>::v_host;
	using t_buffer<T_host>::v_attribute;
	using t_buffer<T_host>::v_cursor_x;
	using t_buffer<T_host>::v_cursor_y;
	using t_buffer<T_host>::f_save_cursor;
	using t_buffer<T_host>::f_scroll_down;
	using t_buffer<T_host>::f_scroll_up;
	using t_buffer<T_host>::f_erase;

	enum t_csi
	{
		e_csi__PRIMARY, e_csi__SECONDARY, e_csi__PRIVATE
	};

	typedef void (t_terminal::*t_state)(wchar_t a_c);

	bool v_mode_insert = false;
	bool v_mode_origin = false;
	bool v_mode_wraparound = true;
	bool v_mode_application_keypad = false;
	bool v_mode_application_cursor = false;
	unsigned char* v_tab_stops;
	unsigned v_region_begin = 0;
	unsigned v_region_size;
	t_state v_state = &t_terminal::f_state_default;
	t_csi v_csi;
	int v_parameters[16];
	int v_parameters_size;

	int f_parameter(size_t a_i, int a_default) const
	{
		return v_parameters_size < a_i + 1 || v_parameters[a_i] < a_default ? a_default : v_parameters[a_i];
	}
	void f_scroll()
	{
		if (this->f_alternate() || v_region_begin > 0 || v_region_size < f_height())
			f_scroll_up(v_region_begin, v_region_size, 1);
		else
			this->f_scroll_log();
	}
	void f_back_space()
	{
		if (v_cursor_x >= f_width()) v_cursor_x = f_width() - 1;
		if (v_cursor_x > 0) --v_cursor_x;
	}
	void f_tab_stop_forward(unsigned a_n)
	{
		if (a_n <= 0) return;
		do {
			do {
				if (++v_cursor_x >= f_width()) {
					v_cursor_x = f_width() - 1;
					return;
				}
			} while ((v_tab_stops[v_cursor_x / 8] & (1 << v_cursor_x % 8)) == 0);
		} while (--a_n > 0);
	}
	void f_tab_set()
	{
		if (v_cursor_x < f_width()) v_tab_stops[v_cursor_x / 8] |= 1 << v_cursor_x % 8;
	}
	void f_tab_clear()
	{
		switch (f_parameter(0, 0)) {
		case 0:
			if (v_cursor_x < f_width()) v_tab_stops[v_cursor_x / 8] &= ~(1 << v_cursor_x % 8);
			break;
		case 3:
			std::fill_n(v_tab_stops, (f_width() + 7) / 8, 0);
			break;
		}
	}
	void f_line_feed()
	{
		f_index();
	}
	void f_carriage_return()
	{
		v_cursor_x = 0;
	}
	void f_restore_cursor()
	{
		t_buffer<T_host>::f_restore_cursor();
		if (v_cursor_x >= f_width()) v_cursor_x = f_width() - 1;
		unsigned bottom = v_mode_origin ? v_region_begin + v_region_size : f_height();
		if (v_cursor_y >= bottom) v_cursor_y = bottom - 1;
		if (v_mode_origin && v_cursor_y < v_region_begin) v_cursor_y = v_region_begin;
	}
	void f_index()
	{
		if (++v_cursor_y == v_region_begin + v_region_size) {
			f_scroll();
			--v_cursor_y;
		} else if (v_cursor_y >= f_height()) {
			v_cursor_y = f_height() - 1;
		}
	}
	void f_reverse_index()
	{
		if (v_cursor_y == v_region_begin)
			f_scroll_down(v_region_begin, v_region_size, 1);
		else if (v_cursor_y > 0)
			--v_cursor_y;
	}
	void f_next_line()
	{
		v_cursor_x = 0;
		f_index();
	}
	void f_cursor_up()
	{
		int p0 = f_parameter(0, 1);
		unsigned top = v_mode_origin ? v_region_begin : 0;
		if (p0 > static_cast<int>(v_cursor_y - top))
			v_cursor_y = top;
		else
			v_cursor_y -= p0;
	}
	void f_cursor_down()
	{
		v_cursor_y += f_parameter(0, 1);
		unsigned bottom = v_mode_origin ? v_region_begin + v_region_size : f_height();
		if (v_cursor_y >= bottom) v_cursor_y = bottom - 1;
	}
	void f_cursor_forward()
	{
		v_cursor_x += f_parameter(0, 1);
		if (v_cursor_x >= f_width()) v_cursor_x = f_width() - 1;
	}
	void f_cursor_backward()
	{
		int p0 = f_parameter(0, 1);
		if (p0 > static_cast<int>(v_cursor_x))
			v_cursor_x = 0;
		else
			v_cursor_x -= p0;
	}
	void f_cursor_next_line()
	{
		f_cursor_down();
		v_cursor_x = 0;
	}
	void f_cursor_preceding_line()
	{
		f_cursor_up();
		v_cursor_x = 0;
	}
	void f_cursor_character_absolute(size_t a_i = 0)
	{
		v_cursor_x = f_parameter(a_i, 1) - 1;
		if (v_cursor_x >= f_width()) v_cursor_x = f_width() - 1;
	}
	void f_cursor_position()
	{
		f_line_position_absolute();
		f_cursor_character_absolute(1);
	}
	void f_line_position_absolute()
	{
		v_cursor_y = f_parameter(0, 1) - 1;
		if (v_mode_origin) v_cursor_y += v_region_begin;
		unsigned bottom = v_mode_origin ? v_region_begin + v_region_size : f_height();
		if (v_cursor_y >= bottom) v_cursor_y = bottom - 1;
	}
	void f_erase_display()
	{
		switch (f_parameter(0, 0)) {
		case 0:
			f_erase(v_cursor_x, v_cursor_y, f_width() - v_cursor_x, false);
			f_erase(v_cursor_y + 1, f_height() - v_cursor_y - 1);
			break;
		case 1:
			f_erase(0, v_cursor_y);
			f_erase(0, v_cursor_y, v_cursor_x + 1, false);
			break;
		case 2:
			f_erase(0, f_height());
			break;
		}
	}
	void f_erase_line()
	{
		switch (f_parameter(0, 0)) {
		case 0:
			f_erase(v_cursor_x, v_cursor_y, f_width() - v_cursor_x, false);
			break;
		case 1:
			f_erase(0, v_cursor_y, v_cursor_x + 1, false);
			break;
		case 2:
			f_erase(v_cursor_y, 1);
			break;
		}
	}
	void f_erase_character()
	{
		int p0 = f_parameter(0, 1);
		if (v_cursor_x + p0 > f_width()) p0 = f_width() - v_cursor_x;
		f_erase(v_cursor_x, v_cursor_y, p0, false);
	}
	void f_insert_line()
	{
		unsigned bottom = v_region_begin + v_region_size;
		if (v_cursor_y < v_region_begin || v_cursor_y >= bottom) return;
		f_scroll_down(v_cursor_y, bottom - v_cursor_y, f_parameter(0, 1));
		v_cursor_x = 0;
	}
	void f_delete_line()
	{
		unsigned bottom = v_region_begin + v_region_size;
		if (v_cursor_y < v_region_begin || v_cursor_y >= bottom) return;
		f_scroll_up(v_cursor_y, bottom - v_cursor_y, f_parameter(0, 1));
		v_cursor_x = 0;
	}
	void f_insert_character()
	{
		this->f_insert(v_cursor_x, v_cursor_y, f_parameter(0, 1));
	}
	void f_delete_character()
	{
		f_erase(v_cursor_x, v_cursor_y, f_parameter(0, 1), true);
	}
	void f_scroll_up()
	{
		f_scroll_up(v_region_begin, v_region_size, f_parameter(0, 1));
	}
	void f_scroll_down()
	{
		f_scroll_down(v_region_begin, v_region_size, f_parameter(0, 1));
	}
	void f_attribute()
	{
		if (v_parameters_size < 1) {
			v_parameters[0] = -1;
			v_parameters_size = 1;
		}
		for (int i = 0; i < v_parameters_size; ++i) {
			switch (v_parameters[i]) {
			case -1:
			case 0:
				v_attribute = {false, false, false, false, false, 0, 1};
				break;
			case 1:
				v_attribute.v_bold = true;
				break;
			case 2:
				v_attribute.v_faint = true;
				break;
			case 4:
				v_attribute.v_underlined = true;
				break;
			case 5:
				v_attribute.v_blink = true;
				break;
			case 7:
				v_attribute.v_inverse = true;
				break;
			case 22:
				v_attribute.v_bold = false;
				v_attribute.v_faint = false;
				break;
			case 24:
				v_attribute.v_underlined = false;
				break;
			case 25:
				v_attribute.v_blink = false;
				break;
			case 27:
				v_attribute.v_inverse = false;
				break;
			case 30:
			case 31:
			case 32:
			case 33:
			case 34:
			case 35:
			case 36:
			case 37:
				v_attribute.v_foreground = v_parameters[i] - 28;
				break;
			case 39:
				v_attribute.v_foreground = 0;
				break;
			case 40:
			case 41:
			case 42:
			case 43:
			case 44:
			case 45:
			case 46:
			case 47:
				v_attribute.v_background = v_parameters[i] - 38;
				break;
			case 49:
				v_attribute.v_background = 1;
				break;
			default:
std::fprintf(stderr, "Unknown attribute: %d\n", v_parameters[i]);
			}
		}
	}
	void f_mode(bool a_mode)
	{
		for (int i = 0; i < v_parameters_size; ++i) {
			switch (v_csi) {
			case e_csi__PRIMARY:
				switch (v_parameters[i]) {
				case 4:
					v_mode_insert = a_mode;
					break;
				default:
std::fprintf(stderr, "%s unknown mode: %d\n", a_mode ? "Set" : "Reset", v_parameters[i]);
				}
				break;
			case e_csi__SECONDARY:
std::fprintf(stderr, "%s unknown secondary mode: %d\n", a_mode ? "Set" : "Reset", v_parameters[i]);
				break;
			default:
				switch (v_parameters[i]) {
				case 1:
					v_mode_application_cursor = a_mode;
					break;
				case 3:
					v_region_begin = 0;
					v_region_size = f_height();
					v_cursor_x = v_cursor_y = 0;
					f_erase(0, f_height());
					break;
				case 6:
					v_mode_origin = a_mode;
					v_cursor_x = 0;
					v_cursor_y = a_mode ? v_region_begin : 0;
					break;
				case 7:
					v_mode_wraparound = a_mode;
					break;
				case 1049:
					if (a_mode) {
						this->f_save_cursor();
						this->f_alternate__(true);
						f_erase(0, f_height());
						v_cursor_x = v_cursor_y = 0;
					} else {
						this->f_alternate__(false);
						f_restore_cursor();
					}
					v_region_begin = 0;
					v_region_size = f_height();
					break;
				default:
std::fprintf(stderr, "%s unknown private mode: %d\n", a_mode ? "Set" : "Reset", v_parameters[i]);
				}
			}
		}
	}
	void f_scroll_region()
	{
		int h = f_height();
		int p0 = std::min(f_parameter(0, 1) - 1, h - 2);
		int p1 = f_parameter(1, -1);
		p1 = std::max(p1 < 0 ? h : p1, p0 + 2);
		v_region_begin = p0;
		v_region_size = p1 - p0;
		v_cursor_x = 0;
		v_cursor_y = v_mode_origin ? v_region_begin : 0;
	}
	void f_put(wchar_t a_c)
	{
		if (v_cursor_x >= f_width()) {
			if (v_mode_wraparound) {
				f_index();
				v_cursor_x = 0;
			} else {
				v_cursor_x = f_width() - 1;
			}
		}
		t_buffer<T_host>::f_put(v_cursor_x, v_cursor_y, {a_c, v_attribute}, v_mode_insert);
		v_cursor_x += v_host.f_width(a_c);
	}
	void f_control_character(wchar_t a_c)
	{
		switch (a_c) {
		case L'G' - L'@':
			v_host.f_bell();
			break;
		case L'H' - L'@':
			f_back_space();
			break;
		case L'I' - L'@':
			f_tab_stop_forward(1);
			break;
		case L'J' - L'@':
		case L'K' - L'@':
		case L'L' - L'@':
			f_line_feed();
			break;
		case L'M' - L'@':
			f_carriage_return();
			break;
		case L'[' - L'@':
			v_state = &t_terminal::f_state_escape;
			break;
		default:
std::fprintf(stderr, "Unknown control character: %x\n", a_c);
		}
	}
	void f_control_sequence(wchar_t a_c)
	{
		switch (a_c) {
		case L'@':
			f_insert_character();
			break;
		case L'A':
			f_cursor_up();
			break;
		case L'B':
			f_cursor_down();
			break;
		case L'C':
			f_cursor_forward();
			break;
		case L'D':
			f_cursor_backward();
			break;
		case L'E':
			f_cursor_next_line();
			break;
		case L'F':
			f_cursor_preceding_line();
			break;
		case L'G':
			f_cursor_character_absolute();
			break;
		case L'H':
			f_cursor_position();
			break;
		case L'J':
			f_erase_display();
			break;
		case L'K':
			f_erase_line();
			break;
		case L'L':
			f_insert_line();
			break;
		case L'M':
			f_delete_line();
			break;
		case L'P':
			f_delete_character();
			break;
		case L'S':
			f_scroll_up();
			break;
		case L'T':
			f_scroll_down();
			break;
		case L'X':
			f_erase_character();
			break;
		case L'd':
			f_line_position_absolute();
			break;
		case L'f':
			f_cursor_position();
			break;
		case L'g':
			f_tab_clear();
			break;
		case L'h':
			f_mode(true);
			break;
		case L'l':
			f_mode(false);
			break;
		case L'm':
			f_attribute();
			break;
		case L'r':
			f_scroll_region();
			break;
		default:
std::fprintf(stderr, "Unknown control sequence:");
switch (v_csi) {
case e_csi__SECONDARY:
	std::fprintf(stderr, " >");
	break;
case e_csi__PRIVATE:
	std::fprintf(stderr, " ?");
	break;
}
for (size_t i = 0; i < v_parameters_size; ++i) std::fprintf(stderr, " %d", v_parameters[i]);
std::fprintf(stderr, " %c\n", a_c);
		}
		v_state = &t_terminal::f_state_default;
	}
	void f_state_default(wchar_t a_c)
	{
		if (std::iswcntrl(a_c))
			f_control_character(a_c);
		else
			f_put(a_c);
	}
	void f_state_escape(wchar_t a_c)
	{
		if (std::iswcntrl(a_c)) {
			f_control_character(a_c);
			return;
		}
		switch (a_c) {
		case L'#':
			v_state = &t_terminal::f_state_escape_sharp;
			return;
		case L'(':
			v_state = &t_terminal::f_state_designate_g0;
			return;
		case L'7':
			this->f_save_cursor();
			break;
		case L'8':
			f_restore_cursor();
			break;
		case L'=':
			v_mode_application_keypad = true;
			break;
		case L'>':
			v_mode_application_keypad = false;
			break;
		case L'D':
			f_index();
			break;
		case L'E':
			f_next_line();
			break;
		case L'H':
			f_tab_set();
			break;
		case L'M':
			f_reverse_index();
			break;
		case L'[':
			v_csi = e_csi__PRIMARY;
			v_parameters_size = 0;
			v_state = &t_terminal::f_state_csi;
			return;
		default:
std::fprintf(stderr, "Unknown escape sequence: %c\n", a_c);
		}
		v_state = &t_terminal::f_state_default;
	}
	void f_state_escape_sharp(wchar_t a_c)
	{
		if (std::iswcntrl(a_c)) {
			f_control_character(a_c);
			return;
		}
		switch (a_c) {
		case L'8':
			for (unsigned i = 0; i < f_height(); ++i)
				for (unsigned j = 0; j < f_width(); ++j)
					t_buffer<T_host>::f_put(j, i, {L'E', v_attribute}, false);
			break;
		default:
std::fprintf(stderr, "Unknown escape sequence: # %c\n", a_c);
		}
		v_state = &t_terminal::f_state_default;
	}
	void f_state_designate_g0(wchar_t a_c)
	{
		if (std::iswcntrl(a_c)) {
			f_control_character(a_c);
			return;
		}
		switch (a_c) {
		case L'B':
			break;
		default:
std::fprintf(stderr, "Unknown G0 character set: # %c\n", a_c);
		}
		v_state = &t_terminal::f_state_default;
	}
	void f_state_csi(wchar_t a_c)
	{
		if (std::iswcntrl(a_c)) {
			f_control_character(a_c);
		} else if (std::iswdigit(a_c)) {
			v_parameters[v_parameters_size] = a_c - L'0';
			v_state = &t_terminal::f_state_csi_parameter_digit;
		} else {
			switch (a_c) {
			case L';':
				v_parameters[v_parameters_size++] = -1;
				break;
			case L'>':
				v_csi = e_csi__SECONDARY;
				break;
			case L'?':
				v_csi = e_csi__PRIVATE;
				break;
			default:
				f_control_sequence(a_c);
				return;
			}
			v_state = &t_terminal::f_state_csi_parameter;
		}
	}
	void f_state_csi_parameter(wchar_t a_c)
	{
		if (std::iswcntrl(a_c)) {
			f_control_character(a_c);
		} else if (std::iswdigit(a_c)) {
			v_parameters[v_parameters_size] = a_c - L'0';
			v_state = &t_terminal::f_state_csi_parameter_digit;
		} else if (a_c == L';') {
			v_parameters[v_parameters_size++] = -1;
		} else {
			f_control_sequence(a_c);
		}
	}
	void f_state_csi_parameter_digit(wchar_t a_c)
	{
		if (std::iswcntrl(a_c)) {
			f_control_character(a_c);
		} else if (std::iswdigit(a_c)) {
			v_parameters[v_parameters_size] *= 10;
			v_parameters[v_parameters_size] += a_c - L'0';
		} else if (a_c == L';') {
			++v_parameters_size;
			v_state = &t_terminal::f_state_csi_parameter;
		} else {
			++v_parameters_size;
			f_control_sequence(a_c);
		}
	}

public:
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

private:
	static const char* v_codes[e_code__NONE][4];

public:
	using t_buffer<T_host>::f_width;
	using t_buffer<T_host>::f_height;

	t_terminal(T_host& a_host, unsigned a_log, unsigned a_width, unsigned a_height) : t_buffer<T_host>(a_host, a_log, a_width, a_height), v_region_size(a_height)
	{
		unsigned n = (f_width() + 7) / 8;
		v_tab_stops = new unsigned char[n];
		std::fill_n(v_tab_stops, n, 1);
	}
	~t_terminal()
	{
		delete[] v_tab_stops;
	}
	void f_resize(unsigned a_width, unsigned a_height)
	{
		unsigned m = (f_width() + 7) / 8;
		t_buffer<T_host>::f_resize(a_width, a_height);
		unsigned n = (f_width() + 7) / 8;
		auto tab_stops = new unsigned char[n];
		std::fill(std::copy(v_tab_stops, v_tab_stops + std::min(m, n), tab_stops), tab_stops + n, 1);
		delete[] v_tab_stops;
		v_tab_stops = tab_stops;
		v_region_begin = 0;
		v_region_size = f_height();
	}
	const char* f_code(t_code a_code, size_t a_shift) const
	{
		if (v_mode_application_keypad && a_code >= e_code__KP_ENTER) a_shift = 3;
		if (v_mode_application_cursor && a_code >= e_code__UP && a_code <= e_code__LEFT) a_shift = 3;
		return v_codes[a_code][a_shift];
	}
	void operator()(wchar_t a_c)
	{
		(this->*v_state)(a_c);
	}
};

template<typename T_host>
const char* t_terminal<T_host>::v_codes[e_code__NONE][4] = {
	// Normal, Shift, Control, Control + Shift
	{"\t", "\x1b[Z", "\t", "\x1b[Z"}, // Tab
	{"\b", "\x7f", "\x7f", "\x7f"}, // BackSpace
	{"\x1b[1~", "\x1b[1$", "\x1b[1^", "\x1b[1@"}, // Find
	{"\x1b[2~", "\x1b[2$"/*paste*/, "\x1b[2^", "\x1b[2@"}, // Insert
	{"\x1b[3~", "\x1b[3$", "\x1b[3^", "\x1b[3@"}, // Execute
	{"\x1b[4~", "\x1b[4$", "\x1b[4^", "\x1b[4@"}, // Select
	{"\x1b[5~", "\x1b[5$"/*scroll-up*/, "\x1b[5^", "\x1b[5@"}, // Prior
	{"\x1b[6~", "\x1b[6$"/*scroll-down*/, "\x1b[6^", "\x1b[6@"}, // Next
	{"\x1b[7~", "\x1b[7$", "\x1b[7^", "\x1b[7@"}, // Home
	{"\x1b[8~", "\x1b[8$", "\x1b[8^", "\x1b[8@"}, // End
	{"\x1b[3~", "\x1b[3$", "\x1b[3^", "\x1b[3@"}, // Delete
	{"\x1b[11~", "\x1b[23~", "\x1b[11^", "\x1b[23^"}, // F1
	{"\x1b[12~", "\x1b[24~", "\x1b[12^", "\x1b[24^"}, // F2
	{"\x1b[13~", "\x1b[25~", "\x1b[13^", "\x1b[25^"}, // F3
	{"\x1b[14~", "\x1b[26~", "\x1b[14^", "\x1b[26^"}, // F4
	{"\x1b[15~", "\x1b[28~", "\x1b[15^", "\x1b[28^"}, // F5
	{"\x1b[17~", "\x1b[29~", "\x1b[17^", "\x1b[29^"}, // F6
	{"\x1b[18~", "\x1b[31~", "\x1b[18^", "\x1b[31^"}, // F7
	{"\x1b[19~", "\x1b[32~", "\x1b[19^", "\x1b[32^"}, // F8
	{"\x1b[20~", "\x1b[33~", "\x1b[20^", "\x1b[33^"}, // F9
	{"\x1b[21~", "\x1b[34~", "\x1b[21^", "\x1b[34^"}, // F10
	{"\x1b[23~", "\x1b[23$", "\x1b[23^", "\x1b[23@"}, // F11
	{"\x1b[24~", "\x1b[24$", "\x1b[24^", "\x1b[24@"}, // F12
	{"\x1b[25~", "\x1b[25$", "\x1b[25^", "\x1b[25@"}, // F13
	{"\x1b[26~", "\x1b[26$", "\x1b[26^", "\x1b[26@"}, // F14
	{"\x1b[28~", "\x1b[28$", "\x1b[28^", "\x1b[28@"}, // F15
	{"\x1b[29~", "\x1b[29$", "\x1b[29^", "\x1b[29@"}, // F16
	{"\x1b[31~", "\x1b[31$", "\x1b[31^", "\x1b[31@"}, // F17
	{"\x1b[32~", "\x1b[32$", "\x1b[32^", "\x1b[32@"}, // F18
	{"\x1b[33~", "\x1b[33$", "\x1b[33^", "\x1b[33@"}, // F19
	{"\x1b[34~", "\x1b[34$", "\x1b[34^", "\x1b[34@"}, // F20
	// Normal, Shift, Control, Application
	{"\x1b[A", "\x1b[a", "\x1bOa", "\x1bOA"}, // Up
	{"\x1b[B", "\x1b[b", "\x1bOb", "\x1bOB"}, // Down
	{"\x1b[C", "\x1b[c", "\x1bOc", "\x1bOC"}, // Right
	{"\x1b[D", "\x1b[d", "\x1bOd", "\x1bOD"}, // Left
	{"\r", 0, 0, "\x1bOM"}, // KP_Enter
	{"\x1bOP", 0, 0, "\x1bOP"}, // KP_F1
	{"\x1bOQ", 0, 0, "\x1bOQ"}, // KP_F2
	{"\x1bOR", 0, 0, "\x1bOR"}, // KP_F3
	{"\x1bOS", 0, 0, "\x1bOS"}, // KP_F4
	{"*", 0, 0, "\x1bOj"}, // KP_Multiply
	{"+", 0, 0, "\x1bOk"}, // KP_Add
	{",", 0, 0, "\x1bOl"}, // KP_Separator
	{"-", 0, 0, "\x1bOm"}, // KP_Subtract
	{".", 0, 0, "\x1bOn"}, // KP_Decimal
	{"/", 0, 0, "\x1bOo"}, // KP_Divide
	{"0", 0, 0, "\x1bOp"}, // KP_0
	{"1", 0, 0, "\x1bOq"}, // KP_1
	{"2", 0, 0, "\x1bOr"}, // KP_2
	{"3", 0, 0, "\x1bOs"}, // KP_3
	{"4", 0, 0, "\x1bOt"}, // KP_4
	{"5", 0, 0, "\x1bOu"}, // KP_5
	{"6", 0, 0, "\x1bOv"}, // KP_6
	{"7", 0, 0, "\x1bOw"}, // KP_7
	{"8", 0, 0, "\x1bOx"}, // KP_8
	{"9", 0, 0, "\x1bOy"}, // KP_9
};

#endif
