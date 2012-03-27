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
	e_key__NONE = 0,

	/* TTY Functions */
	e_key__BACK_SPACE = 0xFF08,	/* back space, back char */
	e_key__TAB = 0xFF09,
	e_key__LINEFEED = 0xFF0A,	/* Linefeed, LF */
	e_key__CLEAR = 0xFF0B,
	e_key__RETURN = 0xFF0D,	/* Return, enter */
	e_key__PAUSE = 0xFF13,	/* Pause, hold */
	e_key__SCROLL_LOCK = 0xFF14,
	e_key__SYS_REQ = 0xFF15,
	e_key__ESCAPE = 0xFF1B,
	e_key__DELETE = 0xFFFF,	/* Delete, rubout */

	/* Cursor control & motion */
	e_key__HOME = 0xFF50,
	e_key__LEFT = 0xFF51,	/* Move left, left arrow */
	e_key__UP = 0xFF52,	/* Move up, up arrow */
	e_key__RIGHT = 0xFF53,	/* Move right, right arrow */
	e_key__DOWN = 0xFF54,	/* Move down, down arrow */
	e_key__PRIOR = 0xFF55,	/* Prior, previous */
	e_key__NEXT = 0xFF56,	/* Next */
	e_key__END = 0xFF57,	/* EOL */
	e_key__BEGIN = 0xFF58,	/* BOL */

	/* Misc Functions */
	e_key__SELECT = 0xFF60,	/* Select, mark */
	e_key__PRINT = 0xFF61,
	e_key__EXECUTE = 0xFF62,	/* Execute, run, do */
	e_key__INSERT = 0xFF63,	/* Insert, insert here */
	e_key__UNDO = 0xFF65,	/* Undo, oops */
	e_key__REDO = 0xFF66,	/* redo, again */
	e_key__MENU = 0xFF67,
	e_key__FIND = 0xFF68,	/* Find, search */
	e_key__CANCEL = 0xFF69,	/* Cancel, stop, abort, exit */
	e_key__HELP = 0xFF6A,	/* Help */
	e_key__BREAK = 0xFF6B,
	e_key__MODE_SWITCH = 0xFF7E,	/* Character set switch */
	e_key__NUM_LOCK = 0xFF7F,

	/* Keypad Functions */
	e_key__KP_SPACE = 0xFF80,	/* space */
	e_key__KP_TAB = 0xFF89,
	e_key__KP_ENTER = 0xFF8D,	/* enter */
	e_key__KP_F1 = 0xFF91,	/* PF1, KP_A, ... */
	e_key__KP_F2 = 0xFF92,
	e_key__KP_F3 = 0xFF93,
	e_key__KP_F4 = 0xFF94,
	e_key__KP_HOME = 0xFF95,
	e_key__KP_LEFT = 0xFF96,
	e_key__KP_UP = 0xFF97,
	e_key__KP_RIGHT = 0xFF98,
	e_key__KP_DOWN = 0xFF99,
	e_key__KP_PRIOR = 0xFF9A,
	e_key__KP_NEXT = 0xFF9B,
	e_key__KP_END = 0xFF9C,
	e_key__KP_BEGIN = 0xFF9D,
	e_key__KP_INSERT = 0xFF9E,
	e_key__KP_DELETE = 0xFF9F,
	e_key__KP_EQUAL = 0xFFBD,	/* equals */
	e_key__KP_MULTIPLY = 0xFFAA,
	e_key__KP_ADD = 0xFFAB,
	e_key__KP_SEPARATOR = 0xFFAC,	/* separator, often comma */
	e_key__KP_SUBTRACT = 0xFFAD,
	e_key__KP_DECIMAL = 0xFFAE,
	e_key__KP_DIVIDE = 0xFFAF,
	e_key__KP_0 = 0xFFB0,
	e_key__KP_1 = 0xFFB1,
	e_key__KP_2 = 0xFFB2,
	e_key__KP_3 = 0xFFB3,
	e_key__KP_4 = 0xFFB4,
	e_key__KP_5 = 0xFFB5,
	e_key__KP_6 = 0xFFB6,
	e_key__KP_7 = 0xFFB7,
	e_key__KP_8 = 0xFFB8,
	e_key__KP_9 = 0xFFB9,

	/* Auxilliary Functions */
	e_key__F1 = 0xFFBE,
	e_key__F2 = 0xFFBF,
	e_key__F3 = 0xFFC0,
	e_key__F4 = 0xFFC1,
	e_key__F5 = 0xFFC2,
	e_key__F6 = 0xFFC3,
	e_key__F7 = 0xFFC4,
	e_key__F8 = 0xFFC5,
	e_key__F9 = 0xFFC6,
	e_key__F10 = 0xFFC7,
	e_key__F11 = 0xFFC8,
	e_key__F12 = 0xFFC9,
	e_key__F13 = 0xFFCA,
	e_key__F14 = 0xFFCB,
	e_key__F15 = 0xFFCC,
	e_key__F16 = 0xFFCD,
	e_key__F17 = 0xFFCE,
	e_key__F18 = 0xFFCF,
	e_key__F19 = 0xFFD0,
	e_key__F20 = 0xFFD1,
	e_key__F21 = 0xFFD2,
	e_key__F22 = 0xFFD3,
	e_key__F23 = 0xFFD4,
	e_key__F24 = 0xFFD5,

	/* Modifiers */
	e_key__SHIFT_L = 0xFFE1,	/* Left shift */
	e_key__SHIFT_R = 0xFFE2,	/* Right shift */
	e_key__CONTROL_L = 0xFFE3,	/* Left control */
	e_key__CONTROL_R = 0xFFE4,	/* Right control */
	e_key__CAPS_LOCK = 0xFFE5,	/* Caps lock */
	e_key__SHIFT_LOCK = 0xFFE6,	/* Shift lock */
	e_key__META_L = 0xFFE7,	/* Left meta */
	e_key__META_R = 0xFFE8,	/* Right meta */
	e_key__ALT_L = 0xFFE9,	/* Left alt */
	e_key__ALT_R = 0xFFEA,	/* Right alt */

	/* * ISO 9995 Function and Modifier Keys */
	e_key__LEFT_TAB = 0xFE20,

	/* Latin 1 */
	e_key__SPACE = 0x020,
	e_key__EXCLAM = 0x021,
	e_key__QUOTEDBL = 0x022,
	e_key__NUMBERSIGN = 0x023,
	e_key__DOLLAR = 0x024,
	e_key__PERCENT = 0x025,
	e_key__AMPERSAND = 0x026,
	e_key__APOSTROPHE = 0x027,
	e_key__PARENLEFT = 0x028,
	e_key__PARENRIGHT = 0x029,
	e_key__ASTERISK = 0x02a,
	e_key__PLUS = 0x02b,
	e_key__COMMA = 0x02c,
	e_key__MINUS = 0x02d,
	e_key__PERIOD = 0x02e,
	e_key__SLASH = 0x02f,
	e_key__0 = 0x030,
	e_key__1 = 0x031,
	e_key__2 = 0x032,
	e_key__3 = 0x033,
	e_key__4 = 0x034,
	e_key__5 = 0x035,
	e_key__6 = 0x036,
	e_key__7 = 0x037,
	e_key__8 = 0x038,
	e_key__9 = 0x039,
	e_key__COLON = 0x03a,
	e_key__SEMICOLON = 0x03b,
	e_key__LESS = 0x03c,
	e_key__EQUAL = 0x03d,
	e_key__GREATER = 0x03e,
	e_key__QUESTION = 0x03f,
	e_key__AT = 0x040,
	e_key__A = 0x041,
	e_key__B = 0x042,
	e_key__C = 0x043,
	e_key__D = 0x044,
	e_key__E = 0x045,
	e_key__F = 0x046,
	e_key__G = 0x047,
	e_key__H = 0x048,
	e_key__I = 0x049,
	e_key__J = 0x04a,
	e_key__K = 0x04b,
	e_key__L = 0x04c,
	e_key__M = 0x04d,
	e_key__N = 0x04e,
	e_key__O = 0x04f,
	e_key__P = 0x050,
	e_key__Q = 0x051,
	e_key__R = 0x052,
	e_key__S = 0x053,
	e_key__T = 0x054,
	e_key__U = 0x055,
	e_key__V = 0x056,
	e_key__W = 0x057,
	e_key__X = 0x058,
	e_key__Y = 0x059,
	e_key__Z = 0x05a,
	e_key__BRACKETLEFT = 0x05b,
	e_key__BACKSLASH = 0x05c,
	e_key__BRACKETRIGHT = 0x05d,
	e_key__ASCIICIRCUM = 0x05e,
	e_key__UNDERSCORE = 0x05f,
	e_key__GRAVE = 0x060,
	e_key__BRACELEFT = 0x07b,
	e_key__BAR = 0x07c,
	e_key__BRACERIGHT = 0x07d,
	e_key__ASCIITILDE = 0x07e,
};

