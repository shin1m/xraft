#include "ui.h"

#include "context.h"

LRESULT CALLBACK t_component::f_process(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	t_component* component = reinterpret_cast<t_component*>(GetWindowLongPtr(hWnd, 0));
	switch (message) {
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);
			component->f_on_paint(hdc);
			EndPaint(hWnd, &ps);
			return 0;
		}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

void t_component::f_on_create(HWND a_parent, int a_x, int a_y, size_t a_width, size_t a_height)
{
	v_hwnd = CreateWindowEx(WS_EX_NOACTIVATE | WS_EX_NOPARENTNOTIFY | WS_EX_TOPMOST, f_class(), NULL, WS_DISABLED | WS_POPUP, a_x, a_y, a_width, a_height, a_parent, NULL, v_hinstance, NULL);
}

BOOL t_component::f_register()
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_IME;
	wc.lpfnWndProc = f_process;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(LONG_PTR);
	wc.hInstance = v_hinstance;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = f_class();
	wc.hIconSm = NULL;
	return RegisterClassEx(&wc);
}

void t_component::f_show(t_ui* a_ui, int a_x, int a_y, size_t a_width, size_t a_height)
{
	if (v_hwnd == NULL) {
		f_on_create(a_ui->v_hwnd, a_x, a_y, a_width, a_height);
		SetWindowLongPtr(v_hwnd, 0, reinterpret_cast<LONG_PTR>(this));
		ShowWindow(v_hwnd, SW_SHOWNOACTIVATE);
	} else {
		MoveWindow(v_hwnd, a_x, a_y, a_width, a_height, TRUE);
	}
	InvalidateRect(v_hwnd, NULL, FALSE);
}

void t_component::f_hide()
{
	if (v_hwnd != NULL) {
		DestroyWindow(v_hwnd);
		v_hwnd = NULL;
	}
}

void t_composition::f_on_paint(HDC a_hdc)
{
	HWND parent = GetParent(v_hwnd);
	t_imc imc(reinterpret_cast<HIMC>(GetWindowLongPtr(parent, IMMGWL_IMC)));
	t_ui* ui = reinterpret_cast<t_ui*>(GetWindowLongPtr(parent, IMMGWLP_PRIVATE));
	t_imcc<COMPOSITIONSTRING> composition(imc->hCompStr);
	const WCHAR* cs = reinterpret_cast<WCHAR*>(reinterpret_cast<char*>(static_cast<LPCOMPOSITIONSTRING>(composition)) + composition->dwCompStrOffset);
	const BYTE* as = reinterpret_cast<BYTE*>(static_cast<LPCOMPOSITIONSTRING>(composition)) + composition->dwCompAttrOffset;
	RECT rect;
	GetClientRect(v_hwnd, &rect);
	HGDIOBJ hfont = SelectObject(a_hdc, ui->v_font);
	TEXTMETRIC tm;
	GetTextMetrics(a_hdc, &tm);
	const COLORREF colors[6][3] = {
		{GetSysColor(COLOR_GRAYTEXT), GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_WINDOWTEXT)},
		{GetSysColor(COLOR_HIGHLIGHTTEXT), GetSysColor(COLOR_HIGHLIGHT), GetSysColor(COLOR_WINDOWTEXT)},
		{GetSysColor(COLOR_WINDOWTEXT), GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_WINDOWTEXT)},
		{GetSysColor(COLOR_HIGHLIGHTTEXT), GetSysColor(COLOR_HIGHLIGHT), GetSysColor(COLOR_WINDOWTEXT)},
		{GetSysColor(COLOR_HIGHLIGHTTEXT), GetSysColor(COLOR_HIGHLIGHT), GetSysColor(COLOR_WINDOWTEXT)},
		{GetSysColor(COLOR_WINDOWTEXT), GetSysColor(COLOR_WINDOW), GetSysColor(COLOR_WINDOWTEXT)},
	};
	int y = 0;
	for (size_t i = v_first; i < v_last; ++i) {
		const t_ui::t_line& line = ui->v_lines[i];
		const WCHAR* p = cs + line.v_i;
		const BYTE* q = as + line.v_i;
		const BYTE* r = as + line.v_j;
		RECT u;
		u.left = 0;
		u.top = y + tm.tmHeight - 1;
		u.bottom = u.top + 1;
		while (q < r) {
			BYTE a = *q;
			size_t n = 0;
			do ++n; while (++q < r && *q == a);
			if (a > ATTR_FIXEDCONVERTED) a = ATTR_INPUT_ERROR;
			const COLORREF* c = colors[a];
			SetTextColor(a_hdc, c[0]);
			SetBkColor(a_hdc, c[1]);
			TextOut(a_hdc, u.left, y, p, n);
			SIZE size;
			GetTextExtentPoint32(a_hdc, p, n, &size);
			u.right = u.left + size.cx;
			SetBkColor(a_hdc, c[2]);
			ExtTextOut(a_hdc, 0, 0, ETO_OPAQUE, &u, NULL, 0, NULL);
			u.left = u.right;
			p += n;
		}
		u.top = y;
		u.right = rect.right;
		y += tm.tmHeight;
		u.bottom = y;
		SetBkColor(a_hdc, RGB(255, 255, 255));
		ExtTextOut(a_hdc, 0, 0, ETO_OPAQUE, &u, NULL, 0, NULL);
	}
	SelectObject(a_hdc, hfont);
}

