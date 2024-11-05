#ifndef XRAFT__SYMBOL_H
#define XRAFT__SYMBOL_H

#ifdef XRAFT_WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#endif

namespace xraft
{

enum t_key
{
	c_key__NONE = 0,

	/* TTY Functions */
	c_key__BACK_SPACE = 0xFF08,	/* back space, back char */
	c_key__TAB = 0xFF09,
	c_key__LINEFEED = 0xFF0A,	/* Linefeed, LF */
	c_key__CLEAR = 0xFF0B,
	c_key__RETURN = 0xFF0D,	/* Return, enter */
	c_key__PAUSE = 0xFF13,	/* Pause, hold */
	c_key__SCROLL_LOCK = 0xFF14,
	c_key__SYS_REQ = 0xFF15,
	c_key__ESCAPE = 0xFF1B,
	c_key__DELETE = 0xFFFF,	/* Delete, rubout */

	/* Cursor control & motion */
	c_key__HOME = 0xFF50,
	c_key__LEFT = 0xFF51,	/* Move left, left arrow */
	c_key__UP = 0xFF52,	/* Move up, up arrow */
	c_key__RIGHT = 0xFF53,	/* Move right, right arrow */
	c_key__DOWN = 0xFF54,	/* Move down, down arrow */
	c_key__PRIOR = 0xFF55,	/* Prior, previous */
	c_key__NEXT = 0xFF56,	/* Next */
	c_key__END = 0xFF57,	/* EOL */
	c_key__BEGIN = 0xFF58,	/* BOL */

	/* Misc Functions */
	c_key__SELECT = 0xFF60,	/* Select, mark */
	c_key__PRINT = 0xFF61,
	c_key__EXECUTE = 0xFF62,	/* Execute, run, do */
	c_key__INSERT = 0xFF63,	/* Insert, insert here */
	c_key__UNDO = 0xFF65,	/* Undo, oops */
	c_key__REDO = 0xFF66,	/* redo, again */
	c_key__MENU = 0xFF67,
	c_key__FIND = 0xFF68,	/* Find, search */
	c_key__CANCEL = 0xFF69,	/* Cancel, stop, abort, exit */
	c_key__HELP = 0xFF6A,	/* Help */
	c_key__BREAK = 0xFF6B,
	c_key__MODE_SWITCH = 0xFF7E,	/* Character set switch */
	c_key__NUM_LOCK = 0xFF7F,

	/* Keypad Functions */
	c_key__KP_SPACE = 0xFF80,	/* space */
	c_key__KP_TAB = 0xFF89,
	c_key__KP_ENTER = 0xFF8D,	/* enter */
	c_key__KP_F1 = 0xFF91,	/* PF1, KP_A, ... */
	c_key__KP_F2 = 0xFF92,
	c_key__KP_F3 = 0xFF93,
	c_key__KP_F4 = 0xFF94,
	c_key__KP_HOME = 0xFF95,
	c_key__KP_LEFT = 0xFF96,
	c_key__KP_UP = 0xFF97,
	c_key__KP_RIGHT = 0xFF98,
	c_key__KP_DOWN = 0xFF99,
	c_key__KP_PRIOR = 0xFF9A,
	c_key__KP_NEXT = 0xFF9B,
	c_key__KP_END = 0xFF9C,
	c_key__KP_BEGIN = 0xFF9D,
	c_key__KP_INSERT = 0xFF9E,
	c_key__KP_DELETE = 0xFF9F,
	c_key__KP_EQUAL = 0xFFBD,	/* equals */
	c_key__KP_MULTIPLY = 0xFFAA,
	c_key__KP_ADD = 0xFFAB,
	c_key__KP_SEPARATOR = 0xFFAC,	/* separator, often comma */
	c_key__KP_SUBTRACT = 0xFFAD,
	c_key__KP_DECIMAL = 0xFFAE,
	c_key__KP_DIVIDE = 0xFFAF,
	c_key__KP_0 = 0xFFB0,
	c_key__KP_1 = 0xFFB1,
	c_key__KP_2 = 0xFFB2,
	c_key__KP_3 = 0xFFB3,
	c_key__KP_4 = 0xFFB4,
	c_key__KP_5 = 0xFFB5,
	c_key__KP_6 = 0xFFB6,
	c_key__KP_7 = 0xFFB7,
	c_key__KP_8 = 0xFFB8,
	c_key__KP_9 = 0xFFB9,

