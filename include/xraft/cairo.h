#ifndef XRAFT__CAIRO_H
#define XRAFT__CAIRO_H

#include "graphics.h"
#include "window.h"

#ifdef XRAFT_X11
#include <cairo-xlib.h>
#if CAIRO_HAS_XLIB_XRENDER_SURFACE
#include <cairo-xlib-xrender.h>
#endif
#endif
#ifdef XRAFT_WIN32
#include <cairo-win32.h>
#endif

namespace xraft
{

template<typename T_callable>
void f_draw(cairo_surface_t* a_surface, T_callable a_callable)
{
	try {
		cairo_t* context = cairo_create(a_surface);
		try {
			a_callable(context);
		} catch (...) {
			cairo_destroy(context);
			throw;
		}
		cairo_destroy(context);
	} catch (...) {
		cairo_surface_destroy(a_surface);
		throw;
	}
	cairo_surface_destroy(a_surface);
}

#ifdef XRAFT_X11
inline cairo_surface_t* f_surface_create(t_pixmap& a_pixmap)
{
	Display* display = f_application()->f_x11_display();
#if CAIRO_HAS_XLIB_XRENDER_SURFACE
	if (a_pixmap.f_alpha()) {
		Screen* screen = DefaultScreenOfDisplay(display);
		XRenderPictFormat pf;
		pf.type = PictTypeDirect;
		pf.depth = 32;
		pf.direct.red = 16;
		pf.direct.redMask = 0xff;
		pf.direct.green = 8;
		pf.direct.greenMask = 0xff;
		pf.direct.blue = 0;
		pf.direct.blueMask = 0xff;
		pf.direct.alpha = 24;
		pf.direct.alphaMask = 0xff;
		XRenderPictFormat* format = XRenderFindFormat(display, PictFormatType | PictFormatDepth | PictFormatRed | PictFormatRedMask | PictFormatGreen | PictFormatGreenMask | PictFormatBlue | PictFormatBlueMask | PictFormatAlpha | PictFormatAlphaMask, &pf, 0);
		return cairo_xlib_surface_create_with_xrender_format(display, a_pixmap.f_handle(), screen, format, a_pixmap.f_width(), a_pixmap.f_height());
	}
#endif
	int screen = DefaultScreen(display);
	Visual* visual = DefaultVisual(display, screen);
	return cairo_xlib_surface_create(display, a_pixmap.f_handle(), visual, a_pixmap.f_width(), a_pixmap.f_height());
}

inline cairo_surface_t* f_surface_create(t_bitmap& a_bitmap)
{
	Display* display = f_application()->f_x11_display();
	int screen = DefaultScreen(display);
	Visual* visual = DefaultVisual(display, screen);
	return cairo_xlib_surface_create(display, a_bitmap.f_handle(), visual, a_bitmap.f_width(), a_bitmap.f_height());
}

template<typename T_callable>
inline void f_draw(t_window& a_window, T_callable a_callable)
{
	Display* display = f_application()->f_x11_display();
	int screen = DefaultScreen(display);
	Visual* visual = DefaultVisual(display, screen);
	const t_rectangle& geometry = a_window.f_geometry();
	f_draw(cairo_xlib_surface_create(display, a_window.f_handle(), visual, geometry.v_width, geometry.v_height), a_callable);
}

template<typename T_callable>
inline void f_draw(t_graphics& a_graphics, T_callable a_callable)
{
	t_drawable* p = a_graphics.f_drawable();
	t_window* window = dynamic_cast<t_window*>(p);
	if (window) {
		f_draw(*window, a_callable);
		return;
	}
	t_pixmap* pixmap = dynamic_cast<t_pixmap*>(p);
	if (pixmap) {
		Display* display = f_application()->f_x11_display();
		int screen = DefaultScreen(display);
		Visual* visual = DefaultVisual(display, screen);
		f_draw(cairo_xlib_surface_create(display, pixmap->f_handle(), visual, pixmap->f_width(), pixmap->f_height()), a_callable);
		return;
	}
	t_bitmap* bitmap = dynamic_cast<t_bitmap*>(p);
	if (bitmap) {
		Display* display = f_application()->f_x11_display();
		Screen* screen = DefaultScreenOfDisplay(display);
		f_draw(cairo_xlib_surface_create_for_bitmap(display, pixmap->f_handle(), screen, bitmap->f_width(), bitmap->f_height()), a_callable);
		return;
	}
}
#endif

#ifdef XRAFT_WIN32
template<typename T>
struct t_proxy
{
	static cairo_user_data_key_t v_key;

	static cairo_surface_t* f_create(T& a_target)
	{
		HDC hdc0 = GetDC(NULL);
		HDC hdc = CreateCompatibleDC(hdc0);
		ReleaseDC(NULL, hdc0);
		HGDIOBJ old = SelectObject(hdc, a_target.f_handle());
		cairo_surface_t* surface = cairo_win32_surface_create(hdc);
		new t_proxy(surface, a_target, hdc, old);
		return surface;
	}
	static void f_destroy(void* a_p)
	{
		delete static_cast<t_proxy*>(a_p);
	}
	static cairo_surface_t* f_create_alpha(T& a_target)
	{
		BITMAP bm;
		GetObject(a_target.f_handle(), sizeof(BITMAP), &bm);
		cairo_surface_t* surface = cairo_image_surface_create_for_data(static_cast<unsigned char*>(bm.bmBits), CAIRO_FORMAT_ARGB32, bm.bmWidth, bm.bmHeight, bm.bmWidthBytes);
		cairo_surface_set_user_data(surface, &v_key, new t_pointer<T>(&a_target), f_destroy_alpha);
		return surface;
	}
	static void f_destroy_alpha(void* a_p)
	{
		delete static_cast<t_pointer<T>*>(a_p);
	}

	t_pointer<T> v_p;
	HDC v_hdc;
	HGDIOBJ v_old;

	t_proxy(cairo_surface_t* a_surface, T& a_p, HDC a_hdc, HGDIOBJ a_old) : v_p(&a_p), v_hdc(a_hdc), v_old(a_old)
	{
		cairo_surface_set_user_data(a_surface, &v_key, this, f_destroy);
	}
	~t_proxy()
	{
		SelectObject(v_hdc, v_old);
		DeleteDC(v_hdc);
	}
};

template<typename T>
cairo_user_data_key_t t_proxy<T>::v_key;

inline cairo_surface_t* f_surface_create(t_bitmap& a_bitmap)
{
	return t_proxy<t_bitmap>::f_create(a_bitmap);
}

inline cairo_surface_t* f_surface_create(t_pixmap& a_pixmap)
{
	return a_pixmap.f_alpha() ? t_proxy<t_pixmap>::f_create_alpha(a_pixmap) : t_proxy<t_pixmap>::f_create(a_pixmap);
}

template<typename T_callable>
inline void f_draw(t_window& a_window, T_callable a_callable)
{
	HWND hwnd = static_cast<HWND>(a_window.f_handle());
	HDC hdc = GetDC(hwnd);
	try {
		f_draw(cairo_win32_surface_create(hdc), a_callable);
	} catch (...) {
		ReleaseDC(hwnd, hdc);
		throw;
	}
	ReleaseDC(hwnd, hdc);
}

template<typename T_callable>
inline void f_draw(t_graphics& a_graphics, T_callable a_callable)
{
	f_draw(cairo_win32_surface_create(a_graphics.f_win32_hdc()), a_callable);
}

#endif

}

#endif
