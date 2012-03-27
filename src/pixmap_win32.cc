#include <xraft/pixmap.h>

namespace xraft
{

t_bitmap::~t_bitmap()
{
	DeleteObject(v_handle);
}

t_bitmap::t_bitmap(unsigned a_width, unsigned a_height, const char* a_data, int a_bytes_per_line) : v_width(a_width), v_height(a_height)
{
	char* bits = NULL;
	if (a_data) {
		if (a_bytes_per_line <= 0) a_bytes_per_line = (v_width + 7) / 8;
		int bytes_per_line = (v_width + 15) / 16 * 2;
		bits = new char[bytes_per_line * v_height];
		int width = (v_width + 7) / 8;
		char* p = bits;
		for (unsigned i = 0; i < v_height; ++i) {
			const char* q = a_data;
			for (int j = 0; j < width; ++j) *p++ = *q++;
			for (int j = width; j < bytes_per_line; ++j) *p++ = 0;
			a_data += a_bytes_per_line;
		}
	}
	v_handle = CreateBitmap(v_width, v_height, 1, 1, bits);
	delete[] bits;
}

t_pixmap::~t_pixmap()
{
	DeleteObject(v_handle);
}

t_pixmap::t_pixmap(unsigned a_width, unsigned a_height, bool a_alpha, const char* a_data, int a_bytes_per_line) : v_width(a_width), v_height(a_height), v_alpha(a_alpha)
{
	BITMAPINFO bmi;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = v_width;
	bmi.bmiHeader.biHeight = -static_cast<int>(v_height);
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = a_alpha ? 32 : 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = 0;
	bmi.bmiHeader.biXPelsPerMeter = 0;
	bmi.bmiHeader.biYPelsPerMeter = 0;
	bmi.bmiHeader.biClrUsed = 0;
	bmi.bmiHeader.biClrImportant = 0;
	HDC hdc = GetDC(NULL);
	if (v_alpha) {
		char* bits;
		v_handle = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, reinterpret_cast<void**>(&bits), NULL, 0);
		if (a_data) {
			BITMAP bm;
			GetObject(v_handle, sizeof(BITMAP), &bm);
			if (a_bytes_per_line <= 0) a_bytes_per_line = v_width * 4;
			for (unsigned i = 0; i < v_height; ++i) {
				char* p = bits;
				const char* q = a_data;
				for (unsigned j = 0; j < v_width; ++j) {
					char alpha = *q++;
					char red = *q++;
					char green = *q++;
					char blue = *q++;
					*p++ = blue;
					*p++ = green;
					*p++ = red;
					*p++ = alpha;
				}
				bits += bm.bmWidthBytes;
				a_data += a_bytes_per_line;
			}
		}
	} else {
		v_handle = CreateCompatibleBitmap(hdc, v_width, v_height);
		if (a_data) {
			if (a_bytes_per_line <= 0) a_bytes_per_line = v_width * 3;
			char* bits = new char[v_width * 3 * v_height];
			char* p = bits;
			for (unsigned i = 0; i < v_height; ++i) {
				const char* q = a_data;
				for (unsigned j = 0; j < v_width; ++j) {
					char red = *q++;
					char green = *q++;
					char blue = *q++;
					*p++ = blue;
					*p++ = green;
					*p++ = red;
				}
				a_data += a_bytes_per_line;
			}
			SetDIBits(hdc, static_cast<HBITMAP>(v_handle), 0, v_height, bits, &bmi, 0);
			delete[] bits;
		}
	}
	ReleaseDC(NULL, hdc);
}

}
