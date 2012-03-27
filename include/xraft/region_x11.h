#ifndef XRAFT__REGION_X11_H
#define XRAFT__REGION_X11_H

#include <algorithm>
#include <X11/Xutil.h>

namespace xraft
{

class t_region
{
	friend class t_window;
	friend class t_graphics;

	Region v_region;

public:
	t_region() : v_region(XCreateRegion())
	{
	}
	~t_region()
	{
		XDestroyRegion(v_region);
	}
	void f_clear()
	{
		XDestroyRegion(v_region);
		v_region = XCreateRegion();
	}
	void f_swap(t_region& a)
	{
		std::swap(v_region, a.v_region);
	}
	bool f_empty() const
	{
		return XEmptyRegion(v_region) == True;
	}
	bool f_contains(int x, int y, unsigned width, unsigned height) const
	{
		return XRectInRegion(v_region, x, y, width, height) != RectangleOut;
	}
	t_rectangle f_bounds() const
	{
		XRectangle bounds;
		XClipBox(v_region, &bounds);
		return t_rectangle(bounds.x, bounds.y, bounds.width, bounds.height);
	}
	void f_intersect(const t_region& a, const t_region& b)
	{
		XIntersectRegion(a.v_region, b.v_region, v_region);
	}
	void f_unite(const t_region& a, const t_region& b)
	{
		XUnionRegion(a.v_region, b.v_region, v_region);
	}
	void f_unite(const t_region& a, int x, int y, unsigned width, unsigned height)
	{
		XRectangle rectangle;
		rectangle.x = x;
		rectangle.y = y;
		rectangle.width = width;
		rectangle.height = height;
		XUnionRectWithRegion(&rectangle, a.v_region, v_region);
	}
	void f_subtract(const t_region& a, const t_region& b)
	{
		XSubtractRegion(a.v_region, b.v_region, v_region);
	}
	void f_xor(const t_region& a, const t_region& b)
	{
		XXorRegion(a.v_region, b.v_region, v_region);
	}
	void f_translate(int x, int y)
	{
		XOffsetRegion(v_region, x, y);
	}
};

}

#endif
