#ifndef XRAFT__GRAPHICS_X11_H
#define XRAFT__GRAPHICS_X11_H

namespace xraft
{

class t_graphics;

class t_font : public t_object
{
	friend class t_application;
	friend class t_graphics;

	XOC v_oc;

	virtual ~t_font();

public:
	t_font(const char* a_name) : v_oc(XCreateOC(f_application()->v_om, XNBaseFontName, a_name, NULL))
	{
	}
	unsigned f_ascent() const
	{
		return -XExtentsOfFontSet(v_oc)->max_logical_extent.y;
	}
	unsigned f_descent() const
	{
		const XRectangle& extent = XExtentsOfFontSet(v_oc)->max_logical_extent;
		return extent.y + extent.height;
	}
	unsigned f_height() const
	{
		return XExtentsOfFontSet(v_oc)->max_logical_extent.height;
	}
	unsigned f_width() const
	{
		return XwcTextEscapement(v_oc, L"M", 1);
	}
	unsigned f_width(wchar_t a_c) const
	{
		return XwcTextEscapement(v_oc, &a_c, 1);
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

typedef unsigned long t_pixel;

class t_color
{
	unsigned char v_red;
	unsigned char v_green;
	unsigned char v_blue;
	t_pixel v_pixel;

public:
	t_color(unsigned char a_red, unsigned char a_green, unsigned char a_blue) : v_red(a_red), v_green(a_green), v_blue(a_blue)
	{
		t_application* application = f_application();
		v_pixel = application->f_x11_red(a_red);
		v_pixel |= application->f_x11_green(a_green);
		v_pixel |= application->f_x11_blue(a_blue);
	}
	t_color(const char* a_name)
	{
		Display* display = f_application()->f_x11_display();
		XColor color;
		XAllocNamedColor(display, DefaultColormap(display, DefaultScreen(display)), a_name, &color, &color);
		v_red = color.red / 0x0101;
		v_green = color.green / 0x0101;
		v_blue = color.blue / 0x0101;
		v_pixel = color.pixel;
	}
	unsigned char f_red() const
	{
		return v_red;
	}
	unsigned char f_green() const
	{
		return v_green;
	}
	unsigned char f_blue() const
	{
		return v_blue;
	}
	t_pixel f_pixel() const
	{
		return v_pixel;
	}
};

class t_graphics
{
	t_pointer<t_drawable> v_drawable;
	t_region v_region;
	t_point v_origin;
	t_pointer<t_font> v_font;

public:
	enum t_function
	{
		e_function__CLEAR = GXclear,
		e_function__AND = GXand,
		e_function__AND_REVERSE = GXandReverse,
		e_function__COPY = GXcopy,
		e_function__AND_INVERTED = GXandInverted,
		e_function__NOOP = GXnoop,
		e_function__XOR = GXxor,
		e_function__OR = GXor,
		e_function__NOR = GXnor,
		e_function__EQUIV = GXequiv,
		e_function__INVERT = GXinvert,
		e_function__OR_REVERSE = GXorReverse,
		e_function__COPY_INVERTED = GXcopyInverted,
		e_function__OR_INVERTED = GXorInverted,
		e_function__NAND = GXnand,
		e_function__SET = GXset
	};

	t_graphics(const t_pointer<t_drawable>& a_drawable, bool a_include = false) : v_drawable(a_drawable), v_origin(0, 0), v_font(f_application()->f_font())
	{
		t_application* application = f_application();
		Display* display = application->f_x11_display();
		GC gc = application->v_gc;
		t_pixel black = BlackPixel(display, DefaultScreen(display));
		t_pixel white = WhitePixel(display, DefaultScreen(display));
		XSetState(display, gc, black, white, GXcopy, ~0ul);
		v_region.f_unite(v_region, 0, 0, 0x7fff, 0x7fff);
		if (a_include) XSetSubwindowMode(display, gc, IncludeInferiors);
	}
	~t_graphics()
	{
		t_application* application = f_application();
		Display* display = application->f_x11_display();
		GC gc = application->v_gc;
		XSetClipMask(display, gc, None);
		XSetSubwindowMode(display, gc, ClipByChildren);
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
		t_application* application = f_application();
		Display* display = application->f_x11_display();
		GC gc = application->v_gc;
//		if (v_region.f_empty()) {
//			XSetClipMask(display, gc, None);
//		} else {
			XSetRegion(display, gc, v_region.v_region);
			XSetClipOrigin(display, gc, 0, 0);
//		}
	}
	void f_font(const t_pointer<t_font>& a_font)
	{
		v_font = a_font;
	}
	void f_color(t_pixel a_pixel)
	{
		t_application* application = f_application();
		XSetForeground(application->f_x11_display(), application->v_gc, a_pixel);
	}
	void f_function(t_function a_function)
	{
		t_application* application = f_application();
		XSetFunction(application->f_x11_display(), application->v_gc, a_function);
	}
	void f_translate(int a_x, int a_y)
	{
		v_origin.v_x = a_x;
		v_origin.v_y = a_y;
	}
	t_rectangle f_bounds() const
	{
		t_rectangle bounds = v_region.f_bounds();
		return t_rectangle(bounds.v_x - v_origin.v_x, bounds.v_y - v_origin.v_y, bounds.v_width, bounds.v_height);
	}
	bool f_invalid(int a_x, int a_y, unsigned a_width, unsigned a_height) const
	{
		a_x += v_origin.v_x;
		a_y += v_origin.v_y;
		return v_region.f_contains(a_x, a_y, a_width, a_height);
	}
	void f_draw(int a_x, int a_y, unsigned a_width, unsigned a_height)
	{
		a_x += v_origin.v_x;
		a_y += v_origin.v_y;
		t_application* application = f_application();
		XDrawRectangle(application->f_x11_display(), v_drawable->v_handle, application->v_gc, a_x, a_y, a_width, a_height);
	}
	void f_draw(const t_point* a_points, size_t a_n);
	void f_fill(int a_x, int a_y, unsigned a_width, unsigned a_height)
	{
		a_x += v_origin.v_x;
		a_y += v_origin.v_y;
		t_application* application = f_application();
		XFillRectangle(application->f_x11_display(), v_drawable->v_handle, application->v_gc, a_x, a_y, a_width, a_height);
	}
	void f_fill(const t_point* a_points, size_t a_n);
	void f_draw(int a_x, int a_y, const wchar_t* a_cs, size_t a_n)
	{
		a_x += v_origin.v_x;
		a_y += v_origin.v_y;
		t_application* application = f_application();
		XwcDrawString(application->f_x11_display(), v_drawable->v_handle, v_font->v_oc, application->v_gc, a_x, a_y, a_cs, a_n);
	}
	void f_draw(int a_x, int a_y, const t_pointer<t_pixmap>& a_pixmap, int a_left, int a_top, unsigned a_width, unsigned a_height, const t_pointer<t_bitmap>& a_mask = 0);
	void f_draw(int a_x, int a_y, Pixmap a_handle, int a_left, int a_top, unsigned a_width, unsigned a_height)
	{
		t_application* application = f_application();
		XCopyArea(application->f_x11_display(), a_handle, v_drawable->v_handle, application->v_gc, a_left, a_top, a_width, a_height, a_x, a_y);
	}
};

}

#endif
