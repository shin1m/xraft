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

	enum class t_csi
	{
		e_PRIMARY, e_SECONDARY, e_PRIVATE
	};

	using t_state = void (t_terminal::*)(wchar_t a_c);

	bool v_mode_insert = false;
	bool v_mode_origin = false;
	bool v_mode_wraparound = true;
	bool v_mode_application_cursor = false;
	bool v_mode_application_keypad = false;
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
			case t_csi::e_PRIMARY:
				switch (v_parameters[i]) {
				case 4:
					v_mode_insert = a_mode;
					break;
				default:
std::fprintf(stderr, "%s unknown mode: %d\n", a_mode ? "Set" : "Reset", v_parameters[i]);
				}
				break;
			case t_csi::e_SECONDARY:
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
	void f_device_status_report()
	{
		int p = f_parameter(0, 0);
		switch (p) {
		case 5:
			v_host.f_send("\x1b[0n", 4);
			break;
		case 6:
			{
				char cs[25];
				v_host.f_send(cs, std::snprintf(cs, sizeof(cs), "\x1b[%d;%dR", v_cursor_y + 1, v_cursor_x + 1));
			}
			break;
		default:
std::fprintf(stderr, "Device Status Report unknown parameter: %d\n", p);
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
		case L'n':
			f_device_status_report();
			break;
		case L'r':
			f_scroll_region();
			break;
		default:
std::fprintf(stderr, "Unknown control sequence:");
switch (v_csi) {
case t_csi::e_SECONDARY:
	std::fprintf(stderr, " >");
	break;
case t_csi::e_PRIVATE:
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
			v_csi = t_csi::e_PRIMARY;
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
				v_csi = t_csi::e_SECONDARY;
				break;
			case L'?':
				v_csi = t_csi::e_PRIVATE;
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
	enum class t_code
	{
		e_BACK_SPACE, e_TAB,
		e_F1, e_F2, e_F3, e_F4,
		e_F5, e_F6, e_F7, e_F8,
		e_F9, e_F10, e_F11, e_F12,
		e_F13, e_F14, e_F15, e_F16,
		e_F17, e_F18, e_F19, e_F20,
		e_F21, e_F22, e_F23, e_F24,
		e_DELETE,
		e_HOME, e_LEFT, e_UP, e_RIGHT, e_DOWN,
		e_PRIOR, e_NEXT, e_END, e_BEGIN,
		e_INSERT,
		e_KP_SPACE, e_KP_TAB, e_KP_ENTER,
		e_KP_F1, e_KP_F2, e_KP_F3, e_KP_F4,
		e_KP_HOME, e_KP_LEFT, e_KP_UP, e_KP_RIGHT, e_KP_DOWN,
		e_KP_PRIOR, e_KP_NEXT, e_KP_END, e_KP_BEGIN,
		e_KP_INSERT, e_KP_DELETE, e_KP_EQUAL,
		e_KP_MULTIPLY, e_KP_ADD, e_KP_SEPARATOR,
		e_KP_SUBTRACT, e_KP_DECIMAL, e_KP_DIVIDE,
		e_KP_0, e_KP_1, e_KP_2, e_KP_3, e_KP_4,
		e_KP_5, e_KP_6, e_KP_7, e_KP_8, e_KP_9,
		e_NONE
	};

private:
	static const char* v_codes[static_cast<size_t>(t_code::e_NONE)][6];

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
		auto p = v_codes[static_cast<size_t>(a_code)];
		if (a_shift == 0) {
			if (v_mode_application_cursor && p[4]) return p[4];
			if (v_mode_application_keypad && p[5]) return p[5];
		}
		return p[a_shift];
	}
	void operator()(wchar_t a_c)
	{
		(this->*v_state)(a_c);
	}
};

template<typename T_host>
const char* t_terminal<T_host>::v_codes[][6] = {
	// Normal, Shift, Control, Control + Shift
	{"\b", "\b", "\x7f", "\x7f", 0, 0}, // e_BACK_SPACE
	{"\t", "\x1b[Z", "\t", "\x1b[Z", 0, 0}, // e_TAB
	{"\x1bOP", "\x1b[1;2P", "\x1b[1;5P", "\x1b[1;6P", 0, 0}, // e_F1
	{"\x1bOQ", "\x1b[1;2Q", "\x1b[1;5Q", "\x1b[1;6Q", 0, 0}, // e_F2
	{"\x1bOR", "\x1b[1;2R", "\x1b[1;5R", "\x1b[1;6R", 0, 0}, // e_F3
	{"\x1bOS", "\x1b[1;2S", "\x1b[1;5S", "\x1b[1;6S", 0, 0}, // e_F4
	{"\x1b[15~", "\x1b[15;2~", "\x1b[15;5~", "\x1b[15;6~", 0, 0}, // e_F5
	{"\x1b[17~", "\x1b[17;2~", "\x1b[17;5~", "\x1b[17;6~", 0, 0}, // e_F6
	{"\x1b[18~", "\x1b[18;2~", "\x1b[18;5~", "\x1b[18;6~", 0, 0}, // e_F7
	{"\x1b[19~", "\x1b[19;2~", "\x1b[19;5~", "\x1b[19;6~", 0, 0}, // e_F8
	{"\x1b[20~", "\x1b[20;2~", "\x1b[20;5~", "\x1b[20;6~", 0, 0}, // e_F9
	{"\x1b[21~", "\x1b[21;2~", "\x1b[21;5~", "\x1b[21;6~", 0, 0}, // e_F10
	{"\x1b[23~", "\x1b[23;2~", "\x1b[23;5~", "\x1b[23;6~", 0, 0}, // e_F11
	{"\x1b[24~", "\x1b[24;2~", "\x1b[24;5~", "\x1b[24;6~", 0, 0}, // e_F12
	{"\x1b[1;2P", "\x1b[1;5P", "\x1b[1;6P", "\x1b[1;3P", 0, 0}, // e_F13
	{"\x1b[1;2Q", "\x1b[1;5Q", "\x1b[1;6Q", "\x1b[1;3Q", 0, 0}, // e_F14
	{"\x1b[1;2R", "\x1b[1;5R", "\x1b[1;6R", "\x1b[1;3R", 0, 0}, // e_F15
	{"\x1b[1;2S", "\x1b[1;5S", "\x1b[1;6S", "\x1b[1;3S", 0, 0}, // e_F16
	{"\x1b[15;2~", "\x1b[15;5~", "\x1b[15;6~", "\x1b[15;3~", 0, 0}, // e_F17
	{"\x1b[17;2~", "\x1b[17;5~", "\x1b[17;6~", "\x1b[17;3~", 0, 0}, // e_F18
	{"\x1b[18;2~", "\x1b[18;5~", "\x1b[18;6~", "\x1b[18;3~", 0, 0}, // e_F19
	{"\x1b[19;2~", "\x1b[19;5~", "\x1b[19;6~", "\x1b[19;3~", 0, 0}, // e_F20
	{"\x1b[20;2~", "\x1b[20;5~", "\x1b[20;6~", "\x1b[20;3~", 0, 0}, // e_F21
	{"\x1b[21;2~", "\x1b[21;5~", "\x1b[21;6~", "\x1b[21;3~", 0, 0}, // e_F22
	{"\x1b[23;2~", "\x1b[23;5~", "\x1b[23;6~", "\x1b[23;3~", 0, 0}, // e_F23
	{"\x1b[24;2~", "\x1b[24;5~", "\x1b[24;6~", "\x1b[24;3~", 0, 0}, // e_F24
	{"\x1b[3~", "\x1b[3;2~", "\x1b[3;5~", "\x1b[3;6~", 0, 0}, // e_DELETE
	{"\x1b[H", "\x1b[1;2H", "\x1b[1;5H", "\x1b[1;6H", "\x1bOH", 0}, // e_HOME
	{"\x1b[D", "\x1b[1;2D", "\x1b[1;5D", "\x1b[1;6D", "\x1bOD", 0}, // e_LEFT
	{"\x1b[A", "\x1b[1;2A", "\x1b[1;5A", "\x1b[1;6A", "\x1bOA", 0}, // e_UP
	{"\x1b[C", "\x1b[1;2C", "\x1b[1;5C", "\x1b[1;6C", "\x1bOC", 0}, // e_RIGHT
	{"\x1b[B", "\x1b[1;2B", "\x1b[1;5B", "\x1b[1;6B", "\x1bOB", 0}, // e_DOWN
	{"\x1b[5~", "\x1b[5;2~", "\x1b[5;5~", "\x1b[5;6~", 0, 0}, // e_PRIOR
	{"\x1b[6~", "\x1b[6;2~", "\x1b[6;5~", "\x1b[6;6~", 0, 0}, // e_NEXT
	{"\x1b[F", "\x1b[1;2F", "\x1b[1;5F", "\x1b[1;6F", "\x1bOF", 0}, // e_END
	{"\x1b[E", "\x1b[1;2E", "\x1b[1;5E", "\x1b[1;6E", 0, 0}, // e_BEGIN
	{"\x1b[2~", "\x1b[2;2~", "\x1b[2;5~", "\x1b[2;6~", 0, 0}, // e_INSERT
	{" ", "\x1b[1;2 ", "\x1b[1;5 ", "\x1b[1;6 ", 0, "\x1bO "}, // e_KP_SPACE
	{"\t", "\x1b[1;2I", "\x1b[1;5I", "\x1b[1;6I", 0, "\x1bOI"}, // e_KP_TAB
	{"\r", "\x1b[1;2M", "\x1b[1;5M", "\x1b[1;6M", 0, "\x1bOM"}, // e_KP_ENTER
	{"\x1bOP", "\x1b[1;2P", "\x1b[1;5P", "\x1b[1;6P", 0, "\x1bOP"}, // KP_F1
	{"\x1bOQ", "\x1b[1;2Q", "\x1b[1;5Q", "\x1b[1;6Q", 0, "\x1bOQ"}, // KP_F2
	{"\x1bOR", "\x1b[1;2R", "\x1b[1;5R", "\x1b[1;6R", 0, "\x1bOR"}, // KP_F3
	{"\x1bOS", "\x1b[1;2S", "\x1b[1;5S", "\x1b[1;6S", 0, "\x1bOS"}, // KP_F4
	{"\x1bOH", "\x1b[1;2H", "\x1b[1;5H", "\x1b[1;6H", 0, "\x1bOH"}, // e_KP_HOME
	{"\x1b[D", "\x1b[1;2D", "\x1b[1;5D", "\x1b[1;6D", 0, "\x1b[D"}, // e_KP_LEFT
	{"\x1b[A", "\x1b[1;2A", "\x1b[1;5A", "\x1b[1;6A", 0, "\x1b[A"}, // e_KP_UP
	{"\x1b[C", "\x1b[1;2C", "\x1b[1;5C", "\x1b[1;6C", 0, "\x1b[C"}, // e_KP_RIGHT
	{"\x1b[B", "\x1b[1;2B", "\x1b[1;5B", "\x1b[1;6B", 0, "\x1b[B"}, // e_KP_DOWN
	{"\x1b[5~", "\x1b[5;2~", "\x1b[5;5~", "\x1b[5;6~", 0, "\x1b[5~"}, // e_KP_PRIOR
	{"\x1b[6~", "\x1b[6;2~", "\x1b[6;5~", "\x1b[6;6~", 0, "\x1b[6~"}, // e_KP_NEXT
	{"\x1bOF", "\x1b[1;2F", "\x1b[1;5F", "\x1b[1;6F", 0, "\x1bOF"}, // e_KP_END
	{"\x1b[E", "\x1b[1;2E", "\x1b[1;5E", "\x1b[1;6E", 0, "\x1b[E"}, // e_KP_BEGIN
	{"\x1b[2~", "\x1b[2;2~", "\x1b[2;5~", "\x1b[2;6~", 0, "\x1b[2~"}, // e_KP_INSERT
	{"\x1b[3~", "\x1b[3;2~", "\x1b[3;5~", "\x1b[3;6~", 0, "\x1b[3~"}, // e_KP_DELETE
	{"~", "\x1b[1;2X", "\x1b[1;5X", "\x1b[1;6X", 0, "\x1bOX"}, // e_KP_EQUAL
	{"*", "\x1b[1;2j", "\x1b[1;5j", "\x1b[1;6j", 0, "\x1bOj"}, // e_KP_MULTIPLY
	{"+", "\x1b[1;2k", "\x1b[1;5k", "\x1b[1;6k", 0, "\x1bOk"}, // e_KP_ADD
	{",", "\x1b[1;2l", "\x1b[1;5l", "\x1b[1;6l", 0, "\x1bOl"}, // e_KP_SEPARATOR
	{"-", "\x1b[1;2m", "\x1b[1;5m", "\x1b[1;6m", 0, "\x1bOm"}, // e_KP_SUBTRACT
	{".", "\x1b[3;2~", "\x1b[3;5~", "\x1b[3;6~", 0, "\x1b[3~"}, // e_KP_DECIMAL
	{"/", "\x1b[1;2o", "\x1b[1;5o", "\x1b[1;6o", 0, "\x1bOo"}, // e_KP_DIVIDE
	{"0", "\x1b[2;2~", "\x1b[2;5~", "\x1b[2;6~", 0, "\x1b[2~"}, // e_KP_0
	{"1", "\x1b[1;2F", "\x1b[1;5F", "\x1b[1;6F", 0, "\x1bOF"}, // e_KP_1
	{"2", "\x1b[1;2B", "\x1b[1;5B", "\x1b[1;6B", 0, "\x1b[B"}, // e_KP_2
	{"3", "\x1b[6;2~", "\x1b[6;5~", "\x1b[6;6~", 0, "\x1b[6~"}, // e_KP_3
	{"4", "\x1b[1;2D", "\x1b[1;5D", "\x1b[1;6D", 0, "\x1b[D"}, // e_KP_4
	{"5", "\x1b[1;2E", "\x1b[1;5E", "\x1b[1;6E", 0, "\x1b[E"}, // e_KP_5
	{"6", "\x1b[1;2C", "\x1b[1;5C", "\x1b[1;6C", 0, "\x1b[C"}, // e_KP_6
	{"7", "\x1b[1;2H", "\x1b[1;5H", "\x1b[1;6H", 0, "\x1bOH"}, // e_KP_7
	{"8", "\x1b[1;2A", "\x1b[1;5A", "\x1b[1;6A", 0, "\x1b[A"}, // e_KP_8
	{"9", "\x1b[5;2~", "\x1b[5;5~", "\x1b[5;6~", 0, "\x1b[5~"}, // e_KP_9
};

#endif
