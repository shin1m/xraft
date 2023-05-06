#include <xraft/pixmap.h>
#include <cstdint>

namespace xraft
{

namespace
{

inline bool f_lsb()
{
	union
	{
		int i;
		char c;
	};
	i = 1;
	return c != 0;
}

}

t_bitmap::~t_bitmap()
{
	XFreePixmap(f_application()->f_x11_display(), v_handle);
}

t_bitmap::t_bitmap(unsigned a_width, unsigned a_height, const char* a_data, int a_bytes_per_line) : v_width(a_width), v_height(a_height)
{
	t_application* application = f_application();
	Display* display = application->f_x11_display();
	int screen = DefaultScreen(display);
	Drawable drawable = RootWindow(display, screen);
	v_handle = XCreatePixmap(display, drawable, v_width, v_height, 1);
	if (!a_data) return;
	if (a_bytes_per_line <= 0) a_bytes_per_line = (v_width + 7) / 8;
	Visual* visual = DefaultVisual(display, screen);
	XImage* image = XCreateImage(display, visual, 1, XYBitmap, 0, NULL, v_width, v_height, 32, 0);
	image->byte_order = f_lsb() ? LSBFirst : MSBFirst;
	image->bitmap_unit = 8;
	image->bitmap_bit_order = MSBFirst;
	char* data = image->data = new char[image->bytes_per_line * v_height];
	for (unsigned i = 0; i < v_height; ++i) {
		const char* p = a_data;
		char* q = data;
		for (int j = 0; j < image->bytes_per_line; ++j) *q++ = *p++;
		a_data += a_bytes_per_line;
		data += image->bytes_per_line;
	}
	XGCValues values;
	values.foreground = 1;
	values.background = 0;
	GC gc = XCreateGC(display, v_handle, GCForeground | GCBackground, &values);
	XPutImage(display, v_handle, gc, image, 0, 0, 0, 0, v_width, v_height);
	XFreeGC(display, gc);
	XDestroyImage(image);
}

t_pixmap::~t_pixmap()
{
	XFreePixmap(f_application()->f_x11_display(), v_handle);
}

t_pixmap::t_pixmap(unsigned a_width, unsigned a_height, bool a_alpha, const char* a_data, int a_bytes_per_line) : v_width(a_width), v_height(a_height), v_alpha(a_alpha)
{
	t_application* application = f_application();
	Display* display = application->f_x11_display();
	int screen = DefaultScreen(display);
	Drawable drawable = RootWindow(display, screen);
	int depth = v_alpha ? 32 : DefaultDepth(display, screen);
	v_handle = XCreatePixmap(display, drawable, v_width, v_height, depth);
	if (!a_data) return;
	if (a_bytes_per_line <= 0) a_bytes_per_line = v_width * 3;
	Visual* visual = DefaultVisual(display, screen);
	XImage* image = XCreateImage(display, visual, depth, ZPixmap, 0, NULL, v_width, v_height, 32, 0);
	image->byte_order = f_lsb() ? LSBFirst : MSBFirst;
	image->bits_per_pixel = 32;
	image->bytes_per_line = 4 * v_width;
	char* data = image->data = new char[image->bytes_per_line * v_height];
	for (unsigned i = 0; i < v_height; ++i) {
		const char* p = a_data;
		uint32_t* q = reinterpret_cast<uint32_t*>(data);
		for (unsigned j = 0; j < v_width; ++j) {
			unsigned char a = *p++;
			uint32_t pixel = application->f_x11_red(a);
			a = *p++;
			pixel |= application->f_x11_green(a);
			a = *p++;
			pixel |= application->f_x11_blue(a);
			*q++ = pixel;
		}
		a_data += a_bytes_per_line;
		data += image->bytes_per_line;
	}
	XPutImage(display, v_handle, application->v_gc, image, 0, 0, 0, 0, v_width, v_height);
	XDestroyImage(image);
}

}
