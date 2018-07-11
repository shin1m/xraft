#ifndef XRAFT__XRAFTTV__XRAFTTV_H
#define XRAFT__XRAFTTV__XRAFTTV_H

#include <xemmai/array.h>

#include "object.h"
#include "wrapped_view.h"

namespace xemmaix::xrafttv
{

using namespace xraft;
using namespace xemmai;

struct t_attribute : ::xraft::t_object
{
	t_pixel v_foreground;
	t_pixel v_background;

	t_attribute(t_pixel a_foreground, t_pixel a_background) : v_foreground(a_foreground), v_background(a_background)
	{
	}
	t_pixel f_foreground() const
	{
		return v_foreground;
	}
	t_pixel f_background() const
	{
		return v_background;
	}
};

struct t_font
{
	t_pointer<xraft::t_font> v_font;

	t_font(const t_pointer<xraft::t_font>& a_font) : v_font(a_font)
	{
	}
	bool operator==(const t_font& a_x) const
	{
		return v_font == a_x.v_font;
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
	void f_attribute(const t_pointer<t_attribute>& a_attribute)
	{
		v_foreground = a_attribute->v_foreground;
		v_background = a_attribute->v_background;
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

struct t_event
{
	t_scoped v_handlers;

	t_event() : v_handlers(t_array::f_instantiate())
	{
	}
	void f_add(t_scoped&& a_handler)
	{
		f_as<t_array&>(v_handlers).f_push(std::move(a_handler));
	}
	void f_remove(t_scoped&& a_handler)
	{
		t_array& array = f_as<t_array&>(v_handlers);
		for (size_t i = 0; i < array.f_size(); ++i) {
			if (array[i] != a_handler) continue;
			array.f_remove(i);
			break;
		}
	}
	void operator()() const
	{
		t_array& array = f_as<t_array&>(v_handlers);
		for (size_t i = 0; i < array.f_size(); ++i) array[i]();
	}
	void operator()(const t_value& a_0) const
	{
		t_array& array = f_as<t_array&>(v_handlers);
		for (size_t i = 0; i < array.f_size(); ++i) array[i](a_0);
	}
	void operator()(const t_value& a_0, const t_value& a_1) const
	{
		t_array& array = f_as<t_array&>(v_handlers);
		for (size_t i = 0; i < array.f_size(); ++i) array[i](a_0, a_1);
	}
	void operator()(const t_value& a_0, const t_value& a_1, const t_value& a_2) const
	{
		t_array& array = f_as<t_array&>(v_handlers);
		for (size_t i = 0; i < array.f_size(); ++i) array[i](a_0, a_1, a_2);
	}
};

struct t_text_model : ::xraft::t_object, ::xraft::t_text_model<t_pointer<t_attribute>>, ::xraft::t_text_model<t_pointer<t_attribute>>::t_observer
{
	typedef ::xraft::t_text_model<t_pointer<t_attribute>> t_base;

	t_event v_loaded;
	t_event v_replacing;
	t_event v_replaced;

	virtual void f_loaded();
	virtual void f_replacing(size_t a_p, size_t a_n);
	virtual void f_replaced(size_t a_n);

	t_text_model()
	{
		f_add_observer(this);
	}
	void f_add_loaded(t_scoped&& a_handler)
	{
		v_loaded.f_add(std::move(a_handler));
	}
	void f_remove_loaded(t_scoped&& a_handler)
	{
		v_loaded.f_remove(std::move(a_handler));
	}
	void f_add_replacing(t_scoped&& a_handler)
	{
		v_replacing.f_add(std::move(a_handler));
	}
	void f_remove_replacing(t_scoped&& a_handler)
	{
		v_replacing.f_remove(std::move(a_handler));
	}
	void f_add_replaced(t_scoped&& a_handler)
	{
		v_replaced.f_add(std::move(a_handler));
	}
	void f_remove_replaced(t_scoped&& a_handler)
	{
		v_replaced.f_remove(std::move(a_handler));
	}
	void f_enable_notification()
	{
		t_base::f_enable_notification();
	}
	void f_disable_notification()
	{
		t_base::f_disable_notification();
	}
	size_t f_text_size() const
	{
		return t_base::f_text_size();
	}
	size_t f_segments_size() const
	{
		return t_base::f_segments_size();
	}
	size_t f_segment_to_text(size_t a_p) const
	{
		if (a_p >= f_segments_size()) t_throwable::f_throw(L"out of range.");
		return t_base::f_segment_to_text(a_p);
	}
	const t_pointer<t_attribute>& f_segment_to_attribute(size_t a_p) const
	{
		if (a_p >= f_segments_size()) t_throwable::f_throw(L"out of range.");
		return t_base::f_segment_to_attribute(a_p);
	}
	size_t f_text_to_segment(size_t a_p) const
	{
		if (a_p >= f_text_size()) t_throwable::f_throw(L"out of range.");
		return t_base::f_text_to_segment(a_p);
	}
	t_scoped f_slice(size_t a_p, size_t a_n) const;
	void f_replace(size_t a_p, size_t a_n, t_scoped&& a_segments, const std::wstring& a_text);
	void f_attribute(size_t a_p, size_t a_n, const t_pointer<t_attribute>& a_attribute)
	{
		size_t n = f_text_size();
		if (a_p > n || a_n > n || a_p + a_n > n) t_throwable::f_throw(L"out of range.");
		t_base::f_attribute(a_p, a_n, a_attribute);
	}
};

struct t_wrapped_view : ::xraft::t_object, ::xraft::t_wrapped_view<t_pointer<t_attribute>, t_font>, ::xraft::t_wrapped_view<t_pointer<t_attribute>, t_font>::t_observer
{
	typedef ::xraft::t_wrapped_view<t_pointer<t_attribute>, t_font> t_base;

	t_event v_invalidated;
	t_event v_resized;
	t_event v_moved;
	t_pointer<t_text_model> v_model;

	virtual void f_invalidated(size_t a_y, size_t a_height);
	virtual void f_resized();
	virtual void f_moved(size_t a_y, size_t a_height, size_t a_delta);

	t_wrapped_view() : t_base(t_font(f_application()->f_font()))
	{
		f_add_observer(this);
	}
	void f_add_invalidated(t_scoped&& a_handler)
	{
		v_invalidated.f_add(std::move(a_handler));
	}
	void f_remove_invalidated(t_scoped&& a_handler)
	{
		v_invalidated.f_remove(std::move(a_handler));
	}
	void f_add_resized(t_scoped&& a_handler)
	{
		v_resized.f_add(std::move(a_handler));
	}
	void f_remove_resized(t_scoped&& a_handler)
	{
		v_resized.f_remove(std::move(a_handler));
	}
	void f_add_moved(t_scoped&& a_handler)
	{
		v_moved.f_add(std::move(a_handler));
	}
	void f_remove_moved(t_scoped&& a_handler)
	{
		v_moved.f_remove(std::move(a_handler));
	}
	void f_draw(t_graphics& a_g, t_pixel a_end, size_t a_top, size_t a_bottom)
	{
		t_context context(a_g, f_font(), a_end);
		t_base::f_draw(context, a_top, a_bottom);
	}
	const t_pointer<xraft::t_font>& f_font() const
	{
		return t_base::f_font().v_font;
	}
	void f_font__(const t_pointer<xraft::t_font>& a_font)
	{
		t_base::f_font__(t_font(a_font));
	}
	const t_pointer<t_text_model>& f_model() const
	{
		return v_model;
	}
	void f_model__(const t_pointer<t_text_model>& a_model)
	{
		t_base::f_model__(a_model);
		v_model = a_model;
	}
	size_t f_width() const
	{
		return t_base::f_width();
	}
	void f_width__(size_t a_width)
	{
		t_base::f_width__(a_width);
	}
	size_t f_height() const
	{
		return t_base::f_height();
	}
	bool f_caret_visible() const
	{
		return t_base::f_caret_visible();
	}
	void f_caret_visible__(bool a_visible)
	{
		t_base::f_caret_visible__(a_visible);
	}
	size_t f_caret_text() const
	{
		return f_caret().v_text;
	}
	size_t f_caret_row() const
	{
		return f_caret().v_row;
	}
	size_t f_caret_line() const
	{
		return f_caret().v_line;
	}
	size_t f_caret_row_x() const
	{
		return f_caret().v_row_x;
	}
	size_t f_caret_line_x() const
	{
		return f_caret().v_line_x;
	}
	size_t f_rows_size() const
	{
		return t_base::f_rows_size();
	}
	size_t f_lines_size() const
	{
		return t_base::f_lines_size();
	}
	size_t f_line_to_row(size_t a_line) const
	{
		if (a_line >= f_lines_size()) t_throwable::f_throw(L"out of range.");
		return t_base::f_line_to_row(a_line);
	}
	size_t f_row_to_text(size_t a_row) const
	{
		if (a_row >= f_rows_size()) t_throwable::f_throw(L"out of range.");
		return t_base::f_row_to_text(a_row);
	}
	size_t f_line_to_text(size_t a_line) const
	{
		if (a_line >= f_lines_size()) t_throwable::f_throw(L"out of range.");
		return t_base::f_line_to_text(a_line);
	}
	void f_caret_move_text(size_t a_text)
	{
		if (!v_model) t_throwable::f_throw(L"no model.");
		t_base::f_caret_move_text(a_text);
	}
	void f_caret_move_text_forward(size_t a_n)
	{
		if (!v_model) t_throwable::f_throw(L"no model.");
		t_base::f_caret_move_text_forward(a_n);
	}
	void f_caret_move_text_backward(size_t a_n)
	{
		if (!v_model) t_throwable::f_throw(L"no model.");
		t_base::f_caret_move_text_backward(a_n);
	}
	void f_caret_move_line_forward(size_t a_n)
	{
		if (!v_model) t_throwable::f_throw(L"no model.");
		t_base::f_caret_move_line_forward(a_n);
	}
	void f_caret_move_line_backward(size_t a_n)
	{
		if (!v_model) t_throwable::f_throw(L"no model.");
		t_base::f_caret_move_line_backward(a_n);
	}
	void f_caret_move_line_x(size_t a_x)
	{
		if (!v_model) t_throwable::f_throw(L"no model.");
		t_base::f_caret_move_line_x(a_x);
	}
	void f_caret_move_point(size_t a_x, size_t a_y)
	{
		if (!v_model) t_throwable::f_throw(L"no model.");
		t_base::f_caret_move_point(a_x, a_y);
	}
};

struct t_extension : xemmai::t_extension
{
	t_slot v_module_xraft;
	xemmaix::xraft::t_extension* v_xraft;
	t_slot v_symbol_size;
	t_slot_of<t_type> v_type_attribute;
	t_slot_of<t_type> v_type_text_model;
	t_slot_of<t_type> v_type_wrapped_view;

	t_extension(xemmai::t_object* a_module, t_scoped&& a_xraft);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	const T* f_extension() const
	{
		return v_xraft->f_extension<T>();
	}
	template<typename T>
	t_slot_of<t_type>& f_type_slot()
	{
		return v_xraft->f_type_slot<T>();
	}
	template<typename T>
	t_type* f_type() const
	{
		return const_cast<t_extension*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_scoped f_as(T&& a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(f_extension<typename t::t_extension>(), std::forward<T>(a_value));
	}
};

template<>
inline const t_extension* t_extension::f_extension<t_extension>() const
{
	return this;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_attribute>()
{
	return v_type_attribute;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_text_model>()
{
	return v_type_text_model;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_wrapped_view>()
{
	return v_type_wrapped_view;
}

}

namespace xemmai
{

template<>
struct t_type_of<xemmaix::xrafttv::t_attribute> : xemmaix::xraft::t_derivable<t_bears<xemmaix::xrafttv::t_attribute, t_type_of<xraft::t_object>>>
{
	typedef xemmaix::xrafttv::t_extension t_extension;

	static t_scoped f_construct(t_type* a_class, xraft::t_pixel a_foreground, xraft::t_pixel a_background)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xemmaix::xrafttv::t_attribute(a_foreground, a_background));
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
};

template<>
struct t_type_of<xemmaix::xrafttv::t_text_model> : xemmaix::xraft::t_derivable<t_bears<xemmaix::xrafttv::t_text_model, t_type_of<xraft::t_object>>>
{
	typedef xemmaix::xrafttv::t_extension t_extension;

	static t_scoped f_construct(t_type* a_class)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xemmaix::xrafttv::t_text_model());
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
};

template<>
struct t_type_of<xemmaix::xrafttv::t_wrapped_view> : xemmaix::xraft::t_derivable<t_bears<xemmaix::xrafttv::t_wrapped_view, t_type_of<xraft::t_object>>>
{
	typedef xemmaix::xrafttv::t_extension t_extension;

	static t_scoped f_construct(t_type* a_class)
	{
		return xemmaix::xraft::t_proxy::f_construct(a_class, new xemmaix::xrafttv::t_wrapped_view());
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
};

}

#endif