	/* Auxilliary Functions */
	c_key__F1 = 0xFFBE,
	c_key__F2 = 0xFFBF,
	c_key__F3 = 0xFFC0,
	c_key__F4 = 0xFFC1,
	c_key__F5 = 0xFFC2,
	c_key__F6 = 0xFFC3,
	c_key__F7 = 0xFFC4,
	c_key__F8 = 0xFFC5,
	c_key__F9 = 0xFFC6,
	c_key__F10 = 0xFFC7,
	c_key__F11 = 0xFFC8,
	c_key__F12 = 0xFFC9,
	c_key__F13 = 0xFFCA,
	c_key__F14 = 0xFFCB,
	c_key__F15 = 0xFFCC,
	c_key__F16 = 0xFFCD,
	c_key__F17 = 0xFFCE,
	c_key__F18 = 0xFFCF,
	c_key__F19 = 0xFFD0,
	c_key__F20 = 0xFFD1,
	c_key__F21 = 0xFFD2,
	c_key__F22 = 0xFFD3,
	c_key__F23 = 0xFFD4,
	c_key__F24 = 0xFFD5,

	/* Modifiers */
	c_key__SHIFT_L = 0xFFE1,	/* Left shift */
	c_key__SHIFT_R = 0xFFE2,	/* Right shift */
	c_key__CONTROL_L = 0xFFE3,	/* Left control */
	c_key__CONTROL_R = 0xFFE4,	/* Right control */
	c_key__CAPS_LOCK = 0xFFE5,	/* Caps lock */
	c_key__SHIFT_LOCK = 0xFFE6,	/* Shift lock */
	c_key__META_L = 0xFFE7,	/* Left meta */
	c_key__META_R = 0xFFE8,	/* Right meta */
	c_key__ALT_L = 0xFFE9,	/* Left alt */
	c_key__ALT_R = 0xFFEA,	/* Right alt */

	/* * ISO 9995 Function and Modifier Keys */
	c_key__LEFT_TAB = 0xFE20,

