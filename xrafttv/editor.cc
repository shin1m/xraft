#include <clocale>
#include <cstdio>
#include <xraft/application.h>
#include <xraft/graphics.h>

#include "wrapped_view.h"

using namespace std::literals;
using namespace xraft;

struct t_attribute
{
	t_pixel v_foreground;
	t_pixel v_background;

	t_attribute(t_pixel a_foreground = t_pixel(), t_pixel a_background = t_pixel()) : v_foreground(a_foreground), v_background(a_background)
	{
	}
	bool operator==(const t_attribute& a_x) const
	{
		return v_foreground == a_x.v_foreground && v_background == a_x.v_background;
	}
	bool operator!=(const t_attribute& a_x) const
	{
		return !(*this == a_x);
	}
};

struct t_font
{
	t_pointer<xraft::t_font> v_font;

	t_font(const t_pointer<xraft::t_font>& a_font) : v_font(a_font)
	{
	}
	size_t f_height() const
	{
		return v_font->f_height();
	}
	size_t f_width() const
	{
		return v_font->f_width();
	}
	size_t f_width(wchar_t a_c) const
	{
		return v_font->f_width(a_c);
	}
};

struct t_context
{
	t_graphics& v_g;
	t_pointer<xraft::t_font> v_font;
	t_pixel v_end;
	t_pixel v_foreground;
	t_pixel v_background;

	t_context(t_graphics& a_g, const t_pointer<xraft::t_font>& a_font, t_pixel a_end) : v_g(a_g), v_font(a_font), v_end(a_end)
	{
	}
	bool f_invalid(size_t a_y, size_t a_width, size_t a_height)
	{
		return v_g.f_invalid(0, a_y, a_width, a_height);
	}
	void f_attribute(const ::t_attribute& a_attribute)
	{
		v_foreground = a_attribute.v_foreground;
		v_background = a_attribute.v_background;
	}
	void f_draw_string(size_t a_x, size_t a_y, size_t a_width, const wchar_t* a_cs, size_t a_n)
	{
		v_g.f_color(v_background);
		v_g.f_fill(a_x, a_y, a_width, v_font->f_height());
		v_g.f_color(v_foreground);
		v_g.f_draw(a_x, a_y + v_font->f_ascent(), a_cs, a_n);
	}
	void f_draw_tab(size_t a_x, size_t a_y, size_t a_width)
	{
		size_t width = v_font->f_width();
		size_t height = v_font->f_height();
		v_g.f_color(v_background);
		v_g.f_fill(a_x, a_y, a_width, height);
		int n = (width < height ? width : height) >> 1;
		a_x += (static_cast<int>(width) - n) >> 1;
		a_y += static_cast<int>(height >> 1) - n;
		t_point points[] = {
			t_point(a_x, a_y), t_point(a_x + n, a_y + n), t_point(a_x, a_y + n * 2)
		};
		v_g.f_color(v_foreground);
		v_g.f_fill(points, 3);
	}
	void f_draw_line_terminator(size_t a_x, size_t a_y)
	{
		size_t width = v_font->f_width();
		size_t height = v_font->f_height();
		v_g.f_color(v_background);
		v_g.f_fill(a_x, a_y, width, height);
		int n = width * 3 >> 2;
		if (static_cast<int>(height) < n) n = height;
		a_x += (static_cast<int>(width) - n) >> 1;
		a_y += static_cast<int>(height) - n;
		t_point points[] = {
			t_point(a_x, a_y), t_point(a_x, a_y + n), t_point(a_x + n, a_y + n)
		};
		v_g.f_color(v_foreground);
		v_g.f_fill(points, 3);
	}
	void f_draw_line_end(size_t a_x, size_t a_y, size_t a_width)
	{
		v_g.f_color(v_end);
		v_g.f_fill(a_x, a_y, a_width, v_font->f_height());
	}
	void f_draw_page_end(size_t a_y, size_t a_width, size_t a_height)
	{
		v_g.f_color(v_end);
		v_g.f_fill(0, a_y, a_width, a_height);
	}
	void f_draw_caret(size_t a_x, size_t a_y)
	{
		size_t height = v_font->f_height();
		v_g.f_function(t_graphics::e_function__INVERT);
		int n = height >> 2;
		{
			t_point points[] = {
				t_point(a_x - n, a_y), t_point(a_x, a_y + n), t_point(a_x + n, a_y)
			};
			v_g.f_fill(points, 3);
		}
		++n;
		a_y += height;
		{
			t_point points[] = {
				t_point(a_x - n, a_y), t_point(a_x, a_y - n), t_point(a_x + n, a_y)
			};
			v_g.f_fill(points, 3);
		}
		v_g.f_function(t_graphics::e_function__COPY);
	}
};

