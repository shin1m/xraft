#ifndef XRAFT__GRAPHICS_WIN32_H
#define XRAFT__GRAPHICS_WIN32_H

namespace xraft
{

class t_graphics;

class t_font : public t_object
{
	friend class t_application;
	friend class t_graphics;

	struct t_width
	{
		wchar_t c;
		INT w;
	};

	HFONT v_hfont;
	TEXTMETRIC v_tm;
	mutable t_width v_widths[256];

protected:
	XRAFT__EXPORT virtual ~t_font();

public:
	t_font(const char* a_face, unsigned a_size = 12, bool a_bold = false, bool a_italic = false, bool a_underline = false)
	{
		v_hfont = CreateFontA(
			a_size, 0, 0, 0, a_bold ? FW_BOLD : FW_DONTCARE, a_italic ? TRUE : FALSE, a_underline ? TRUE : FALSE, FALSE,
			DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE, a_face);
		HDC hdc = f_application()->v_hdc;
		SelectObject(hdc, v_hfont);
		GetTextMetrics(hdc, &v_tm);
		INT widths[256];
		GetCharWidthW(hdc, 0, 255, widths);
		for (int i = 0; i < 256; ++i) {
			v_widths[i].c = static_cast<wchar_t>(i);
			v_widths[i].w = widths[i];
		}
	}
	unsigned f_ascent() const
	{
		return v_tm.tmAscent;
	}
	unsigned f_descent() const
	{
		return v_tm.tmDescent;
	}
	unsigned f_height() const
	{
		return v_tm.tmHeight;
	}
	unsigned f_width() const
	{
		return v_tm.tmAveCharWidth;
	}
	unsigned f_width(wchar_t a_c) const
	{
		t_width& w = v_widths[static_cast<unsigned char>((a_c >> 8) ^ a_c)];
		if (a_c == w.c) return w.w;
		w.c = a_c;
		HDC hdc = f_application()->v_hdc;
		SelectObject(hdc, v_hfont);
		GetCharWidthW(hdc, a_c, a_c, &w.w);
		return w.w;
	}
	template<typename I>
	unsigned f_width(I a_i, size_t a_n, unsigned a_x = 0) const
	{
		unsigned x = a_x;
		while (a_n > 0) {
			x += *a_i == L'\t' ? f_width() : f_width(*a_i);
			++a_i;
			--a_n;
		}
		return x - a_x;
	}
};

typedef COLORREF t_pixel;

class t_color
{
	COLORREF v_colorref;

public:
	t_color(unsigned char a_red, unsigned char a_green, unsigned char a_blue) : v_colorref(RGB(a_red, a_green, a_blue))
	{
	}
	t_color(int a_id = COLOR_WINDOWTEXT) : v_colorref(GetSysColor(a_id))
	{
	}
	unsigned char f_red() const
	{
		return GetRValue(v_colorref);
	}
	unsigned char f_green() const
	{
		return GetGValue(v_colorref);
	}
	unsigned char f_blue() const
	{
		return GetBValue(v_colorref);
	}
	t_pixel f_pixel() const
	{
		return v_colorref;
	}
};

class t_graphics
{
	friend class t_window;

	t_graphics(const t_pointer<t_window>& a_window, HDC a_hdc) : v_drawable(a_window), v_origin(0, 0), v_font(f_application()->f_font()), v_owns(false), v_hdc(a_hdc)
	{
		f_initialize();
		if (GetClipRgn(v_hdc, v_region.v_handle) == 0) v_region.f_unite(v_region, 0, 0, 0x7fff, 0x7fff);
	}
	void f_initialize()
	{
		SelectObject(v_hdc, v_font->v_hfont);
		t_pixel pixel = RGB(0, 0, 0);
		SelectObject(v_hdc, CreatePen(PS_SOLID, 0, pixel));
		SelectObject(v_hdc, CreateSolidBrush(pixel));
		SetTextColor(v_hdc, pixel);
		SetROP2(v_hdc, R2_COPYPEN);
	}

protected:
	t_pointer<t_drawable> v_drawable;
	t_region v_region;
	t_point v_origin;
	t_pointer<t_font> v_font;
	bool v_owns;
	HDC v_hdc;
	HGDIOBJ v_bitmap;

public:
	enum t_function
	{
		e_function__CLEAR = R2_BLACK,
		e_function__AND = R2_MASKPEN,
		e_function__AND_REVERSE = R2_MASKPENNOT,
		e_function__COPY = R2_COPYPEN,
		e_function__AND_INVERTED = R2_MASKNOTPEN,
		e_function__NOOP = R2_NOP,
		e_function__XOR = R2_XORPEN,
		e_function__OR = R2_MERGEPEN,
		e_function__NOR = R2_NOTMERGEPEN,
		e_function__EQUIV = R2_NOTXORPEN,
		e_function__INVERT = R2_NOT,
		e_function__OR_REVERSE = R2_MERGEPENNOT,
		e_function__COPY_INVERTED = R2_NOTCOPYPEN,
		e_function__OR_INVERTED = R2_MERGENOTPEN,
		e_function__NAND = R2_NOTMASKPEN,
		e_function__SET = R2_WHITE
	};

	HDC f_win32_hdc() const
	{
		return v_hdc;
	}

