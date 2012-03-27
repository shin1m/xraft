#include <xraft/graphics.h>

namespace xraft
{

t_font::~t_font()
{
	XDestroyOC(v_oc);
}

void t_graphics::f_draw(const t_point* a_points, size_t a_n)
{
	XPoint* ps = new XPoint[a_n];
	for (size_t i = 0; i < a_n; ++i) {
		ps[i].x = a_points[i].v_x + v_origin.v_x;
		ps[i].y = a_points[i].v_y + v_origin.v_y;
	}
	t_application* application = f_application();
	XDrawLines(application->f_x11_display(), v_drawable->v_handle, application->v_gc, ps, a_n, CoordModeOrigin);
	delete[] ps;
}

void t_graphics::f_fill(const t_point* a_points, size_t a_n)
{
	XPoint* ps = new XPoint[a_n];
	for (size_t i = 0; i < a_n; ++i) {
		ps[i].x = a_points[i].v_x + v_origin.v_x;
		ps[i].y = a_points[i].v_y + v_origin.v_y;
	}
	t_application* application = f_application();
	XFillPolygon(application->f_x11_display(), v_drawable->v_handle, application->v_gc, ps, a_n, Complex, CoordModeOrigin);
	delete[] ps;
}

void t_graphics::f_draw(int a_x, int a_y, const t_pointer<t_pixmap>& a_pixmap, int a_left, int a_top, unsigned a_width, unsigned a_height, const t_pointer<t_bitmap>& a_mask)
{
	if (!a_pixmap) return;
	a_x += v_origin.v_x;
	a_y += v_origin.v_y;
	int i = XRectInRegion(v_region.v_region, a_x, a_y, a_width, a_height);
	if (i == RectangleOut) return;
	t_application* application = f_application();
	Display* display = application->f_x11_display();
	GC gc = application->v_gc;
	if (a_mask) {
		GC gc_save = application->v_gc_save;
		XCopyGC(display, gc, GCClipXOrigin | GCClipYOrigin | GCClipMask, gc_save);
		Pixmap mask = a_mask->v_handle;
		if (i != RectangleIn) {
			mask = XCreatePixmap(display, v_drawable->v_handle, a_width, a_height, 1);
			GC gc_mask = application->v_gc_mask;
			XSetForeground(display, gc_mask, 0);
			XSetClipMask(display, gc_mask, None);
			XFillRectangle(display, mask, gc_mask, 0, 0, a_width, a_height);
			XSetForeground(display, gc_mask, 1);
			XSetRegion(display, gc_mask, v_region.v_region);
			XSetClipOrigin(display, gc_mask, -a_x, -a_y);
			XCopyArea(display, a_mask->v_handle, mask, gc_mask, a_left, a_top, a_width, a_height, 0, 0);
			XSetClipOrigin(display, gc, a_x, a_y);
		} else {
			XSetClipOrigin(display, gc, a_x - a_left, a_y - a_top);
		}
		XSetClipMask(display, gc, mask);
		XCopyArea(display, a_pixmap->v_handle, v_drawable->v_handle, gc, a_left, a_top, a_width, a_height, a_x, a_y);
		XCopyGC(display, gc_save, GCClipXOrigin | GCClipYOrigin | GCClipMask, gc);
		if (i != RectangleIn) XFreePixmap(display, mask);
	} else {
		XCopyArea(display, a_pixmap->v_handle, v_drawable->v_handle, gc, a_left, a_top, a_width, a_height, a_x, a_y);
	}
}

}