void t_candidates::f_on_paint(HDC a_hdc)
{
	HWND parent = GetParent(v_hwnd);
	t_imc imc(reinterpret_cast<HIMC>(GetWindowLongPtr(parent, IMMGWL_IMC)));
	t_ui* ui = reinterpret_cast<t_ui*>(GetWindowLongPtr(parent, IMMGWLP_PRIVATE));
	t_imcc<CANDIDATEINFO> candidates(imc->hCandInfo);
	LPCANDIDATELIST list = reinterpret_cast<LPCANDIDATELIST>(reinterpret_cast<char*>(static_cast<LPCANDIDATEINFO>(candidates)) + candidates->dwOffset[0]);
	RECT rect;
	GetClientRect(v_hwnd, &rect);
	HGDIOBJ hfont = SelectObject(a_hdc, GetStockObject(SYSTEM_FIXED_FONT));
	DrawEdge(a_hdc, &rect, BDR_RAISEDOUTER, BF_RECT | BF_FLAT);
	++rect.left;
	--rect.right;
	{
		t_imcc<COMPOSITIONSTRING> composition(imc->hCompStr);
		std::wstring s(reinterpret_cast<WCHAR*>(reinterpret_cast<char*>(static_cast<LPCOMPOSITIONSTRING>(composition)) + composition->dwCompReadStrOffset), composition->dwCompReadStrLen);
		wchar_t cs[32];
		std::swprintf(cs, 32, L" %d/%d", list->dwSelection + 1, list->dwCount);
		s += cs;
		SIZE size;
		GetTextExtentPoint32(a_hdc, s.c_str(), s.size(), &size);
		RECT r = rect;
		++r.top;
		rect.top = r.bottom = r.top + size.cy + 4;
		SetTextColor(a_hdc, GetSysColor(COLOR_MENUTEXT));
		SetBkColor(a_hdc, GetSysColor(COLOR_3DFACE));
		ExtTextOut(a_hdc, r.left + 3, r.top + 2, ETO_OPAQUE, &r, s.c_str(), s.size(), NULL);
	}
	--rect.bottom;
	HGDIOBJ hpen = SelectObject(a_hdc, GetStockObject(WHITE_PEN));
	Rectangle(a_hdc, rect.left, rect.top, rect.right, rect.bottom);
	SelectObject(a_hdc, hpen);
	++rect.left;
	++rect.top;
	--rect.right;
	for (DWORD i = 0; i < list->dwPageSize; ++i) {
		DWORD j = list->dwPageStart + i;
		const WCHAR* cs = reinterpret_cast<WCHAR*>(reinterpret_cast<char*>(list) + list->dwOffset[j]);
		DWORD n = ((j + 1 < list->dwCount ? list->dwOffset[j + 1] : list->dwSize) - list->dwOffset[j]) / sizeof(WCHAR);
		SIZE size;
		GetTextExtentPoint32(a_hdc, cs, n, &size);
		rect.bottom = rect.top + size.cy + 4;
		if (j == list->dwSelection) {
			SetTextColor(a_hdc, GetSysColor(COLOR_HIGHLIGHTTEXT));
			SetBkColor(a_hdc, GetSysColor(COLOR_HIGHLIGHT));
		} else {
			SetTextColor(a_hdc, GetSysColor(COLOR_WINDOWTEXT));
			SetBkColor(a_hdc, GetSysColor(COLOR_WINDOW));
		}
		ExtTextOut(a_hdc, rect.left + 2, rect.top + 2, ETO_OPAQUE, &rect, cs, n, NULL);
		rect.top = rect.bottom;
	}
	SelectObject(a_hdc, hfont);
}