	/* Latin 1 */
	c_key__SPACE = 0x020,
	c_key__EXCLAM = 0x021,
	c_key__QUOTEDBL = 0x022,
	c_key__NUMBERSIGN = 0x023,
	c_key__DOLLAR = 0x024,
	c_key__PERCENT = 0x025,
	c_key__AMPERSAND = 0x026,
	c_key__APOSTROPHE = 0x027,
	c_key__PARENLEFT = 0x028,
	c_key__PARENRIGHT = 0x029,
	c_key__ASTERISK = 0x02a,
	c_key__PLUS = 0x02b,
	c_key__COMMA = 0x02c,
	c_key__MINUS = 0x02d,
	c_key__PERIOD = 0x02e,
	c_key__SLASH = 0x02f,
	c_key__0 = 0x030,
	c_key__1 = 0x031,
	c_key__2 = 0x032,
	c_key__3 = 0x033,
	c_key__4 = 0x034,
	c_key__5 = 0x035,
	c_key__6 = 0x036,
	c_key__7 = 0x037,
	c_key__8 = 0x038,
	c_key__9 = 0x039,
	c_key__COLON = 0x03a,
	c_key__SEMICOLON = 0x03b,
	c_key__LESS = 0x03c,
	c_key__EQUAL = 0x03d,
	c_key__GREATER = 0x03e,
	c_key__QUESTION = 0x03f,
	c_key__AT = 0x040,
	c_key__A = 0x041,
	c_key__B = 0x042,
	c_key__C = 0x043,
	c_key__D = 0x044,
	c_key__E = 0x045,
	c_key__F = 0x046,
	c_key__G = 0x047,
	c_key__H = 0x048,
	c_key__I = 0x049,
	c_key__J = 0x04a,
	c_key__K = 0x04b,
	c_key__L = 0x04c,
	c_key__M = 0x04d,
	c_key__N = 0x04e,
	c_key__O = 0x04f,
	c_key__P = 0x050,
	c_key__Q = 0x051,
	c_key__R = 0x052,
	c_key__S = 0x053,
	c_key__T = 0x054,
	c_key__U = 0x055,
	c_key__V = 0x056,
	c_key__W = 0x057,
	c_key__X = 0x058,
	c_key__Y = 0x059,
	c_key__Z = 0x05a,
	c_key__BRACKETLEFT = 0x05b,
	c_key__BACKSLASH = 0x05c,
	c_key__BRACKETRIGHT = 0x05d,
	c_key__ASCIICIRCUM = 0x05e,
	c_key__UNDERSCORE = 0x05f,
	c_key__GRAVE = 0x060,
	c_key__BRACELEFT = 0x07b,
	c_key__BAR = 0x07c,
	c_key__BRACERIGHT = 0x07d,
	c_key__ASCIITILDE = 0x07e,
};

enum t_modifier
{
	c_modifier__NONE = 0,
	c_modifier__SHIFT = 1 << 0,
	c_modifier__LOCK = 1 << 1,
	c_modifier__CONTROL = 1 << 2,
	c_modifier__ALTERNATE = 1 << 3,
	c_modifier__META = 1 << 4,
	c_modifier__BUTTON1 = 1 << 8,
	c_modifier__BUTTON2 = 1 << 9,
	c_modifier__BUTTON3 = 1 << 10,
	c_modifier__BUTTON4 = 1 << 11,
	c_modifier__BUTTON5 = 1 << 12,
};

enum t_button
{
	c_button__NONE = 0,
	c_button__1 = 1,
	c_button__2 = 2,
	c_button__3 = 3,
	c_button__4 = 4,
	c_button__5 = 5,
};

#ifdef XRAFT_WIN32
inline t_key f_win32_key(UINT a_key)
{
	switch (a_key) {
/*
	case VK_LBUTTON:
		return c_key__LBUTTON;
	case VK_RBUTTON:
		return c_key__RBUTTON;
*/
	case VK_CANCEL:
		return c_key__CANCEL;
/*
	case VK_MBUTTON:    // NOT contiguous with L & RBUTTON
		return c_key__MBUTTON;
	case VK_XBUTTON1:    // NOT contiguous with L & RBUTTON
		return c_key__XBUTTON1;
	case VK_XBUTTON2:    // NOT contiguous with L & RBUTTON
		return c_key__XBUTTON2;
*/
	case VK_BACK:
		return c_key__BACK_SPACE;
	case VK_TAB:
		return c_key__TAB;
	case VK_CLEAR:
		return c_key__CLEAR;
	case VK_RETURN:
		return c_key__RETURN;
	case VK_SHIFT:
		return c_key__SHIFT_L;
	case VK_CONTROL:
		return c_key__CONTROL_L;
	case VK_MENU:
		return c_key__ALT_L;
	case VK_PAUSE:
		return c_key__PAUSE;
	case VK_CAPITAL:
/*
	case VK_KANA:
		return c_key__KANA;
	case VK_HANGEUL:  // old name - should be here for compatibility
	case VK_HANGUL:
		return c_key__HANGUL;
	case VK_JUNJA:
		return c_key__JUNJA;
	case VK_FINAL:
		return c_key__FINAL;
	case VK_HANJA:
		return c_key__HANJA;
	case VK_KANJI:
		return c_key__KANJI;
*/
	case VK_ESCAPE:
		return c_key__ESCAPE;
/*
	case VK_CONVERT:
		return c_key__CONVERT;
	case VK_NONCONVERT:
		return c_key__NONCONVERT;
	case VK_ACCEPT:
		return c_key__ACCEPT;
	case VK_MODECHANGE:
		return c_key__MODECHANGE;
*/
	case VK_SPACE:
		return c_key__SPACE;
	case VK_PRIOR:
		return c_key__PRIOR;
	case VK_NEXT:
		return c_key__NEXT;
	case VK_END:
		return c_key__END;
	case VK_HOME:
		return c_key__HOME;
	case VK_LEFT:
		return c_key__LEFT;
	case VK_UP:
		return c_key__UP;
	case VK_RIGHT:
		return c_key__RIGHT;
	case VK_DOWN:
		return c_key__DOWN;
	case VK_SELECT:
		return c_key__SELECT;
	case VK_PRINT:
		return c_key__PRINT;
	case VK_EXECUTE:
		return c_key__EXECUTE;
/*
	case VK_SNAPSHOT:
		return c_key__SNAPSHOT;
*/
	case VK_INSERT:
		return c_key__INSERT;
	case VK_DELETE:
		return c_key__DELETE;
	case VK_HELP:
		return c_key__HELP;
	case '0':
		return c_key__0;
	case '1':
		return c_key__1;
	case '2':
		return c_key__2;
	case '3':
		return c_key__3;
	case '4':
		return c_key__4;
	case '5':
		return c_key__5;
	case '6':
		return c_key__6;
	case '7':
		return c_key__7;
	case '8':
		return c_key__8;
	case '9':
		return c_key__9;
	case 'A':
		return c_key__A;
	case 'B':
		return c_key__B;
	case 'C':
		return c_key__C;
	case 'D':
		return c_key__D;
	case 'E':
		return c_key__E;
	case 'F':
		return c_key__F;
	case 'G':
		return c_key__G;
	case 'H':
		return c_key__H;
	case 'I':
		return c_key__I;
	case 'J':
		return c_key__J;
	case 'K':
		return c_key__K;
	case 'L':
		return c_key__L;
	case 'M':
		return c_key__M;
	case 'N':
		return c_key__N;
	case 'O':
		return c_key__O;
	case 'P':
		return c_key__P;
	case 'Q':
		return c_key__Q;
	case 'R':
		return c_key__R;
	case 'S':
		return c_key__S;
	case 'T':
		return c_key__T;
	case 'U':
		return c_key__U;
	case 'V':
		return c_key__V;
	case 'W':
		return c_key__W;
	case 'X':
		return c_key__X;
	case 'Y':
		return c_key__Y;
	case 'Z':
		return c_key__Z;
	case VK_LWIN:
		return c_key__META_L;
	case VK_RWIN:
		return c_key__META_R;
/*
	case VK_APPS:
		return c_key__APPS;
	case VK_SLEEP:
		return c_key__SLEEP;
*/
	case VK_NUMPAD0:
		return c_key__KP_0;
	case VK_NUMPAD1:
		return c_key__KP_1;
	case VK_NUMPAD2:
		return c_key__KP_2;
	case VK_NUMPAD3:
		return c_key__KP_3;
	case VK_NUMPAD4:
		return c_key__KP_4;
	case VK_NUMPAD5:
		return c_key__KP_5;
	case VK_NUMPAD6:
		return c_key__KP_6;
	case VK_NUMPAD7:
		return c_key__KP_7;
	case VK_NUMPAD8:
		return c_key__KP_8;
	case VK_NUMPAD9:
		return c_key__KP_9;
	case VK_MULTIPLY:
		return c_key__KP_MULTIPLY;
	case VK_ADD:
		return c_key__KP_ADD;
	case VK_SEPARATOR:
		return c_key__KP_SEPARATOR;
	case VK_SUBTRACT:
		return c_key__KP_SUBTRACT;
	case VK_DECIMAL:
		return c_key__KP_DECIMAL;
	case VK_DIVIDE:
		return c_key__KP_DIVIDE;
	case VK_F1:
		return c_key__F1;
	case VK_F2:
		return c_key__F2;
	case VK_F3:
		return c_key__F3;
	case VK_F4:
		return c_key__F4;
	case VK_F5:
		return c_key__F5;
	case VK_F6:
		return c_key__F6;
	case VK_F7:
		return c_key__F7;
	case VK_F8:
		return c_key__F8;
	case VK_F9:
		return c_key__F9;
	case VK_F10:
		return c_key__F10;
	case VK_F11:
		return c_key__F11;
	case VK_F12:
		return c_key__F12;
	case VK_F13:
		return c_key__F13;
	case VK_F14:
		return c_key__F14;
	case VK_F15:
		return c_key__F15;
	case VK_F16:
		return c_key__F16;
	case VK_F17:
		return c_key__F17;
	case VK_F18:
		return c_key__F18;
	case VK_F19:
		return c_key__F19;
	case VK_F20:
		return c_key__F20;
	case VK_F21:
		return c_key__F21;
	case VK_F22:
		return c_key__F22;
	case VK_F23:
		return c_key__F23;
	case VK_F24:
		return c_key__F24;
	case VK_NUMLOCK:
		return c_key__NUM_LOCK;
	case VK_SCROLL:
		return c_key__SCROLL_LOCK;
/*
	case VK_OEM_NEC_EQUAL:   // '=' key on numpad
		return c_key__OEM_NEC_EQUAL;
	case VK_OEM_FJ_JISHO:   // 'Dictionary' key
		return c_key__OEM_FJ_JISHO;
	case VK_OEM_FJ_MASSHOU:   // 'Unregister word' key
		return c_key__OEM_FJ_MASSHOU;
	case VK_OEM_FJ_TOUROKU:   // 'Register word' key
		return c_key__OEM_FJ_TOUROKU;
	case VK_OEM_FJ_LOYA:   // 'Left OYAYUBI' key
		return c_key__OEM_FJ_LOYA;
	case VK_OEM_FJ_ROYA:   // 'Right OYAYUBI' key
		return c_key__OEM_FJ_ROYA;
*/
	case VK_LSHIFT:
		return c_key__SHIFT_L;
	case VK_RSHIFT:
		return c_key__SHIFT_R;
	case VK_LCONTROL:
		return c_key__CONTROL_L;
	case VK_RCONTROL:
		return c_key__CONTROL_R;
	case VK_LMENU:
		return c_key__ALT_L;
	case VK_RMENU:
		return c_key__ALT_R;
/*
	case VK_BROWSER_BACK:
		return c_key__BROWSER_BACK;
	case VK_BROWSER_FORWARD:
		return c_key__BROWSER_FORWARD;
	case VK_BROWSER_REFRESH:
		return c_key__BROWSER_REFRESH;
	case VK_BROWSER_STOP:
		return c_key__BROWSER_STOP;
	case VK_BROWSER_SEARCH:
		return c_key__BROWSER_SEARCH;
	case VK_BROWSER_FAVORITES:
		return c_key__BROWSER_FAVORITES;
	case VK_BROWSER_HOME:
		return c_key__BROWSER_HOME;
	case VK_VOLUME_MUTE:
		return c_key__VOLUME_MUTE;
	case VK_VOLUME_DOWN:
		return c_key__VOLUME_DOWN;
	case VK_VOLUME_UP:
		return c_key__VOLUME_UP;
	case VK_MEDIA_NEXT_TRACK:
		return c_key__MEDIA_NEXT_TRACK;
	case VK_MEDIA_PREV_TRACK:
		return c_key__MEDIA_PREV_TRACK;
	case VK_MEDIA_STOP:
		return c_key__MEDIA_STOP;
	case VK_MEDIA_PLAY_PAUSE:
		return c_key__MEDIA_PLAY_PAUSE;
	case VK_LAUNCH_MAIL:
		return c_key__LAUNCH_MAIL;
	case VK_LAUNCH_MEDIA_SELECT:
		return c_key__LAUNCH_MEDIA_SELECT;
	case VK_LAUNCH_APP1:
		return c_key__LAUNCH_APP1;
	case VK_LAUNCH_APP2:
		return c_key__LAUNCH_APP2;
*/
	case VK_OEM_1:   // ';:' for US
		return c_key__SEMICOLON;
	case VK_OEM_PLUS:   // '+' any country
		//return c_key__PLUS;
		return c_key__EQUAL;
	case VK_OEM_COMMA:   // ',' any country
		return c_key__COMMA;
	case VK_OEM_MINUS:   // '-' any country
		return c_key__MINUS;
	case VK_OEM_PERIOD:   // '.' any country
		return c_key__PERIOD;
	case VK_OEM_2:   // '/?' for US
		return c_key__SLASH;
	case VK_OEM_3:   // '`~' for US
		return c_key__GRAVE;
	case VK_OEM_4:  //  '[{' for US
		return c_key__BRACKETLEFT;
	case VK_OEM_5:  //  '\|' for US
		return c_key__BACKSLASH;
	case VK_OEM_6:  //  ']}' for US
		return c_key__BRACKETRIGHT;
	case VK_OEM_7:  //  ''"' for US
		return c_key__APOSTROPHE;
/*
	case VK_OEM_8:
		return c_key__OEM_8;
	case VK_OEM_AX:  //  'AX' key on Japanese AX kbd
		return c_key__OEM_AX;
	case VK_OEM_102:  //  "<>" or "\|" on RT 102-key kbd.
		return c_key__OEM_102;
	case VK_ICO_HELP:  //  Help key on ICO
		return c_key__OEM_HELP;
	case VK_ICO_00:  //  00 key on ICO
		return c_key__OEM_00;
	case VK_PROCESSKEY:
		return c_key__PROCESSKEY;
	case VK_ICO_CLEAR:
		return c_key__ICO_CLEAR;
	case VK_PACKET:
		return c_key__PACKET;
	case VK_OEM_RESET:
		return c_key__OEM_RESET;
	case VK_OEM_JUMP:
		return c_key__OEM_JUMP;
	case VK_OEM_PA1:
		return c_key__OEM_PA1;
	case VK_OEM_PA2:
		return c_key__OEM_PA2;
	case VK_OEM_PA3:
		return c_key__OEM_PA3;
	case VK_OEM_WSCTRL:
		return c_key__OEM_WSCTRL;
	case VK_OEM_CUSEL:
		return c_key__OEM_CUSEL;
	case VK_OEM_ATTN:
		return c_key__OEM_ATTN;
	case VK_OEM_FINISH:
		return c_key__OEM_FINISH;
	case VK_OEM_COPY:
		return c_key__OEM_COPY;
	case VK_OEM_AUTO:
		return c_key__OEM_AUTO;
	case VK_OEM_ENLW:
		return c_key__OEM_ENLW;
	case VK_OEM_BACKTAB:
		return c_key__OEM_BACKTAB;
	case VK_ATTN:
		return c_key__ATTN;
	case VK_CRSEL:
		return c_key__CRSEL;
	case VK_EXSEL:
		return c_key__EXSEL;
	case VK_EREOF:
		return c_key__EREOF;
	case VK_PLAY:
		return c_key__PLAY;
	case VK_ZOOM:
		return c_key__ZOOM;
	case VK_NONAME:
		return c_key__NONAME;
	case VK_PA1:
		return c_key__PA1;
	case VK_OEM_CLEAR:
		return c_key__OEM_CLEAR;
*/
	default:
		return c_key__NONE;
	}
}
#endif

}

#endif