enum t_modifier
{
	e_modifier__NONE = 0,
	e_modifier__SHIFT = 1 << 0,
	e_modifier__LOCK = 1 << 1,
	e_modifier__CONTROL = 1 << 2,
	e_modifier__ALTERNATE = 1 << 3,
	e_modifier__META = 1 << 4,
	e_modifier__BUTTON1 = 1 << 8,
	e_modifier__BUTTON2 = 1 << 9,
	e_modifier__BUTTON3 = 1 << 10,
	e_modifier__BUTTON4 = 1 << 11,
	e_modifier__BUTTON5 = 1 << 12,
};

enum t_button
{
	e_button__NONE = 0,
	e_button__1 = 1,
	e_button__2 = 2,
	e_button__3 = 3,
	e_button__4 = 4,
	e_button__5 = 5,
};

#ifdef XRAFT_WIN32
inline t_key f_win32_key(UINT a_key)
{
	switch (a_key) {
/*
	case VK_LBUTTON:
		return e_key__LBUTTON;
	case VK_RBUTTON:
		return e_key__RBUTTON;
*/
	case VK_CANCEL:
		return e_key__CANCEL;
/*
	case VK_MBUTTON:    // NOT contiguous with L & RBUTTON
		return e_key__MBUTTON;
	case VK_XBUTTON1:    // NOT contiguous with L & RBUTTON
		return e_key__XBUTTON1;
	case VK_XBUTTON2:    // NOT contiguous with L & RBUTTON
		return e_key__XBUTTON2;
*/
	case VK_BACK:
		return e_key__BACK_SPACE;
	case VK_TAB:
		return e_key__TAB;
	case VK_CLEAR:
		return e_key__CLEAR;
	case VK_RETURN:
		return e_key__RETURN;
	case VK_SHIFT:
		return e_key__SHIFT_L;
	case VK_CONTROL:
		return e_key__CONTROL_L;
	case VK_MENU:
		return e_key__ALT_L;
	case VK_PAUSE:
		return e_key__PAUSE;
	case VK_CAPITAL:
/*
	case VK_KANA:
		return e_key__KANA;
	case VK_HANGEUL:  // old name - should be here for compatibility
	case VK_HANGUL:
		return e_key__HANGUL;
	case VK_JUNJA:
		return e_key__JUNJA;
	case VK_FINAL:
		return e_key__FINAL;
	case VK_HANJA:
		return e_key__HANJA;
	case VK_KANJI:
		return e_key__KANJI;
*/
	case VK_ESCAPE:
		return e_key__ESCAPE;
/*
	case VK_CONVERT:
		return e_key__CONVERT;
	case VK_NONCONVERT:
		return e_key__NONCONVERT;
	case VK_ACCEPT:
		return e_key__ACCEPT;
	case VK_MODECHANGE:
		return e_key__MODECHANGE;
*/
	case VK_SPACE:
		return e_key__SPACE;
	case VK_PRIOR:
		return e_key__PRIOR;
	case VK_NEXT:
		return e_key__NEXT;
	case VK_END:
		return e_key__END;
	case VK_HOME:
		return e_key__HOME;
	case VK_LEFT:
		return e_key__LEFT;
	case VK_UP:
		return e_key__UP;
	case VK_RIGHT:
		return e_key__RIGHT;
	case VK_DOWN:
		return e_key__DOWN;
	case VK_SELECT:
		return e_key__SELECT;
	case VK_PRINT:
		return e_key__PRINT;
	case VK_EXECUTE:
		return e_key__EXECUTE;
/*
	case VK_SNAPSHOT:
		return e_key__SNAPSHOT;
*/
	case VK_INSERT:
		return e_key__INSERT;
	case VK_DELETE:
		return e_key__DELETE;
	case VK_HELP:
		return e_key__HELP;
	case '0':
		return e_key__0;
	case '1':
		return e_key__1;
	case '2':
		return e_key__2;
	case '3':
		return e_key__3;
	case '4':
		return e_key__4;
	case '5':
		return e_key__5;
	case '6':
		return e_key__6;
	case '7':
		return e_key__7;
	case '8':
		return e_key__8;
	case '9':
		return e_key__9;
	case 'A':
		return e_key__A;
	case 'B':
		return e_key__B;
	case 'C':
		return e_key__C;
	case 'D':
		return e_key__D;
	case 'E':
		return e_key__E;
	case 'F':
		return e_key__F;
	case 'G':
		return e_key__G;
	case 'H':
		return e_key__H;
	case 'I':
		return e_key__I;
	case 'J':
		return e_key__J;
	case 'K':
		return e_key__K;
	case 'L':
		return e_key__L;
	case 'M':
		return e_key__M;
	case 'N':
		return e_key__N;
	case 'O':
		return e_key__O;
	case 'P':
		return e_key__P;
	case 'Q':
		return e_key__Q;
	case 'R':
		return e_key__R;
	case 'S':
		return e_key__S;
	case 'T':
		return e_key__T;
	case 'U':
		return e_key__U;
	case 'V':
		return e_key__V;
	case 'W':
		return e_key__W;
	case 'X':
		return e_key__X;
	case 'Y':
		return e_key__Y;
	case 'Z':
		return e_key__Z;
	case VK_LWIN:
		return e_key__META_L;
	case VK_RWIN:
		return e_key__META_R;
/*
	case VK_APPS:
		return e_key__APPS;
	case VK_SLEEP:
		return e_key__SLEEP;
*/
	case VK_NUMPAD0:
		return e_key__KP_0;
	case VK_NUMPAD1:
		return e_key__KP_1;
	case VK_NUMPAD2:
		return e_key__KP_2;
	case VK_NUMPAD3:
		return e_key__KP_3;
	case VK_NUMPAD4:
		return e_key__KP_4;
	case VK_NUMPAD5:
		return e_key__KP_5;
	case VK_NUMPAD6:
		return e_key__KP_6;
	case VK_NUMPAD7:
		return e_key__KP_7;
	case VK_NUMPAD8:
		return e_key__KP_8;
	case VK_NUMPAD9:
		return e_key__KP_9;
	case VK_MULTIPLY:
		return e_key__KP_MULTIPLY;
	case VK_ADD:
		return e_key__KP_ADD;
	case VK_SEPARATOR:
		return e_key__KP_SEPARATOR;
	case VK_SUBTRACT:
		return e_key__KP_SUBTRACT;
	case VK_DECIMAL:
		return e_key__KP_DECIMAL;
	case VK_DIVIDE:
		return e_key__KP_DIVIDE;
	case VK_F1:
		return e_key__F1;
	case VK_F2:
		return e_key__F2;
	case VK_F3:
		return e_key__F3;
	case VK_F4:
		return e_key__F4;
	case VK_F5:
		return e_key__F5;
	case VK_F6:
		return e_key__F6;
	case VK_F7:
		return e_key__F7;
	case VK_F8:
		return e_key__F8;
	case VK_F9:
		return e_key__F9;
	case VK_F10:
		return e_key__F10;
	case VK_F11:
		return e_key__F11;
	case VK_F12:
		return e_key__F12;
	case VK_F13:
		return e_key__F13;
	case VK_F14:
		return e_key__F14;
	case VK_F15:
		return e_key__F15;
	case VK_F16:
		return e_key__F16;
	case VK_F17:
		return e_key__F17;
	case VK_F18:
		return e_key__F18;
	case VK_F19:
		return e_key__F19;
	case VK_F20:
		return e_key__F20;
	case VK_F21:
		return e_key__F21;
	case VK_F22:
		return e_key__F22;
	case VK_F23:
		return e_key__F23;
	case VK_F24:
		return e_key__F24;
	case VK_NUMLOCK:
		return e_key__NUM_LOCK;
	case VK_SCROLL:
		return e_key__SCROLL_LOCK;
/*
	case VK_OEM_NEC_EQUAL:   // '=' key on numpad
		return e_key__OEM_NEC_EQUAL;
	case VK_OEM_FJ_JISHO:   // 'Dictionary' key
		return e_key__OEM_FJ_JISHO;
	case VK_OEM_FJ_MASSHOU:   // 'Unregister word' key
		return e_key__OEM_FJ_MASSHOU;
	case VK_OEM_FJ_TOUROKU:   // 'Register word' key
		return e_key__OEM_FJ_TOUROKU;
	case VK_OEM_FJ_LOYA:   // 'Left OYAYUBI' key
		return e_key__OEM_FJ_LOYA;
	case VK_OEM_FJ_ROYA:   // 'Right OYAYUBI' key
		return e_key__OEM_FJ_ROYA;
*/
	case VK_LSHIFT:
		return e_key__SHIFT_L;
	case VK_RSHIFT:
		return e_key__SHIFT_R;
	case VK_LCONTROL:
		return e_key__CONTROL_L;
	case VK_RCONTROL:
		return e_key__CONTROL_R;
	case VK_LMENU:
		return e_key__ALT_L;
	case VK_RMENU:
		return e_key__ALT_R;
/*
	case VK_BROWSER_BACK:
		return e_key__BROWSER_BACK;
	case VK_BROWSER_FORWARD:
		return e_key__BROWSER_FORWARD;
	case VK_BROWSER_REFRESH:
		return e_key__BROWSER_REFRESH;
	case VK_BROWSER_STOP:
		return e_key__BROWSER_STOP;
	case VK_BROWSER_SEARCH:
		return e_key__BROWSER_SEARCH;
	case VK_BROWSER_FAVORITES:
		return e_key__BROWSER_FAVORITES;
	case VK_BROWSER_HOME:
		return e_key__BROWSER_HOME;
	case VK_VOLUME_MUTE:
		return e_key__VOLUME_MUTE;
	case VK_VOLUME_DOWN:
		return e_key__VOLUME_DOWN;
	case VK_VOLUME_UP:
		return e_key__VOLUME_UP;
	case VK_MEDIA_NEXT_TRACK:
		return e_key__MEDIA_NEXT_TRACK;
	case VK_MEDIA_PREV_TRACK:
		return e_key__MEDIA_PREV_TRACK;
	case VK_MEDIA_STOP:
		return e_key__MEDIA_STOP;
	case VK_MEDIA_PLAY_PAUSE:
		return e_key__MEDIA_PLAY_PAUSE;
	case VK_LAUNCH_MAIL:
		return e_key__LAUNCH_MAIL;
	case VK_LAUNCH_MEDIA_SELECT:
		return e_key__LAUNCH_MEDIA_SELECT;
	case VK_LAUNCH_APP1:
		return e_key__LAUNCH_APP1;
	case VK_LAUNCH_APP2:
		return e_key__LAUNCH_APP2;
*/
	case VK_OEM_1:   // ';:' for US
		return e_key__SEMICOLON;
	case VK_OEM_PLUS:   // '+' any country
		//return e_key__PLUS;
		return e_key__EQUAL;
	case VK_OEM_COMMA:   // ',' any country
		return e_key__COMMA;
	case VK_OEM_MINUS:   // '-' any country
		return e_key__MINUS;
	case VK_OEM_PERIOD:   // '.' any country
		return e_key__PERIOD;
	case VK_OEM_2:   // '/?' for US
		return e_key__SLASH;
	case VK_OEM_3:   // '`~' for US
		return e_key__GRAVE;
	case VK_OEM_4:  //  '[{' for US
		return e_key__BRACKETLEFT;
	case VK_OEM_5:  //  '\|' for US
		return e_key__BACKSLASH;
	case VK_OEM_6:  //  ']}' for US
		return e_key__BRACKETRIGHT;
	case VK_OEM_7:  //  ''"' for US
		return e_key__APOSTROPHE;
/*
	case VK_OEM_8:
		return e_key__OEM_8;
	case VK_OEM_AX:  //  'AX' key on Japanese AX kbd
		return e_key__OEM_AX;
	case VK_OEM_102:  //  "<>" or "\|" on RT 102-key kbd.
		return e_key__OEM_102;
	case VK_ICO_HELP:  //  Help key on ICO
		return e_key__OEM_HELP;
	case VK_ICO_00:  //  00 key on ICO
		return e_key__OEM_00;
	case VK_PROCESSKEY:
		return e_key__PROCESSKEY;
	case VK_ICO_CLEAR:
		return e_key__ICO_CLEAR;
	case VK_PACKET:
		return e_key__PACKET;
	case VK_OEM_RESET:
		return e_key__OEM_RESET;
	case VK_OEM_JUMP:
		return e_key__OEM_JUMP;
	case VK_OEM_PA1:
		return e_key__OEM_PA1;
	case VK_OEM_PA2:
		return e_key__OEM_PA2;
	case VK_OEM_PA3:
		return e_key__OEM_PA3;
	case VK_OEM_WSCTRL:
		return e_key__OEM_WSCTRL;
	case VK_OEM_CUSEL:
		return e_key__OEM_CUSEL;
	case VK_OEM_ATTN:
		return e_key__OEM_ATTN;
	case VK_OEM_FINISH:
		return e_key__OEM_FINISH;
	case VK_OEM_COPY:
		return e_key__OEM_COPY;
	case VK_OEM_AUTO:
		return e_key__OEM_AUTO;
	case VK_OEM_ENLW:
		return e_key__OEM_ENLW;
	case VK_OEM_BACKTAB:
		return e_key__OEM_BACKTAB;
	case VK_ATTN:
		return e_key__ATTN;
	case VK_CRSEL:
		return e_key__CRSEL;
	case VK_EXSEL:
		return e_key__EXSEL;
	case VK_EREOF:
		return e_key__EREOF;
	case VK_PLAY:
		return e_key__PLAY;
	case VK_ZOOM:
		return e_key__ZOOM;
	case VK_NONAME:
		return e_key__NONAME;
	case VK_PA1:
		return e_key__PA1;
	case VK_OEM_CLEAR:
		return e_key__OEM_CLEAR;
*/
	default:
		return e_key__NONE;
	}
}
#endif

}

#endif
