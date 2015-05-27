#include <xraft/graphics.h>

namespace xraft
{

t_font::~t_font()
{
	DeleteObject(v_hfont);
}

void t_graphics::f_draw(int a_x, int a_y, const t_pointer<t_pixmap>& a_pixmap, int a_left, int a_top, unsigned a_width, unsigned a_height, const t_pointer<t_bitmap>& a_mask)
{
	if (!a_pixmap) return;
	a_x += v_origin.v_x;
	a_y += v_origin.v_y;
	RECT rect;
	rect.left = a_x;
	rect.top = a_y;
	rect.right = a_x + static_cast<int>(a_width);
	rect.bottom = a_y + static_cast<int>(a_height);
	if (RectVisible(v_hdc, &rect) == FALSE) return;
	HDC hdc = f_application()->v_hdc;
	HGDIOBJ old = SelectObject(hdc, a_pixmap->v_handle);
	if (a_mask) {
		HGDIOBJ old = SelectObject(v_hdc, GetStockObject(BLACK_BRUSH));
		MaskBlt(v_hdc, a_x, a_y, a_width, a_height, hdc, a_left, a_top, static_cast<HBITMAP>(a_mask->v_handle), a_left, a_top, MAKEROP4(SRCCOPY, PATINVERT));
		SelectObject(v_hdc, old);
	} else if (a_pixmap->f_alpha()) {
		BLENDFUNCTION bf;
		bf.BlendOp = AC_SRC_OVER;
		bf.BlendFlags = 0;
		bf.SourceConstantAlpha = 255;
		bf.AlphaFormat = AC_SRC_ALPHA;
		AlphaBlend(v_hdc, a_x, a_y, a_width, a_height, hdc, a_left, a_top, a_width, a_height, bf);
	} else {
		BitBlt(v_hdc, a_x, a_y, a_width, a_height, hdc, a_left, a_top, SRCCOPY);
	}
	SelectObject(hdc, old);
}

}
