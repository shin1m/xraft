#include "common.h"

#include <cerrno>
#include <cstdio>
#include <cstring>
#include <cwctype>
#include <algorithm>
#include <sys/ioctl.h>

const char* t_terminal::v_codes[e_code__NONE][4] = {
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

t_terminal::t_code t_terminal::f_code(t_key a_key)
{
	switch (a_key) {
	case e_key__TAB:
		return e_code__TAB;
	case e_key__BACK_SPACE:
		return e_code__BACK_SPACE;
	case e_key__FIND:
		return e_code__FIND;
	case e_key__INSERT:
		return e_code__INSERT;
	case e_key__EXECUTE:
		return e_code__EXECUTE;
	case e_key__SELECT:
		return e_code__SELECT;
	case e_key__PRIOR:
		return e_code__PRIOR;
	case e_key__NEXT:
		return e_code__NEXT;
	case e_key__HOME:
		return e_code__HOME;
	case e_key__END:
		return e_code__END;
	case e_key__DELETE:
		return e_code__DELETE;
	case e_key__F1:
		return e_code__F1;
	case e_key__F2:
		return e_code__F2;
	case e_key__F3:
		return e_code__F3;
	case e_key__F4:
		return e_code__F4;
	case e_key__F5:
		return e_code__F5;
	case e_key__F6:
		return e_code__F6;
	case e_key__F7:
		return e_code__F7;
	case e_key__F8:
		return e_code__F8;
	case e_key__F9:
		return e_code__F9;
	case e_key__F10:
		return e_code__F10;
	case e_key__F11:
		return e_code__F11;
	case e_key__F12:
		return e_code__F12;
/*
	case e_key__F13:
		return e_code__F13;
	case e_key__F14:
		return e_code__F14;
	case e_key__F15:
		return e_code__F15;
	case e_key__F16:
		return e_code__F16;
	case e_key__F17:
		return e_code__F17;
	case e_key__F18:
		return e_code__F18;
	case e_key__F19:
		return e_code__F19;
	case e_key__F20:
		return e_code__F20;
*/
	case e_key__UP:
		return e_code__UP;
	case e_key__DOWN:
		return e_code__DOWN;
	case e_key__RIGHT:
		return e_code__RIGHT;
	case e_key__LEFT:
		return e_code__LEFT;
	case e_key__KP_ENTER:
		return e_code__KP_ENTER;
	case e_key__KP_F1:
		return e_code__KP_F1;
	case e_key__KP_F2:
		return e_code__KP_F2;
	case e_key__KP_F3:
		return e_code__KP_F3;
	case e_key__KP_F4:
		return e_code__KP_F4;
	case e_key__KP_MULTIPLY:
		return e_code__KP_MULTIPLY;
	case e_key__KP_ADD:
		return e_code__KP_ADD;
	case e_key__KP_SEPARATOR:
		return e_code__KP_SEPARATOR;
	case e_key__KP_SUBTRACT:
		return e_code__KP_SUBTRACT;
	case e_key__KP_DECIMAL:
		return e_code__KP_DECIMAL;
	case e_key__KP_DIVIDE:
		return e_code__KP_DIVIDE;
	case e_key__KP_0:
		return e_code__KP_0;
	case e_key__KP_1:
		return e_code__KP_1;
	case e_key__KP_2:
		return e_code__KP_2;
	case e_key__KP_3:
		return e_code__KP_3;
	case e_key__KP_4:
		return e_code__KP_4;
	case e_key__KP_5:
		return e_code__KP_5;
	case e_key__KP_6:
		return e_code__KP_6;
	case e_key__KP_7:
		return e_code__KP_7;
	case e_key__KP_8:
		return e_code__KP_8;
	case e_key__KP_9:
		return e_code__KP_9;
	default:
		return e_code__NONE;
	}
}

void t_terminal::f_scroll()
{
	if (f_alternate() || v_region_begin > 0 || v_region_size < f_height())
		f_scroll_up(v_region_begin, v_region_size, 1);
	else
		f_scroll_log();
}

void t_terminal::f_back_space()
{
	if (v_cursor_x >= f_width()) v_cursor_x = f_width() - 1;
	if (v_cursor_x > 0) --v_cursor_x;
}

void t_terminal::f_tab_stop_forward(unsigned a_n)
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

void t_terminal::f_line_feed()
{
	f_index();
}

void t_terminal::f_carriage_return()
{
	v_cursor_x = 0;
}

void t_terminal::f_save_cursor()
{
	v_saved_attribute = v_attribute;
	v_saved_cursor_x = v_cursor_x;
	v_saved_cursor_y = v_cursor_y;
}

void t_terminal::f_restore_cursor()
{
	v_attribute = v_saved_attribute;
	v_cursor_x = v_saved_cursor_x;
	v_cursor_y = v_saved_cursor_y;
	if (v_cursor_x >= f_width()) v_cursor_x = f_width() - 1;
	unsigned bottom = v_mode_origin ? v_region_begin + v_region_size : f_height();
	if (v_cursor_y >= bottom) v_cursor_y = bottom - 1;
	if (v_mode_origin && v_cursor_y < v_region_begin) v_cursor_y = v_region_begin;
}

void t_terminal::f_index()
{
	if (++v_cursor_y == v_region_begin + v_region_size) {
		f_scroll();
		--v_cursor_y;
	} else if (v_cursor_y >= f_height()) {
		v_cursor_y = f_height() - 1;
	}
}

void t_terminal::f_reverse_index()
{
	if (v_cursor_y == v_region_begin)
		f_scroll_down(v_region_begin, v_region_size, 1);
	else if (v_cursor_y > 0)
		--v_cursor_y;
}

void t_terminal::f_cursor_up()
{
	if (v_parameters_size < 1 || v_parameters[0] < 1) v_parameters[0] = 1;
	unsigned top = v_mode_origin ? v_region_begin : 0;
	if (v_parameters[0] > static_cast<int>(v_cursor_y - top))
		v_cursor_y = top;
	else
		v_cursor_y -= v_parameters[0];
}

void t_terminal::f_cursor_down()
{
	if (v_parameters_size < 1 || v_parameters[0] < 1) v_parameters[0] = 1;
	v_cursor_y += v_parameters[0];
	unsigned bottom = v_mode_origin ? v_region_begin + v_region_size : f_height();
	if (v_cursor_y >= bottom) v_cursor_y = bottom - 1;
}

void t_terminal::f_cursor_forward()
{
	if (v_parameters_size < 1 || v_parameters[0] < 1) v_parameters[0] = 1;
	v_cursor_x += v_parameters[0];
	if (v_cursor_x >= f_width()) v_cursor_x = f_width() - 1;
}

void t_terminal::f_cursor_backward()
{
	if (v_parameters_size < 1 || v_parameters[0] < 1) v_parameters[0] = 1;
	if (v_parameters[0] > static_cast<int>(v_cursor_x))
		v_cursor_x = 0;
	else
		v_cursor_x -= v_parameters[0];
}

void t_terminal::f_cursor_position()
{
	while (v_parameters_size < 2) v_parameters[v_parameters_size++] = 1;
	v_cursor_x = v_parameters[1] > 0 ? v_parameters[1] - 1 : 0;
	v_cursor_y = v_parameters[0] > 0 ? v_parameters[0] - 1 : 0;
	if (v_cursor_x >= f_width()) v_cursor_x = f_width() - 1;
	if (v_mode_origin) v_cursor_y += v_region_begin;
	unsigned bottom = v_mode_origin ? v_region_begin + v_region_size : f_height();
	if (v_cursor_y >= bottom) v_cursor_y = bottom - 1;
}

void t_terminal::f_erase_display()
{
	if (v_parameters_size < 1) v_parameters[0] = 0;
	switch (v_parameters[0]) {
	case -1:
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

void t_terminal::f_erase_line()
{
	if (v_parameters_size < 1) v_parameters[0] = 0;
	switch (v_parameters[0]) {
	case -1:
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

void t_terminal::f_erase_character()
{
	if (v_parameters_size < 1 || v_parameters[0] < 1) v_parameters[0] = 1;
	if (v_cursor_x + v_parameters[0] > f_width()) v_parameters[0] = f_width() - v_cursor_x;
	f_erase(v_cursor_x, v_cursor_y, v_parameters[0], false);
}

void t_terminal::f_insert_line()
{
	if (v_parameters_size < 1 || v_parameters[0] < 1) v_parameters[0] = 1;
	unsigned bottom = v_region_begin + v_region_size;
	if (v_cursor_y < v_region_begin || v_cursor_y >= bottom) return;
	f_scroll_down(v_cursor_y, bottom - v_cursor_y, v_parameters[0]);
	v_cursor_x = 0;
}

void t_terminal::f_delete_line()
{
	if (v_parameters_size < 1 || v_parameters[0] < 1) v_parameters[0] = 1;
	unsigned bottom = v_region_begin + v_region_size;
	if (v_cursor_y < v_region_begin || v_cursor_y >= bottom) return;
	f_scroll_up(v_cursor_y, bottom - v_cursor_y, v_parameters[0]);
	v_cursor_x = 0;
}

void t_terminal::f_insert_character()
{
	if (v_parameters_size < 1 || v_parameters[0] < 1) v_parameters[0] = 1;
	f_insert(v_cursor_x, v_cursor_y, v_parameters[0]);
}

void t_terminal::f_delete_character()
{
	if (v_parameters_size < 1 || v_parameters[0] < 1) v_parameters[0] = 1;
	f_erase(v_cursor_x, v_cursor_y, v_parameters[0], true);
}

void t_terminal::f_attribute()
{
	if (v_parameters_size < 1) {
		v_parameters[0] = -1;
		v_parameters_size = 1;
	}
	for (int i = 0; i < v_parameters_size; ++i) {
		switch (v_parameters[i]) {
		case -1:
		case 0:
			v_attribute = ::t_attribute(false, false, false, false, 0, 7);
			break;
		case 1:
			v_attribute.f_bold__(true);
			break;
		case 4:
			v_attribute.f_underline__(true);
			break;
		case 5:
			v_attribute.f_blink__(true);
			break;
		case 7:
			v_attribute.f_reverse__(true);
			break;
		case 22:
			v_attribute.f_bold__(false);
			break;
		case 24:
			v_attribute.f_underline__(false);
			break;
		case 25:
			v_attribute.f_blink__(false);
			break;
		case 27:
			v_attribute.f_reverse__(false);
			break;
		case 30:
		case 31:
		case 32:
		case 33:
		case 34:
		case 35:
		case 36:
		case 37:
			v_attribute.f_foreground__(v_parameters[i] - 30);
			break;
		case 39:
			v_attribute.f_foreground__(0);
			break;
		case 40:
		case 41:
		case 42:
		case 43:
		case 44:
		case 45:
		case 46:
		case 47:
			v_attribute.f_background__(v_parameters[i] - 40);
			break;
		case 49:
			v_attribute.f_background__(7);
			break;
		default:
std::printf("Unknown attribute: %d\n", v_parameters[i]);
		}
	}
}

void t_terminal::f_mode(bool a_mode)
{
	for (int i = 0; i < v_parameters_size; ++i) {
		switch (v_csi) {
		case e_csi__PRIMARY:
			switch (v_parameters[i]) {
			case 4:
				v_mode_insert = a_mode;
				break;
			default:
std::printf("%s unknown mode: %d\n", a_mode ? "Set" : "Reset", v_parameters[i]);
			}
			break;
		case e_csi__SECONDARY:
std::printf("%s unknown secondary mode: %d\n", a_mode ? "Set" : "Reset", v_parameters[i]);
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
				f_alternate__(a_mode);
				v_region_begin = 0;
				v_region_size = f_height();
				if (a_mode) {
					f_save_cursor();
					v_cursor_x = v_cursor_y = 0;
					f_erase(0, f_height());
				} else {
					f_restore_cursor();
				}
				break;
			default:
std::printf("%s unknown private mode: %d\n", a_mode ? "Set" : "Reset", v_parameters[i]);
			}
		}
	}
}

void t_terminal::f_scroll_region()
{
	if (v_parameters_size < 1 || v_parameters[0] < 1) v_parameters[0] = 1;
	--v_parameters[0];
	if (v_parameters_size < 2 || v_parameters[1] < 1) v_parameters[1] = f_height();
	if (v_parameters[0] > static_cast<int>(f_height()) - 2) v_parameters[0] = f_height() - 2;
	if (v_parameters[1] < v_parameters[0] + 2) v_parameters[1] = v_parameters[0] + 2;
	v_region_begin = v_parameters[0];
	v_region_size = v_parameters[1] - v_parameters[0];
	v_cursor_x = v_cursor_y = 0;
}

void t_terminal::f_put(wchar_t a_c)
{
	if (v_cursor_x >= f_width()) {
		if (v_mode_wraparound) {
			f_index();
			v_cursor_x = 0;
		} else {
			v_cursor_x = f_width() - 1;
		}
	}
	t_content::f_put(v_cursor_x, v_cursor_y, a_c, v_mode_insert);
	v_cursor_x += wcwidth(a_c);
}

void t_terminal::f_control_character(wchar_t a_c)
{
	switch (a_c) {
	case L'G' - L'@':
		XBell(f_application()->f_x11_display(), 0);
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
std::printf("Unknown control character: %x\n", a_c);
	}
}

void t_terminal::f_control_sequence(wchar_t a_c)
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
	case L'X':
		f_erase_character();
		break;
	case L'f':
		f_cursor_position();
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
std::printf("Unknown control sequence:");
switch (v_csi) {
case e_csi__SECONDARY:
	std::printf(" >");
	break;
case e_csi__PRIVATE:
	std::printf(" ?");
	break;
}
for (size_t i = 0; i < v_parameters_size; ++i) std::printf(" %d", v_parameters[i]);
std::printf(" %c\n", a_c);
	}
	v_state = &t_terminal::f_state_default;
}

void t_terminal::f_state_default(wchar_t a_c)
{
	if (std::iswcntrl(a_c))
		f_control_character(a_c);
	else
		f_put(a_c);
}

void t_terminal::f_state_escape(wchar_t a_c)
{
	if (std::iswcntrl(a_c)) {
		f_control_character(a_c);
		return;
	}
	switch (a_c) {
	case L'#':
		v_state = &t_terminal::f_state_escape_sharp;
		return;
	case L'7':
		f_save_cursor();
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
		v_cursor_x = 0;
		f_index();
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
std::printf("Unknown escape sequence: %c\n", a_c);
	}
	v_state = &t_terminal::f_state_default;
}

void t_terminal::f_state_escape_sharp(wchar_t a_c)
{
	if (std::iswcntrl(a_c)) {
		f_control_character(a_c);
		return;
	}
	switch (a_c) {
	case L'8':
		for (unsigned i = 0; i < f_height(); ++i)
			for (unsigned j = 0; j < f_width(); ++j)
				t_content::f_put(j, i, L'E', false);
		break;
	default:
std::printf("Unknown escape sequence: # %c\n", a_c);
	}
	v_state = &t_terminal::f_state_default;
}

void t_terminal::f_state_csi(wchar_t a_c)
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

void t_terminal::f_state_csi_parameter(wchar_t a_c)
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

void t_terminal::f_state_csi_parameter_digit(wchar_t a_c)
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

void t_terminal::f_send(const char* a_cs, size_t a_n)
{
	while (a_n > 0) {
		ssize_t n = write(v_master, a_cs, a_n);
		if (n == ssize_t(-1)) {
std::printf("write: %s\n", std::strerror(errno));
			break;
		}
		a_cs += n;
		a_n -= n;
	}
}

void t_terminal::f_send(const wchar_t* a_cs, size_t a_n)
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

bool t_terminal::f_send(t_modifier a_modifier, t_key a_key)
{
	t_code code = f_code(a_key);
	if (code == e_code__NONE) return false;
	size_t i = 0;
	if ((a_modifier & e_modifier__SHIFT) != 0) i |= 1;
	if ((a_modifier & e_modifier__CONTROL) != 0) i |= 2;
	if (v_mode_application_keypad && code >= e_code__KP_ENTER) i = 3;
	if (v_mode_application_cursor && code >= e_code__UP && code <= e_code__LEFT) i = 3;
	const char* cs = v_codes[code][i];
	if (cs) f_send(cs, std::strlen(cs));
	return true;
}

t_terminal::~t_terminal()
{
	delete[] v_tab_stops;
}

void t_terminal::f_on_move()
{
	unsigned m = (f_width() + 7) / 8;
	t_content::f_on_move();
	unsigned n = (f_width() + 7) / 8;
	unsigned char* tab_stops = new unsigned char[n];
	std::fill(std::copy(v_tab_stops, v_tab_stops + std::min(m, n), tab_stops), tab_stops + n, 1);
	delete[] v_tab_stops;
	v_tab_stops = tab_stops;
	v_region_begin = 0;
	v_region_size = f_height();
	struct winsize ws;
	ws.ws_col = f_width();
	ws.ws_row = f_height();
	ws.ws_xpixel = ws.ws_ypixel = 0;
	ioctl(v_master, TIOCSWINSZ, &ws);
}

void t_terminal::f_on_key_press(t_modifier a_modifier, t_key a_key, char a_ascii)
{
	if (!f_send(a_modifier, a_key) && a_ascii != '\0') f_send(&a_ascii, 1);
}

void t_terminal::f_on_input_commit(const wchar_t* a_cs, size_t a_n)
{
	f_send(a_cs, a_n);
}

void t_terminal::operator()()
{
	ssize_t n = read(v_master, v_mbs + v_mbn, sizeof(v_mbs) - v_mbn);
	if (n == ssize_t(-1)) {
std::printf("read: %s\n", std::strerror(errno));
		return;
	}
	v_mbn += n;
	if (v_mbn > 0) {
		f_invalidate_cursor();
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
				(this->*v_state)(L'\0');
				++p;
			} else {
				(this->*v_state)(c);
				p += n;
			}
		}
		v_mbn = q - p;
		f_invalidate_cursor();
		f_application()->f_input_negotiate();
	} else {
		f_application()->f_exit();
	}
}

t_terminal::t_terminal(unsigned a_log, unsigned a_width, unsigned a_height, int a_master) :
t_content(a_log, a_width, a_height), v_master(a_master),
v_mbn(0),
v_mode_insert(false), v_mode_origin(false), v_mode_wraparound(true),
v_mode_application_keypad(false), v_mode_application_cursor(false),
v_region_begin(0), v_region_size(a_height),
v_state(&t_terminal::f_state_default)
{
	std::memset(&v_mbstate, 0, sizeof(std::mbstate_t));
	unsigned n = (f_width() + 7) / 8;
	v_tab_stops = new unsigned char[n];
	std::fill_n(v_tab_stops, n, 1);
	f_application()->f_poll(v_master, this, 0);
	f_input_context__(new t_input_context());
}
