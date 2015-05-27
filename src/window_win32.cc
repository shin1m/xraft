#include <xraft/window.h>

#include <xraft/graphics.h>

namespace xraft
{

int t_widget::v_delta;

LRESULT CALLBACK t_window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	t_window* w = reinterpret_cast<t_window*>(GetWindowLong(hwnd, 0));
	return w ? w->OnMessage(uMsg, wParam, lParam) : DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void t_window::f_create(HWND a_parent, ATOM a_atom, DWORD dwExStyle, DWORD dwStyle)
{
	v_handle = CreateWindowEx(dwExStyle, reinterpret_cast<LPCTSTR>(a_atom), NULL, dwStyle, v_geometry.v_x, v_geometry.v_y, v_geometry.v_width, v_geometry.v_height, a_parent, NULL, f_application()->v_hinstance, NULL);
	SetWindowLong(f_hwnd(), 0, reinterpret_cast<LONG>(this));
	ImmAssociateContext(f_hwnd(), v_input_context ? v_input_context->v_himc : NULL);
	for (const auto& p : v_children) p->f_create();
}

t_window::t_window() : v_geometry(0, 0, 0, 0), v_visible(false), v_cursor(f_application()->f_cursor_arrow())
{
}

t_window::~t_window()
{
	for (const auto& p : v_children) p->v_parent = 0;
}

void t_window::f_destroy()
{
	if (this == f_application()->v_pointed) f_application()->f_pointed__(0, 0, 0, e_cross_mode__NORMAL);
	if (this == f_application()->f_focus()) f_application()->f_focus__(0);
	for (const auto& p : v_children) p->f_destroy();
	SetWindowLong(f_hwnd(), 0, 0);
	DestroyWindow(f_hwnd());
	v_handle = NULL;
}

LRESULT t_window::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(f_hwnd(), &ps);
			{
				t_graphics g(this, hdc);
				f_on_paint(g);
			}
			EndPaint(f_hwnd(), &ps);
			return 0;
		}
	case WM_LBUTTONDOWN:
		SetCapture(f_hwnd());
		f_application()->f_pointed__(this, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), e_cross_mode__GRAB);
		f_on_button_press(f_modifier(), e_button__1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_LBUTTONUP:
		f_on_button_release(f_modifier(), e_button__1, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		ReleaseCapture();
		return 0;
	case WM_MBUTTONDOWN:
		SetCapture(f_hwnd());
		f_application()->f_pointed__(this, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), e_cross_mode__GRAB);
		f_on_button_press(f_modifier(), e_button__2, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MBUTTONUP:
		f_on_button_release(f_modifier(), e_button__2, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		ReleaseCapture();
		return 0;
	case WM_RBUTTONDOWN:
		SetCapture(f_hwnd());
		f_application()->f_pointed__(this, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), e_cross_mode__GRAB);
		f_on_button_press(f_modifier(), e_button__3, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_RBUTTONUP:
		f_on_button_release(f_modifier(), e_button__3, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		ReleaseCapture();
		return 0;
	case WM_MOUSEMOVE:
		f_application()->f_pointed__(this, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), e_cross_mode__NORMAL);
		f_on_pointer_move(f_modifier(), GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSELEAVE:
		if (this == f_application()->v_pointed) f_application()->f_pointed__(0, 0, 0, e_cross_mode__NORMAL);
		return 0;
	case WM_MOUSEWHEEL:
		v_delta += static_cast<short>(HIWORD(wParam));
		{
			t_modifier modifier = f_modifier();
			int n = v_delta / WHEEL_DELTA;
			v_delta -= n * WHEEL_DELTA;
			t_button button = n > 0 ? e_button__4 : e_button__5;
			if (n < 0) n = -n;
			while (n > 0) {
				f_on_button_press(modifier, button, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				f_on_button_release(modifier, button, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				--n;
			}
		}
		return 0;
	case WM_CAPTURECHANGED:
		f_application()->f_pointed__(0, 0, 0, e_cross_mode__UNGRAB);
		return 0;
	case WM_SETFOCUS:
		f_on_focus_enter();
		return 0;
	case WM_KILLFOCUS:
		f_on_focus_leave();
		return 0;
	case WM_SETCURSOR:
		if (LOWORD(lParam) == HTCLIENT) {
			SetCursor(reinterpret_cast<HCURSOR>(v_cursor));
			return TRUE;
		} else {
			return DefWindowProc(f_hwnd(), uMsg, wParam, lParam);
		}
	case WM_KEYDOWN:
		f_on_key_press(f_modifier(), f_win32_key(wParam), f_ascii(wParam));
		return 0;
	case WM_KEYUP:
		f_on_key_release(f_modifier(), f_win32_key(wParam), f_ascii(wParam));
		return 0;
	case WM_IME_STARTCOMPOSITION:
		return 0;
	case WM_IME_ENDCOMPOSITION:
		return 0;
	case WM_IME_COMPOSITION:
		if (lParam == 0) f_on_input_compose();
		if (lParam & GCS_RESULTSTR) {
			HIMC himc = ImmGetContext(f_hwnd());
			LONG n = ImmGetCompositionString(himc, GCS_RESULTSTR, NULL, 0) / sizeof(wchar_t);
			std::vector<wchar_t> cs(n);
			ImmGetCompositionString(himc, GCS_RESULTSTR, &cs[0], n * sizeof(wchar_t));
			ImmReleaseContext(f_hwnd(), himc);
			f_on_input_commit(&cs[0], n);
		}
		if (lParam & GCS_COMPSTR) f_on_input_compose();
		return 0;
	case WM_IME_NOTIFY:
		switch (wParam) {
		case IMN_OPENCANDIDATE:
			{
				t_rectangle spot = f_on_input_spot();
				CANDIDATEFORM candidate;
				candidate.dwIndex = 0;
				candidate.dwStyle = CFS_EXCLUDE;
				candidate.ptCurrentPos.x = spot.v_x;
				candidate.ptCurrentPos.y = spot.v_y + static_cast<int>(spot.v_height);
				candidate.rcArea.left = spot.v_x;
				candidate.rcArea.top = spot.v_y;
				candidate.rcArea.right = spot.v_x + static_cast<int>(spot.v_width);
				candidate.rcArea.bottom = spot.v_y + static_cast<int>(spot.v_height);
				HIMC himc = ImmGetContext(f_hwnd());
				ImmSetCandidateWindow(himc, &candidate);
				ImmReleaseContext(f_hwnd(), himc);
			}
		}
		return DefWindowProc(f_hwnd(), uMsg, wParam, lParam);
	default:
		return DefWindowProc(f_hwnd(), uMsg, wParam, lParam);
	}
}

void t_window::f_on_move()
{
}

void t_window::f_on_show()
{
}

void t_window::f_on_hide()
{
}

void t_window::f_on_paint(t_graphics& a_g)
{
}

void t_window::f_on_focus_enter()
{
}

void t_window::f_on_focus_leave()
{
}

void t_window::f_on_key_press(t_modifier a_modifier, t_key a_key, char a_ascii)
{
}

void t_window::f_on_key_release(t_modifier a_modifier, t_key a_key, char a_ascii)
{
}

void t_window::f_on_input_compose()
{
}

void t_window::f_on_input_commit(const wchar_t* a_cs, size_t a_n)
{
}

t_rectangle t_window::f_on_input_spot()
{
	return t_rectangle(0, 0, 0, f_application()->f_font()->f_height());
}

void t_window::f_on_button_press(t_modifier a_modifier, t_button a_button, int a_x, int a_y)
{
}

void t_window::f_on_button_release(t_modifier a_modifier, t_button a_button, int a_x, int a_y)
{
}

void t_window::f_on_pointer_enter(t_modifier a_modifier, int a_x, int a_y, t_cross_mode a_mode, t_cross_detail a_detail)
{
}

void t_window::f_on_pointer_leave(t_modifier a_modifier, int a_x, int a_y, t_cross_mode a_mode, t_cross_detail a_detail)
{
}

void t_window::f_on_pointer_move(t_modifier a_modifier, int a_x, int a_y)
{
}

void t_window::f_add(const t_pointer<t_widget>& a_widget, size_t a_i)
{
	if (a_i == size_t(-1)) a_i = v_children.size();
	if (a_widget->v_parent) {
		std::vector<t_pointer<t_widget>>& children = a_widget->v_parent->v_children;
		size_t i = 0;
		while (children[i] != a_widget) ++i;
		children.erase(children.begin() + i);
		if (a_widget->v_parent == this && a_i > i) --a_i;
	}
	if (a_widget->v_parent != this) {
		a_widget->v_parent = this;
		if (a_widget->v_handle == NULL) {
			if (v_handle != NULL) a_widget->f_create();
		} else if (v_handle == NULL) {
			a_widget->f_destroy();
		} else {
			SetParent(a_widget->f_hwnd(), f_hwnd());
		}
	}
	if (v_handle != NULL) SetWindowPos(a_widget->f_hwnd(), a_i > 0 ? v_children[a_i - 1]->f_hwnd() : HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	v_children.insert(v_children.begin() + a_i, a_widget);
}

void t_window::f_remove(size_t a_i)
{
	auto i = v_children.begin() + a_i;
	const t_pointer<t_widget>& widget = *i;
	if (v_handle != NULL) widget->f_destroy();
	widget->v_parent = 0;
	v_children.erase(i);
}

void t_window::f_invalidate(int a_x, int a_y, unsigned a_width, unsigned a_height)
{
	if (v_handle == NULL) return;
	RECT rect;
	rect.left = a_x;
	rect.top = a_y;
	rect.right = a_x + static_cast<int>(a_width);
	rect.bottom = a_y + static_cast<int>(a_height);
	if (rect.right < 0 || rect.bottom < 0) return;
	if (rect.left < 0) rect.left = 0;
	if (rect.top < 0) rect.top = 0;
	if (rect.left > static_cast<int>(v_geometry.v_width) || rect.top > static_cast<int>(v_geometry.v_height)) return;
	if (rect.right > static_cast<int>(v_geometry.v_width)) rect.right = v_geometry.v_width;
	if (rect.bottom > static_cast<int>(v_geometry.v_height)) rect.bottom = v_geometry.v_height;
	InvalidateRect(f_hwnd(), &rect, FALSE);
}

void t_window::f_scroll(int a_x, int a_y, unsigned a_width, unsigned a_height, int a_dx, int a_dy)
{
	if (v_handle == NULL) return;
	if ((a_dx < 0 ? -a_dx : a_dx) >= static_cast<int>(a_width) || (a_dy < 0 ? -a_dy : a_dy) >= static_cast<int>(a_height)) {
		f_invalidate(a_x, a_y, a_width, a_height);
		return;
	}
	RECT rect;
	rect.left = a_x;
	rect.top = a_y;
	rect.right = a_x + static_cast<int>(a_width);
	rect.bottom = a_y + static_cast<int>(a_height);
	if (rect.right < 0 || rect.bottom < 0) return;
	if (rect.left < 0) rect.left = 0;
	if (rect.top < 0) rect.top = 0;
	if (rect.left > static_cast<int>(v_geometry.v_width) || rect.top > static_cast<int>(v_geometry.v_height)) return;
	if (rect.right > static_cast<int>(v_geometry.v_width)) rect.right = v_geometry.v_width;
	if (rect.bottom > static_cast<int>(v_geometry.v_height)) rect.bottom = v_geometry.v_height;
	ScrollWindowEx(f_hwnd(), a_dx, a_dy, &rect, &rect, NULL, NULL, SW_INVALIDATE);
}

void t_window::f_input_context__(const t_pointer<t_input_context>& a_input_context)
{
	if (a_input_context == v_input_context) return;
	v_input_context = a_input_context;
	if (v_handle != NULL) ImmAssociateContext(f_hwnd(), v_input_context ? v_input_context->v_himc : NULL);
}

void t_widget::f_create()
{
	t_window::f_create(v_parent->f_hwnd(), f_application()->v_atom_window, 0, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS);
}

void t_shell::f_create()
{
	t_window::f_create(NULL, f_application()->v_atom_window, WS_EX_WINDOWEDGE | WS_EX_ACCEPTFILES, WS_POPUP);
}

LRESULT t_shell::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_ACTIVATE:
		if (wParam == WA_INACTIVE)
			f_on_deactivate();
		else
			f_on_activate();
		return 0;
	case WM_DROPFILES:
		return OnDropFiles(reinterpret_cast<HDROP>(wParam));
	default:
		return t_window::OnMessage(uMsg, wParam, lParam);
	}
}

LRESULT t_shell::OnDropFiles(HDROP hDrop)
{
	SetForegroundWindow(f_hwnd());
	UINT cch = DragQueryFile(hDrop, 0, NULL, 0) + 1;
	LPWSTR lpszFile = new WCHAR[cch];
	cch = DragQueryFile(hDrop, 0, lpszFile, cch);
	delete[] lpszFile;
	DragFinish(hDrop);
	return 0;
}

void t_shell::f_on_activate()
{
}

void t_shell::f_on_deactivate()
{
}

void t_frame::f_create()
{
	t_window::f_create(NULL, f_application()->v_atom_window, WS_EX_WINDOWEDGE | WS_EX_ACCEPTFILES, WS_OVERLAPPEDWINDOW);
	SendMessage(f_hwnd(), WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(LoadIcon(f_application()->v_hinstance, L"IDI_XRAFT__FRAME_WIDGET")));
	SetWindowText(f_hwnd(), v_caption.c_str());
}

LRESULT t_frame::OnMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CLOSE:
		f_on_close();
		return 0;
	case WM_SIZE:
		if (wParam != SIZE_MINIMIZED) {
			POINT point = {0, 0};
			ClientToScreen(f_hwnd(), &point);
			RECT rect;
			GetClientRect(f_hwnd(), &rect);
			v_geometry = t_rectangle(point.x, point.y, rect.right, rect.bottom);
			f_on_move();
		}
		return 0;
	default:
		return t_shell::OnMessage(uMsg, wParam, lParam);
	}
}

void t_frame::f_on_close()
{
}

void t_frame::f_caption__(const std::wstring& a_caption)
{
	v_caption = a_caption;
	if (v_handle != NULL) SetWindowText(f_hwnd(), v_caption.c_str());
}

}
