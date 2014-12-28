#include <xraft/application.h>

#include <cstdlib>
#include <winsock2.h>
#include <xraft/graphics.h>

namespace xraft
{

t_application* t_application::v_instance;

LRESULT CALLBACK t_application::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_TIMER:
		{
			t_timer& timer = *reinterpret_cast<t_timer*>(wParam);
			if (timer.v_single) timer.f_stop();
			timer();
		}
		return 0;
	case WM_USER:
		(*reinterpret_cast<t_runnable*>(wParam))();
		return 0;
	default:
		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}

namespace
{

void f_ancestors(const t_pointer<t_window>& a_window, std::vector<t_pointer<t_window> >& a_ancestors)
{
	t_pointer<t_widget> widget = f_dynamic_cast<t_widget>(a_window);
	while (widget) {
		t_pointer<t_window> p = widget->f_parent();
		a_ancestors.push_back(p);
		widget = f_dynamic_cast<t_widget>(p);
	}
}

}

void t_application::f_pointed__(const t_pointer<t_window>& a_window, int a_x, int a_y, t_cross_mode a_mode)
{
	if (a_window == v_pointed) return;
	t_pointer<t_window> pointed = v_pointed;
	v_pointed = a_window;
	std::vector<t_pointer<t_window> > ancestors0;
	f_ancestors(pointed, ancestors0);
	std::vector<t_pointer<t_window> > ancestors1;
	f_ancestors(v_pointed, ancestors1);
	size_t i = 0;
	while (i < ancestors0.size() && i < ancestors1.size() && ancestors0[i] == ancestors1[i]) ++i;
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(TRACKMOUSEEVENT);
	tme.dwHoverTime = 0;
	t_modifier modifier = t_window::f_modifier();
	if (pointed) {
		tme.dwFlags = TME_LEAVE | TME_CANCEL;
		tme.hwndTrack = pointed->f_hwnd();
		TrackMouseEvent(&tme);
		if (i < ancestors1.size() && ancestors1[i] == pointed) {
			pointed->f_on_pointer_leave(modifier, 0, 0, a_mode, e_cross_detail__INNER);
			++i;
		} else {
			pointed->f_on_pointer_leave(modifier, 0, 0, a_mode, e_cross_detail__OUTER);
			size_t j = ancestors0.size();
			if (i < j && ancestors0[i] == v_pointed) ++i;
			while (j > i) ancestors0[--j]->f_on_pointer_leave(modifier, 0, 0, a_mode, e_cross_detail__THROUGH);
		}
	}
	if (v_pointed) {
		tme.dwFlags = TME_LEAVE;
		tme.hwndTrack = v_pointed->f_hwnd();
		TrackMouseEvent(&tme);
		if (i > ancestors1.size()) {
			v_pointed->f_on_pointer_enter(modifier, a_x, a_y, a_mode, e_cross_detail__INNER);
		} else {
			for (size_t j = i; j < ancestors1.size(); ++j) ancestors1[j]->f_on_pointer_enter(modifier, 0, 0, a_mode, e_cross_detail__THROUGH);
			v_pointed->f_on_pointer_enter(modifier, a_x, a_y, a_mode, e_cross_detail__OUTER);
		}
	}
}

void t_application::f_wait()
{
	v_loop = GetMessage(&v_msg, NULL, 0, 0);
}

t_application::t_application(std::vector<std::string>& a_arguments) : v_pointed(0)
{
	if (v_instance) throw std::runtime_error("application instance already exists.");
	v_hinstance = GetModuleHandle(NULL);
	WSADATA wsadata;
	v_wsa = WSAStartup(MAKEWORD(1, 1), &wsadata);
	WNDCLASSEX wcx;
	wcx.cbSize = sizeof(WNDCLASSEX);
	wcx.style = 0;
	wcx.lpfnWndProc = WindowProc;
	wcx.cbClsExtra = 0;
	wcx.cbWndExtra = 0;
	wcx.hInstance = v_hinstance;
	wcx.hIcon = NULL;
	wcx.hCursor = NULL;
	wcx.hbrBackground = NULL;
	wcx.lpszMenuName = NULL;
	wcx.lpszClassName = L"xraft::t_application";
	wcx.hIconSm = NULL;
	v_atom_application = RegisterClassEx(&wcx);
	wcx.lpfnWndProc = t_window::WindowProc;
	wcx.cbWndExtra = sizeof(LONG);
	wcx.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcx.lpszClassName = L"xraft::t_window";
	v_atom_window = RegisterClassEx(&wcx);
	wcx.style = CS_OWNDC;
	wcx.lpszClassName = L"xraft::t_window_with_dc";
	v_atom_window_with_dc = RegisterClassEx(&wcx);
	v_hwnd = CreateWindow(reinterpret_cast<LPCTSTR>(v_atom_application), NULL, 0, 0, 0, 0, 0, NULL, NULL, v_hinstance, NULL);
	v_hdc = CreateCompatibleDC(NULL);
	v_cursor_arrow = reinterpret_cast<t_cursor>(LoadCursor(NULL, IDC_ARROW));
	v_cursor_ibeam = reinterpret_cast<t_cursor>(LoadCursor(NULL, IDC_IBEAM));
	v_cursor_vslide = reinterpret_cast<t_cursor>(LoadCursor(NULL, IDC_SIZENS));
	v_cursor_hslide = reinterpret_cast<t_cursor>(LoadCursor(NULL, IDC_SIZEWE));
	v_instance = this;
	v_font = new t_font("", 12, false, false, false);
	v_input_context = new t_input_context();
}

t_application::~t_application()
{
	for (const auto& p : v_shells) p->f_destroy();
	v_shells.clear();
	v_input_context = 0;
	v_font = 0;
	v_instance = 0;
	DeleteDC(v_hdc);
	DestroyWindow(v_hwnd);
	if (v_wsa == 0) WSACleanup();
}

void t_application::f_run()
{
	while (true) {
		f_wait();
		if (v_loop == 0 || v_loop == -1) break;
		TranslateMessage(&v_msg);
		DispatchMessage(&v_msg);
	}
}

void t_application::f_pointer_grabber__(const t_pointer<t_window>& a_window)
{
	if (a_window) {
		SetCapture(a_window->f_hwnd());
		t_point p = a_window->f_from_screen(f_pointer());
		f_pointed__(a_window, p.v_x, p.v_y, e_cross_mode__GRAB);
	} else {
		SetCapture(NULL);
	}
}

void t_application::f_add(const t_pointer<t_shell>& a_shell, size_t a_i)
{
	if (a_i == size_t(-1)) a_i = v_shells.size();
	if (a_shell->v_handle != NULL) {
		size_t i = 0;
		while (v_shells[i] != a_shell) ++i;
		v_shells.erase(v_shells.begin() + i);
		if (a_i > i) --a_i;
	} else {
		a_shell->f_create();
	}
	SetWindowPos(a_shell->f_hwnd(), a_i > 0 ? v_shells[a_i - 1]->f_hwnd() : HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	v_shells.insert(v_shells.begin() + a_i, a_shell);
}

void t_application::f_remove(size_t a_i)
{
	auto i = v_shells.begin() + a_i;
	(*i)->f_destroy();
	v_shells.erase(i);
}

}