class t_editor : public t_frame, t_wrapped_view< ::t_attribute, ::t_font>::t_observer
{
	typedef t_text_model< ::t_attribute>::t_segment t_segment;

	t_text_model< ::t_attribute> v_model;
	t_wrapped_view< ::t_attribute, ::t_font> v_view;
	size_t v_position;
	::t_attribute v_attribute_default;
	::t_attribute v_attribute_compose[4];
	size_t v_composing_begin;
	size_t v_composing_size;

	void f_insert(wchar_t a_c)
	{
		t_segment ss[] = {t_segment(1, v_attribute_default)};
		v_model.f_replace(v_view.f_caret().v_text, 0, &ss[0], &ss[1], &a_c);
	}
	void f_backspace()
	{
		size_t text = v_view.f_caret().v_text;
		if (text > 0) v_model.f_replace(text - 1, 1, static_cast<t_segment*>(0), static_cast<t_segment*>(0), static_cast<wchar_t*>(0));
	}
	void f_delete()
	{
		size_t text = v_view.f_caret().v_text;
		if (text < v_model.f_text_size()) v_model.f_replace(text, 1, static_cast<t_segment*>(0), static_cast<t_segment*>(0), static_cast<wchar_t*>(0));
	}
	virtual void f_on_move()
	{
		t_extent extent = f_geometry();
		v_view.f_width__(extent.v_width);
	}
	virtual void f_on_paint(t_graphics& a_g)
	{
		a_g.f_translate(0, -static_cast<int>(v_position));
		t_context context(a_g, f_application()->f_font(), v_attribute_default.v_background);
		t_rectangle bounds = a_g.f_bounds();
		v_view.f_draw(context, bounds.v_y, bounds.v_y + bounds.v_height);
	}
	virtual void f_on_key_press(t_modifier a_modifier, t_key a_key, char a_ascii)
	{
		switch (a_key) {
		case e_key__LEFT:
			v_view.f_caret_move_text_backward(1);
			break;
		case e_key__RIGHT:
			v_view.f_caret_move_text_forward(1);
			break;
		case e_key__UP:
			v_view.f_caret_move_line_backward(1);
			break;
		case e_key__DOWN:
			v_view.f_caret_move_line_forward(1);
			break;
		case e_key__RETURN:
			f_insert(L'\n');
			break;
		case e_key__BACK_SPACE:
			f_backspace();
			break;
		case e_key__DELETE:
			f_delete();
			break;
		case e_key__ESCAPE:
			for (wchar_t c = L' '; c <= L'~'; ++c) f_insert(c);
			break;
		default:
			if (a_ascii != '\0') f_insert(a_ascii);
		}
		size_t h = f_application()->f_font()->f_height();
		f_scroll_viewable(v_view.f_caret().v_row * h, h);
	}
	virtual void f_on_input_compose()
	{
		if (v_composing_size <= 0) v_composing_begin = v_view.f_caret().v_text;
		std::vector<wchar_t> cs;
		std::vector<t_input_attribute> as;
		f_input_context()->f_composition(cs, as);
		std::vector<t_segment> ss;
		size_t caret = 0;
		size_t i = 0;
		while (i < as.size()) {
			t_input_attribute a = as[i];
			size_t j = i;
			do ++j; while (j < as.size() && as[j] == a);
			ss.push_back(t_segment(j - i, v_attribute_compose[a]));
			if ((a & e_input_attribute__REVERSE) != 0) caret = j;
			i = j;
		}
		v_model.f_replace(v_composing_begin, v_composing_size, ss.begin(), ss.end(), &cs[0]);
		v_view.f_caret_move_text(v_composing_begin + caret);
		v_composing_size = cs.size();
	}
	virtual void f_on_input_commit(const wchar_t* a_cs, size_t a_n)
	{
		if (v_composing_size <= 0) v_composing_begin = v_view.f_caret().v_text;
		t_segment ss[] = {t_segment(a_n, v_attribute_default)};
		v_model.f_replace(v_composing_begin, v_composing_size, &ss[0], &ss[1], a_cs);
		v_composing_size = 0;
	}
	virtual t_rectangle f_on_input_spot()
	{
		size_t h = f_application()->f_font()->f_height();
		return t_rectangle(v_view.f_caret().v_row_x, v_view.f_caret().v_row * h, 0, h);
	}
	virtual void f_on_button_press(t_modifier a_modifier, t_button a_button, int a_x, int a_y)
	{
	}
	virtual void f_on_close()
	{
		f_application()->f_exit();
	}
	virtual void f_invalidated(size_t a_y, size_t a_height)
	{
		t_extent extent = f_geometry();
		int top = a_y - v_position;
		int bottom = top + a_height;
		if (top < 0) top = 0;
		f_invalidate(0, top, extent.v_width, bottom - top);
	}
	virtual void f_resized()
	{
		f_position__(v_position);
	}
	virtual void f_moved(size_t a_y, size_t a_height, size_t a_delta)
	{
		t_extent extent = f_geometry();
		int top = a_y - v_position;
		int bottom = top + a_height;
		if (top < 0) top = 0;
		f_scroll(0, top, extent.v_width, bottom - top, 0, a_delta);
	}

public:
	t_editor() :
	v_view(::t_font(f_application()->f_font())),
	v_position(0),
	v_attribute_default(t_color(0, 0, 0).f_pixel(), t_color(255, 255, 255).f_pixel()),
	v_composing_size(0)
	{
		v_attribute_compose[0] = ::t_attribute(t_color(63, 63, 63).f_pixel(), t_color(255, 255, 255).f_pixel());
		v_attribute_compose[1] = ::t_attribute(t_color(191, 191, 191).f_pixel(), t_color(0, 0, 0).f_pixel());
		v_attribute_compose[2] = ::t_attribute(t_color(0, 0, 0).f_pixel(), t_color(191, 191, 191).f_pixel());
		v_attribute_compose[3] = ::t_attribute(t_color(255, 255, 255).f_pixel(), t_color(0, 0, 0).f_pixel());
		v_model.f_enable_notification();
		v_view.f_model__(&v_model);
		v_view.f_add_observer(this);
		v_view.f_caret_visible__(true);
		f_input_context__(f_application()->f_input_context());
	}
	void f_position__(int a_position)
	{
		t_extent extent = f_geometry();
		size_t limit = v_view.f_rows_size() * f_application()->f_font()->f_height();
		if (a_position + extent.v_height > limit) a_position = limit - extent.v_height;
		if (a_position < 0) a_position = 0;
		if (a_position == v_position) return;
		f_scroll(0, 0, extent.v_width, extent.v_height, 0, v_position - a_position);
		v_position = a_position;
	}
	void f_scroll_viewable(size_t a_y, size_t a_height)
	{
		t_extent extent = f_geometry();
		int position = v_position;
		if (position + extent.v_height < a_y + a_height) position = a_y + a_height - extent.v_height;
		if (position > static_cast<int>(a_y)) position = a_y;
		f_position__(position);
	}
};

int main(int argc, char* argv[])
{
	std::setlocale(LC_ALL, "");
	std::vector<std::string> arguments(argv, argv + argc);
	t_application application(arguments);
	t_pointer<t_editor> frame = new t_editor();
	frame->f_caption__(L"Editor"sv);
	frame->f_move(t_rectangle(0, 0, 320, 240));
	application.f_add(frame);
	frame->f_show();
	application.f_run();
	return 0;
}

#ifdef XRAFT_WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return main(__argc, __argv);
}
#endif