	t_graphics(const t_pointer<t_drawable>& a_drawable, bool a_include = false) : v_drawable(a_drawable), v_origin(0, 0), v_font(f_application()->f_font()), v_owns(true)
	{
		if (dynamic_cast<t_window*>(static_cast<t_drawable*>(v_drawable))) {
			v_hdc = GetDC(static_cast<HWND>(v_drawable->v_handle));
		} else {
			v_hdc = CreateCompatibleDC(NULL);
			v_bitmap = SelectObject(v_hdc, v_drawable->v_handle);
		}
		f_initialize();
		v_region.f_unite(v_region, 0, 0, 0x7fff, 0x7fff);
	}
	~t_graphics()
	{
		SelectObject(v_hdc, GetStockObject(DEFAULT_GUI_FONT));
		DeleteObject(SelectObject(v_hdc, GetStockObject(BLACK_PEN)));
		DeleteObject(SelectObject(v_hdc, GetStockObject(BLACK_BRUSH)));
		if (!v_owns) return;
		if (dynamic_cast<t_window*>(static_cast<t_drawable*>(v_drawable))) {
			ReleaseDC(static_cast<HWND>(v_drawable->v_handle), v_hdc);
		} else {
			SelectObject(v_hdc, v_bitmap);
			DeleteDC(v_hdc);
		}
	}
	const t_pointer<t_drawable>& f_drawable() const
	{
		return v_drawable;
	}
	const t_region& f_region() const
	{
		return v_region;
	}
	void f_clip(t_region& a_region)
	{
		v_region.f_swap(a_region);
		SelectClipRgn(v_hdc, v_region.v_handle);
	}
	void f_font(const t_pointer<t_font>& a_font)
	{
		v_font = a_font;
		SelectObject(v_hdc, a_font->v_hfont);
	}
	void f_color(t_pixel a_pixel)
	{
		DeleteObject(SelectObject(v_hdc, CreatePen(PS_SOLID, 0, a_pixel)));
		DeleteObject(SelectObject(v_hdc, CreateSolidBrush(a_pixel)));
		SetTextColor(v_hdc, a_pixel);
	}
	void f_function(t_function a_function)
	{
		SetROP2(v_hdc, a_function);
	}
	void f_translate(int a_x, int a_y)
	{
		v_origin.v_x = a_x;
		v_origin.v_y = a_y;
	}
	t_rectangle f_bounds() const
	{
		RECT rect;
		GetClipBox(v_hdc, &rect);
		return t_rectangle(rect.left - v_origin.v_x, rect.top - v_origin.v_y, rect.right - rect.left, rect.bottom - rect.top);
	}
	bool f_invalid(int a_x, int a_y, unsigned a_width, unsigned a_height) const
	{
		a_x += v_origin.v_x;
		a_y += v_origin.v_y;
		RECT rect;
		rect.left = a_x;
		rect.top = a_y;
		rect.right = a_x + static_cast<int>(a_width);
		rect.bottom = a_y + static_cast<int>(a_height);
		return RectVisible(v_hdc, &rect) != FALSE;
	}
	void f_draw(int a_x, int a_y, unsigned a_width, unsigned a_height)
	{
		a_x += v_origin.v_x;
		a_y += v_origin.v_y;
		HGDIOBJ old = SelectObject(v_hdc, GetStockObject(NULL_BRUSH));
		Rectangle(v_hdc, a_x, a_y, a_x + static_cast<int>(a_width), a_y + static_cast<int>(a_height));
		SelectObject(v_hdc, old);
	}
	void f_draw(const t_point* a_points, size_t a_n)
	{
		POINT* ps = new POINT[a_n];
		for (size_t i = 0; i < a_n; ++i) {
			ps[i].x = a_points[i].v_x + v_origin.v_x;
			ps[i].y = a_points[i].v_y + v_origin.v_y;
		}
		HGDIOBJ old = SelectObject(v_hdc, GetStockObject(NULL_BRUSH));
		Polyline(v_hdc, ps, a_n);
		SelectObject(v_hdc, old);
		delete[] ps;
	}
	void f_fill(int a_x, int a_y, unsigned a_width, unsigned a_height)
	{
		a_x += v_origin.v_x;
		a_y += v_origin.v_y;
		RECT rect;
		rect.left = a_x;
		rect.top = a_y;
		rect.right = a_x + static_cast<int>(a_width);
		rect.bottom = a_y + static_cast<int>(a_height);
		COLORREF colorref = SetBkColor(v_hdc, GetTextColor(v_hdc));
		ExtTextOut(v_hdc, a_x, a_y, ETO_OPAQUE, &rect, NULL, 0, NULL);
		SetBkColor(v_hdc, colorref);
	}
	void f_fill(const t_point* a_points, size_t a_n)
	{
		POINT* ps = new POINT[a_n];
		for (size_t i = 0; i < a_n; ++i) {
			ps[i].x = a_points[i].v_x + v_origin.v_x;
			ps[i].y = a_points[i].v_y + v_origin.v_y;
		}
		HGDIOBJ old = SelectObject(v_hdc, GetStockObject(NULL_PEN));
		Polygon(v_hdc, ps, a_n);
		SelectObject(v_hdc, old);
		delete[] ps;
	}
	void f_draw(int a_x, int a_y, const wchar_t* a_cs, size_t a_n)
	{
		a_x += v_origin.v_x;
		a_y += v_origin.v_y;
		SetBkMode(v_hdc, TRANSPARENT);
		ExtTextOutW(v_hdc, a_x, a_y - v_font->f_ascent(), 0, NULL, a_cs, a_n, NULL);
	}
	XRAFT__EXPORT void f_draw(int a_x, int a_y, const t_pointer<t_pixmap>& a_pixmap, int a_left, int a_top, unsigned a_width, unsigned a_height, const t_pointer<t_bitmap>& a_mask = 0);
};

}

#endif
