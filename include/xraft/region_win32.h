#ifndef XRAFT__REGION_WIN32_H
#define XRAFT__REGION_WIN32_H

#include <algorithm>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

namespace xraft
{

class t_region
{
	friend class t_graphics;

	HRGN v_handle;

public:
	t_region() : v_handle(CreateRectRgn(0, 0, 0, 0))
	{
	}
	~t_region()
	{
		DeleteObject(v_handle);
	}
	void f_clear()
	{
		DeleteObject(v_handle);
		v_handle = CreateRectRgn(0, 0, 0, 0);
	}
	void f_swap(t_region& a)
	{
		std::swap(v_handle, a.v_handle);
	}
	bool f_empty() const
	{
		RECT rect;
		return GetRgnBox(v_handle, &rect) == NULLREGION;
	}
	bool f_contains(int x, int y, unsigned width, unsigned height) const
	{
		RECT rect;
		rect.left = x;
		rect.top = y;
		rect.right = x + width;
		rect.bottom = y + height;
		return RectInRegion(v_handle, &rect) != FALSE;
	}
	t_rectangle f_bounds() const
	{
		RECT rect;
		GetRgnBox(v_handle, &rect);
		return t_rectangle(rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top);
	}
	void f_intersect(const t_region& a, const t_region& b)
	{
		CombineRgn(v_handle, a.v_handle, b.v_handle, RGN_AND);
	}
	void f_unite(const t_region& a, const t_region& b)
	{
		CombineRgn(v_handle, a.v_handle, b.v_handle, RGN_OR);
	}
	void f_unite(const t_region& a, int x, int y, unsigned width, unsigned height)
	{
		HRGN handle = CreateRectRgn(x, y, x + width, y + height);
		CombineRgn(v_handle, a.v_handle, handle, RGN_OR);
		DeleteObject(handle);
	}
	void f_subtract(const t_region& a, const t_region& b)
	{
		CombineRgn(v_handle, a.v_handle, b.v_handle, RGN_DIFF);
	}
	void f_xor(const t_region& a, const t_region& b)
	{
		CombineRgn(v_handle, a.v_handle, b.v_handle, RGN_XOR);
	}
	void f_translate(int x, int y)
	{
		OffsetRgn(v_handle, x, y);
	}
};

}

#endif