void t_guide::f_on_create(HWND a_parent, int a_x, int a_y, size_t a_width, size_t a_height)
{
	v_hwnd = CreateWindowEx(WS_EX_LAYERED | WS_EX_NOACTIVATE | WS_EX_NOPARENTNOTIFY | WS_EX_TOPMOST | WS_EX_TRANSPARENT, f_class(), NULL, WS_DISABLED | WS_POPUP, a_x, a_y, a_width, a_height, a_parent, NULL, v_hinstance, NULL);
	SetLayeredWindowAttributes(v_hwnd, 0, 191, LWA_ALPHA);
}

void t_guide::f_on_paint(HDC a_hdc)
{
	HWND parent = GetParent(v_hwnd);
	t_imc imc(reinterpret_cast<HIMC>(GetWindowLongPtr(parent, IMMGWL_IMC)));
	t_ui* ui = reinterpret_cast<t_ui*>(GetWindowLongPtr(parent, IMMGWLP_PRIVATE));
	t_imcc<GUIDELINE> guide(imc->hGuideLine);
	DWORD n = guide->dwStrLen;
	const WCHAR* cs = reinterpret_cast<WCHAR*>(reinterpret_cast<char*>(static_cast<LPGUIDELINE>(guide)) + guide->dwStrOffset);
	RECT rect;
	GetClientRect(v_hwnd, &rect);
	HGDIOBJ hfont = SelectObject(a_hdc, GetStockObject(SYSTEM_FIXED_FONT));
	DrawEdge(a_hdc, &rect, BDR_RAISEDOUTER, BF_RECT | BF_FLAT);
	rect.left += 1;
	rect.top += 1;
	rect.right -= 1;
	rect.bottom -= 1;
	SetTextColor(a_hdc, GetSysColor(COLOR_MENUTEXT));
	SetBkColor(a_hdc, GetSysColor(COLOR_3DFACE));
	ExtTextOut(a_hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
	rect.left += 1;
	rect.top += 1;
	rect.right -= 1;
	rect.bottom -= 1;
	DrawText(a_hdc, cs, n, &rect, DT_LEFT | DT_NOPREFIX);
	SelectObject(a_hdc, hfont);
}

LRESULT CALLBACK t_ui::f_process(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	t_ui* ui = reinterpret_cast<t_ui*>(GetWindowLongPtr(hWnd, IMMGWLP_PRIVATE));
	switch (message) {
	case WM_CREATE:
f_log("WM_CREATE");
		SetWindowLongPtr(hWnd, IMMGWLP_PRIVATE, reinterpret_cast<LONG_PTR>(new t_ui(hWnd)));
		return 0;
	case WM_DESTROY:
f_log("WM_DESTROY");
		delete ui;
		return 0;
	case WM_IME_SETCONTEXT:
f_log("WM_IME_SETCONTEXT");
		ui->v_active = wParam != 0;
		SetTimer(hWnd, 0, 0, NULL);
		return 0;
	case WM_IME_STARTCOMPOSITION:
f_log("WM_IME_STARTCOMPOSITION");
		ui->v_composing = true;
		SetTimer(hWnd, 0, 0, NULL);
		return 0;
	case WM_IME_COMPOSITION:
f_log("WM_IME_COMPOSITION");
		SetTimer(hWnd, 0, 0, NULL);
		return 0;
	case WM_IME_ENDCOMPOSITION:
f_log("WM_IME_ENDCOMPOSITION");
		ui->v_composing = false;
		SetTimer(hWnd, 0, 0, NULL);
		return 0;
	case WM_IME_COMPOSITIONFULL:
f_log("WM_IME_COMPOSITIONFULL");
		return 0;
	case WM_IME_SELECT:
f_log("WM_IME_SELECT");
		if (wParam) ui->f_font();
		ui->v_active = wParam != 0;
		SetTimer(hWnd, 0, 0, NULL);
		return 0;
	case WM_IME_CHAR:
f_log("WM_IME_CHAR");
		return 0;
	case WM_IME_CONTROL:
f_log("WM_IME_CONTROL");
		switch (wParam) {
		case IMC_GETCANDIDATEPOS:
f_log("\tIMC_GETCANDIDATEPOS");
			{
				HIMC himc = reinterpret_cast<HIMC>(GetWindowLongPtr(hWnd, IMMGWL_IMC));
				if (himc == NULL) return 1;
				t_imc imc(himc);
				*reinterpret_cast<LPCANDIDATEFORM>(lParam) = imc->cfCandForm[0];
				return 0;
			}
		case IMC_GETCOMPOSITIONWINDOW:
f_log("\tIMC_GETCOMPOSITIONWINDOW");
			{
				HIMC himc = reinterpret_cast<HIMC>(GetWindowLongPtr(hWnd, IMMGWL_IMC));
				if (himc == NULL) return 1;
				t_imc imc(himc);
				*reinterpret_cast<LPCOMPOSITIONFORM>(lParam) = imc->cfCompForm;
				return 0;
			}
		case IMC_GETSTATUSWINDOWPOS:
f_log("\tIMC_GETSTATUSWINDOWPOS");
			{
				union {
					POINTS point;
					LRESULT result;
				};
				HIMC himc = reinterpret_cast<HIMC>(GetWindowLongPtr(hWnd, IMMGWL_IMC));
				if (himc == NULL) {
					point.x = point.y = 0;
				} else {
					t_imc imc(himc);
					point.x = static_cast<SHORT>(imc->ptStatusWndPos.x);
					point.y = static_cast<SHORT>(imc->ptStatusWndPos.y);
				}
				return result;
			}
		default:
			return 0;
		}
	case WM_IME_NOTIFY:
f_log("WM_IME_NOTIFY");
		switch (wParam) {
		case IMN_CLOSESTATUSWINDOW:
f_log("\tIMN_CLOSESTATUSWINDOW");
			return 0;
		case IMN_OPENSTATUSWINDOW:
f_log("\tIMN_OPENSTATUSWINDOW");
			return 0;
		case IMN_SETCONVERSIONMODE:
f_log("\tIMN_SETCONVERSIONMODE");
			SetTimer(hWnd, 0, 0, NULL);
			return 0;
		case IMN_SETSENTENCEMODE:
f_log("\tIMN_SETSENTENCEMODE");
			SetTimer(hWnd, 0, 0, NULL);
			return 0;
		case IMN_SETOPENSTATUS:
f_log("\tIMN_SETOPENSTATUS");
			SetTimer(hWnd, 0, 0, NULL);
			return 0;
		case IMN_OPENCANDIDATE:
f_log("\tIMN_OPENCANDIDATE");
			if ((lParam & 1) != 0) {
				ui->v_choosing = true;
				SetTimer(hWnd, 0, 0, NULL);
			}
			return 0;
		case IMN_CHANGECANDIDATE:
f_log("\tIMN_CHANGECANDIDATE");
			if ((lParam & 1) != 0) SetTimer(hWnd, 0, 0, NULL);
			return 0;
		case IMN_CLOSECANDIDATE:
f_log("\tIMN_CLOSECANDIDATE");
			if ((lParam & 1) != 0) {
				ui->v_choosing = false;
				SetTimer(hWnd, 0, 0, NULL);
			}
			return 0;
		case IMN_GUIDELINE:
f_log("\tIMN_GUIDELINE");
			SetTimer(hWnd, 0, 0, NULL);
			return 0;
		case IMN_SETCANDIDATEPOS:
f_log("\tIMN_SETCANDIDATEPOS");
			if ((lParam & 1) != 0) SetTimer(hWnd, 0, 0, NULL);
			return 0;
		case IMN_SETCOMPOSITIONFONT:
f_log("\tIMN_SETCOMPOSITIONFONT");
			ui->f_font();
			SetTimer(hWnd, 0, 0, NULL);
			return 0;
		case IMN_SETCOMPOSITIONWINDOW:
f_log("\tIMN_SETCOMPOSITIONWINDOW");
			SetTimer(hWnd, 0, 0, NULL);
			return 0;
		case IMN_SETSTATUSWINDOWPOS:
f_log("\tIMN_SETSTATUSWINDOWPOS");
			return 0;
		case IMN_PRIVATE:
f_log("\tIMN_PRIVATE");
		default:
			return 0;
		}
	case WM_TIMER:
		KillTimer(hWnd, 0);
		ui->f_composition();
		ui->f_candidates();
		ui->f_guide();
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

POINT t_ui::f_spot(LONG a_width, LONG a_height) const
{
	t_imc imc(reinterpret_cast<HIMC>(GetWindowLongPtr(v_hwnd, IMMGWL_IMC)));
	GUITHREADINFO ti;
	ti.cbSize = sizeof(GUITHREADINFO);
	GetGUIThreadInfo(GetCurrentThreadId(), &ti);
	POINT spot;
	POINT lt;
	if (ti.hwndCaret == imc->hWnd) {
		spot.x = ti.rcCaret.left;
		spot.y = ti.rcCaret.bottom;
		lt.x = ti.rcCaret.left;
		lt.y = ti.rcCaret.top;
	} else {
		const COMPOSITIONFORM& form = imc->cfCompForm;
		switch (form.dwStyle) {
		case CFS_FORCE_POSITION:
		case CFS_POINT:
			{
				spot = lt = form.ptCurrentPos;
				HDC hdc = GetDC(imc->hWnd);
				HGDIOBJ hfont = SelectObject(hdc, v_font);
				TEXTMETRIC tm;
				GetTextMetrics(hdc, &tm);
				SelectObject(hdc, hfont);
				ReleaseDC(imc->hWnd, hdc);
				spot.y += tm.tmHeight;
			}
			break;
		default:
			{
				RECT rect;
				GetClientRect(imc->hWnd, &rect);
				spot.x = rect.left;
				spot.y = rect.bottom;
				lt.x = rect.left;
				lt.y = rect.top;
			}
		}
	}
	const CANDIDATEFORM& form = imc->cfCandForm[0];
	switch (form.dwStyle) {
	case CFS_CANDIDATEPOS:
		spot = form.ptCurrentPos;
		break;
	case CFS_EXCLUDE:
		if (ti.hwndCaret != imc->hWnd) spot = form.ptCurrentPos;
		if (spot.y >= form.rcArea.top && spot.y < form.rcArea.bottom) spot.y = form.rcArea.bottom;
		lt.x = form.rcArea.left;
		lt.y = form.rcArea.top;
		break;
	}
	ClientToScreen(imc->hWnd, &spot);
	ClientToScreen(imc->hWnd, &lt);
	MONITORINFO mi;
	mi.cbSize = sizeof(MONITORINFO);
	GetMonitorInfo(MonitorFromWindow(imc->hWnd, MONITOR_DEFAULTTONEAREST), &mi);
	if (spot.x + a_width > mi.rcWork.right) spot.x = mi.rcWork.right - a_width;
	if (spot.x < mi.rcWork.left) spot.x = mi.rcWork.left;
	if (spot.y + a_height > mi.rcWork.bottom) spot.y = std::min(mi.rcWork.bottom, lt.y) - a_height;
	if (spot.y < mi.rcWork.top) spot.y = mi.rcWork.top;
	return spot;
}

BOOL t_ui::f_register()
{
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_IME;
	wc.lpfnWndProc = f_process;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(LONG_PTR) * 2;
	wc.hInstance = v_hinstance;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = f_class();
	wc.hIconSm = NULL;
	return RegisterClassEx(&wc);
}

void t_ui::f_font()
{
	if (v_font != NULL) DeleteObject(v_font);
	t_imc imc(reinterpret_cast<HIMC>(GetWindowLongPtr(v_hwnd, IMMGWL_IMC)));
	v_font = CreateFontIndirect(&imc->lfFont.W);
}

void t_ui::f_composition()
{
	v_lines.clear();
	HIMC himc = reinterpret_cast<HIMC>(GetWindowLongPtr(v_hwnd, IMMGWL_IMC));
	if (himc == NULL || !v_active || !v_composing) {
		f_clear();
		return;
	}
	t_imc imc(himc);
	t_imcc<COMPOSITIONSTRING> composition(imc->hCompStr);
	DWORD n = composition->dwCompStrLen;
	if (n <= 0) {
		f_clear();
		return;
	}
	const COMPOSITIONFORM& form = imc->cfCompForm;
	POINT spot = form.ptCurrentPos;
	RECT area;
	if (form.dwStyle == CFS_RECT)
		area = form.rcArea;
	else
		GetClientRect(imc->hWnd, &area);
	ClientToScreen(imc->hWnd, &spot);
	{
		POINT point = {area.left, area.top};
		ClientToScreen(imc->hWnd, &point);
		area.left = point.x;
		area.top = point.y;
	}
	{
		POINT point = {area.right, area.bottom};
		ClientToScreen(imc->hWnd, &point);
		area.right = point.x;
		area.bottom = point.y;
	}
	HDC hdc = GetDC(v_hwnd);
	HGDIOBJ hfont = SelectObject(hdc, v_font);
	const WCHAR* cs = reinterpret_cast<WCHAR*>(reinterpret_cast<char*>(static_cast<LPCOMPOSITIONSTRING>(composition)) + composition->dwCompStrOffset);
	v_lines.push_back(t_line(0, spot.x));
	size_t i = 0;
	while (true) {
		t_line& line = v_lines.back();
		while (true) {
			if (cs[i] == L'\n') {
				line.v_j = i++;
				break;
			}
			int w;
			GetCharWidth(hdc, cs[i], cs[i], &w);
			if (line.v_x + w > area.right) {
				if (line.v_x <= area.left) {
					line.v_x += w;
					++i;
				}
				line.v_j = i;
				break;
			}
			line.v_x += w;
			if (++i >= n) {
				line.v_j = i;
				break;
			}
		}
		if (i >= n) break;
		v_lines.push_back(t_line(i, area.left));
	}
	TEXTMETRIC tm;
	GetTextMetrics(hdc, &tm);
	SelectObject(hdc, hfont);
	ReleaseDC(v_hwnd, hdc);
	if (v_lines[0].v_x - spot.x > 0)
		v_first.f_show(this, spot.x, spot.y, v_lines[0].v_x - spot.x, tm.tmHeight);
	else
		v_first.f_hide();
	if (v_lines.size() > 2) {
		v_middle.v_first = 1;
		v_middle.v_last = v_lines.size() - 1;
		v_middle.f_show(this, area.left, spot.y + tm.tmHeight, area.right - area.left, tm.tmHeight * (v_lines.size() - 2));
	} else {
		v_middle.f_hide();
	}
	if (v_lines.size() > 1 && v_lines.back().v_x - area.left > 0) {
		v_last.v_first  = v_lines.size() - 1;
		v_last.v_last  = v_lines.size();
		v_last.f_show(this, area.left, spot.y + tm.tmHeight * (v_lines.size() - 1), v_lines.back().v_x - area.left, tm.tmHeight);
	} else {
		v_last.f_hide();
	}
}

void t_ui::f_candidates()
{
	HIMC himc = reinterpret_cast<HIMC>(GetWindowLongPtr(v_hwnd, IMMGWL_IMC));
	if (himc == NULL || !v_active || !v_choosing) {
		v_candidates.f_hide();
		return;
	}
	t_imc imc(himc);
	t_imcc<CANDIDATEINFO> candidates(imc->hCandInfo);
	if (candidates->dwCount <= 0) {
		v_candidates.f_hide();
		return;
	}
	LPCANDIDATELIST list = reinterpret_cast<LPCANDIDATELIST>(reinterpret_cast<char*>(static_cast<LPCANDIDATEINFO>(candidates)) + candidates->dwOffset[0]);
	HDC hdc = GetDC(v_hwnd);
	HGDIOBJ hfont = SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
	SIZE size;
	{
		t_imcc<COMPOSITIONSTRING> composition(imc->hCompStr);
		std::wstring s(reinterpret_cast<WCHAR*>(reinterpret_cast<char*>(static_cast<LPCOMPOSITIONSTRING>(composition)) + composition->dwCompReadStrOffset), composition->dwCompReadStrLen);
		wchar_t cs[32];
		std::swprintf(cs, 32, L" %d/%d", list->dwSelection + 1, list->dwCount);
		s += cs;
		GetTextExtentPoint32(hdc, s.c_str(), s.size(), &size);
		size.cy += 4;
	}
	for (DWORD i = 0; i < list->dwPageSize; ++i) {
		DWORD j = list->dwPageStart + i;
		const WCHAR* cs = reinterpret_cast<WCHAR*>(reinterpret_cast<char*>(list) + list->dwOffset[j]);
		DWORD n = ((j + 1 < list->dwCount ? list->dwOffset[j + 1] : list->dwSize) - list->dwOffset[j]) / sizeof(WCHAR);
		SIZE s;
		GetTextExtentPoint32(hdc, cs, n, &s);
		if (s.cx > size.cx) size.cx = s.cx;
		size.cy += s.cy + 4;
	}
	size.cx += 8;
	size.cy += 4;
	SelectObject(hdc, hfont);
	ReleaseDC(v_hwnd, hdc);
	POINT spot = f_spot(size.cx, size.cy);
	v_candidates.f_show(this, spot.x, spot.y, size.cx, size.cy);
}

void t_ui::f_guide()
{
	HIMC himc = reinterpret_cast<HIMC>(GetWindowLongPtr(v_hwnd, IMMGWL_IMC));
	if (himc == NULL || !v_active) {
		v_guide.f_hide();
		return;
	}
	t_imc imc(himc);
	t_imcc<GUIDELINE> guide(imc->hGuideLine);
	DWORD n = guide->dwStrLen;
	if (n <= 0) {
		v_guide.f_hide();
		return;
	}
	const WCHAR* cs = reinterpret_cast<WCHAR*>(reinterpret_cast<char*>(static_cast<LPGUIDELINE>(guide)) + guide->dwStrOffset);
	HDC hdc = GetDC(v_hwnd);
	HGDIOBJ hfont = SelectObject(hdc, GetStockObject(SYSTEM_FIXED_FONT));
	RECT rect;
	rect.left = rect.top = rect.bottom = 0;
	rect.right = 256;
	DrawText(hdc, cs, n, &rect, DT_LEFT | DT_NOPREFIX | DT_CALCRECT);
	SIZE size = {rect.right - rect.left + 4, rect.bottom - rect.top + 4};
	SelectObject(hdc, hfont);
	ReleaseDC(v_hwnd, hdc);
	POINT spot = f_spot(size.cx, size.cy);
	v_guide.f_show(this, spot.x, spot.y, size.cx, size.cy);
}
