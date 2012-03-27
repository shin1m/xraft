#ifndef XRAFT__WRAPPED_VIEW_H
#define XRAFT__WRAPPED_VIEW_H

#include <cwctype>

#include "text_model.h"

namespace xraft
{

template<typename T_attribute, typename T_font>
class t_wrapped_view : t_text_model<T_attribute>::t_observer
{
public:
	struct t_observer
	{
		virtual void f_invalidated(size_t a_y, size_t a_height) = 0;
		virtual void f_resized() = 0;
		virtual void f_moved(size_t a_y, size_t a_height, size_t a_delta) = 0;
	};
	struct t_caret
	{
		size_t v_text;
		size_t v_row;
		size_t v_line;
		size_t v_row_x;
		size_t v_line_x;

		t_caret() : v_text(0), v_row(0), v_line(0), v_row_x(0), v_line_x(0)
		{
		}
	};

private:
	struct t_x
	{
		size_t v_text;
		size_t v_pixel;

		t_x(size_t a_text = 0, size_t a_pixel = 0) : v_text(a_text), v_pixel(a_pixel)
		{
		}
	};
	typedef gapped<t_x> t_rows;
	typedef gapped<size_t> t_lines;
	class t_convert
	{
		size_t v_size;

	public:
		t_convert(size_t a_size) : v_size(a_size)
		{
		}
		t_x operator()(const t_x& a_x) const
		{
			return t_x(v_size - a_x.v_text, a_x.v_pixel);
		}
		size_t operator()(size_t a_x) const
		{
			return v_size - a_x;
		}
	};
	friend class t_convert;

	std::list<t_observer*> v_observers;
	T_font v_font;
	t_text_model<T_attribute>* v_model;
	size_t v_width;
	t_rows v_rows;
	t_lines v_lines;
	t_caret v_caret;
	size_t v_preferred_line_x;
	bool v_caret_visible;
	t_caret v_work;
	size_t v_rows_size;
	size_t v_first;
	size_t v_last;

	size_t f_character_width(wchar_t a_c) const
	{
		return std::iswcntrl(a_c) ? v_font.f_width(L'^') + v_font.f_width(a_c + L'@') : v_font.f_width(a_c);
	}
	size_t f_tab_width(size_t a_x) const
	{
		size_t w0 = v_font.f_width();
		if (a_x + w0 > v_width) return w0;
		size_t w1 = w0 << 2;
		return std::min(v_width, ((a_x + w0 - 1) / w1 + 1) * w1) - a_x;
	}
	size_t f_line_to_row_(size_t a_line) const
	{
		return a_line < v_lines.gap_index() ? v_lines[a_line] : v_rows.size() - v_lines[a_line];
	}
	size_t f_row_to_text_(size_t a_row) const
	{
		return a_row < v_rows.gap_index() ? v_rows[a_row].v_text : v_model->f_text_size() - v_rows[a_row].v_text;
	}
	void f_layout();
	void f_resize();
	void f_caret_adjust_pixel(t_caret& a_caret) const;
	void f_caret_move_row_x_forward(t_caret& a_caret, size_t a_preferred_x) const;
	void f_caret_move_line_x_forward(t_caret& a_caret, size_t a_preferred_x) const;

protected:
	void f_fire_invalidated(size_t a_y, size_t a_height)
	{
		for (typename std::list<t_observer*>::iterator i = v_observers.begin(); i != v_observers.end(); ++i) (*i)->f_invalidated(a_y, a_height);
	}
	void f_fire_resized()
	{
		for (typename std::list<t_observer*>::iterator i = v_observers.begin(); i != v_observers.end(); ++i) (*i)->f_resized();
	}
	void f_fire_moved(size_t a_y, size_t a_height, int a_delta)
	{
		for (typename std::list<t_observer*>::iterator i = v_observers.begin(); i != v_observers.end(); ++i) (*i)->f_moved(a_y, a_height, a_delta);
	}
	virtual void f_loaded();
	virtual void f_replacing(size_t a_p, size_t a_n);
	virtual void f_replaced(size_t a_n);

public:
	t_wrapped_view(const T_font& a_font) : v_font(a_font), v_model(0), v_width(0), v_preferred_line_x(0), v_caret_visible(false)
	{
	}
	~t_wrapped_view()
	{
		if (v_model) v_model->f_remove_observer(this);
	}
	void f_add_observer(t_observer* a_observer)
	{
		v_observers.push_back(a_observer);
	}
	void f_remove_observer(t_observer* a_observer)
	{
		v_observers.remove(a_observer);
	}
	template<typename T_context>
	void f_draw(T_context& a_context, size_t a_top, size_t a_bottom);
	const T_font& f_font() const
	{
		return v_font;
	}
	void f_font__(const T_font& a_font)
	{
		if (a_font == v_font) return;
		v_font = a_font;
		f_resize();
	}
	t_text_model<T_attribute>* f_model() const
	{
		return v_model;
	}
	void f_model__(t_text_model<T_attribute>* a_model);
	size_t f_width() const
	{
		return v_width;
	}
	void f_width__(size_t a_width)
	{
		if (a_width == v_width) return;
		v_width = a_width;
		f_resize();
	}
	size_t f_height() const
	{
		return v_rows.size() * v_font.f_height();
	}
	bool f_caret_visible() const
	{
		return v_caret_visible;
	}
	void f_caret_visible__(bool a_visible)
	{
		if (a_visible == v_caret_visible) return;
		v_caret_visible = a_visible;
		size_t h = v_font.f_height();
		f_fire_invalidated(v_caret.v_row * h, h);
	}
	const t_caret& f_caret() const
	{
		return v_caret;
	}
	size_t f_rows_size() const
	{
		return v_rows.size();
	}
	size_t f_lines_size() const
	{
		return v_lines.size();
	}
	size_t f_line_to_row(size_t a_line) const
	{
		return a_line < v_lines.size() ? f_line_to_row_(a_line) : v_rows.size();
	}
	size_t f_row_to_text(size_t a_row) const
	{
		return a_row < v_rows.size() ? f_row_to_text_(a_row) : v_model->f_text_size();
	}
	size_t f_line_to_text(size_t a_line) const
	{
		return a_line < v_lines.size() ? f_row_to_text_(f_line_to_row_(a_line)) : v_model->f_text_size();
	}
	void f_caret_move_text_forward(t_caret& a_caret, size_t a_n) const;
	void f_caret_move_text_backward(t_caret& a_caret, size_t a_n) const;
	void f_caret_move_line_forward(t_caret& a_caret, size_t a_n) const;
	void f_caret_move_line_backward(t_caret& a_caret, size_t a_n) const;
	void f_caret_move_point(t_caret& a_caret, size_t a_x, size_t a_y) const;
	void f_caret_move_text(const typename t_text_model<T_attribute>::t_iterator& a_i)
	{
		f_caret_move_text(a_i - v_model->f_begin());
	}
	void f_caret_move_text(size_t a_text);
	void f_caret_move_text_forward(size_t a_n);
	void f_caret_move_text_backward(size_t a_n);
	void f_caret_move_line_forward(size_t a_n);
	void f_caret_move_line_backward(size_t a_n);
	void f_caret_move_line_x(size_t a_x);
	void f_caret_move_point(size_t a_x, size_t a_y);
};

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_layout()
{
	v_rows_size = v_rows.size();
	v_rows.clear();
	v_lines.clear();
	v_lines.insert_forward(0);
	size_t text = 0;
	size_t row = 0;
	t_x x;
	typename t_text_model<T_attribute>::t_iterator mi = v_model->f_begin();
	typename t_text_model<T_attribute>::t_iterator mend = v_model->f_end();
	while (mi != mend) {
		wchar_t c = *mi;
		size_t w = c == L'\t' ? f_tab_width(x.v_pixel) : c == L'\n' ? v_font.f_width() : f_character_width(c);
		if (x.v_pixel + w > v_width && text > x.v_text) {
			v_rows.insert_forward(x);
			++row;
			x = t_x(text);
			continue;
		} else if (c == L'\n') {
			v_rows.insert_forward(x);
			v_lines.insert_forward(++row);
			x = t_x(++text);
		} else {
			++text;
			x.v_pixel += w;
		}
		++mi;
	}
	v_rows.insert_forward(x);
	v_caret = v_work = t_caret();
	v_preferred_line_x = 0;
	size_t h = v_font.f_height();
	if (v_rows_size < v_rows.size()) {
		int dy = (v_rows.size() - v_rows_size) * h;
		f_fire_moved(v_rows.size() * h - dy, dy, dy);
		f_fire_invalidated(0, v_rows.size() * h);
	} else {
		int dy = (v_rows_size - v_rows.size()) * h;
		f_fire_invalidated(0, v_rows.size() * h);
		f_fire_moved(v_rows.size() * h, dy, -dy);
	}
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_resize()
{
	if (!v_model) return;
	size_t text = v_caret.v_text;
	f_layout();
	f_caret_move_text_forward(v_caret, text);
	f_fire_resized();
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_caret_adjust_pixel(t_caret& a_caret) const
{
	a_caret.v_line_x = 0;
	for (size_t row = f_line_to_row_(a_caret.v_line); row < a_caret.v_row; ++row) a_caret.v_line_x += v_rows[row].v_pixel;
	if (a_caret.v_row > f_line_to_row_(a_caret.v_line) && a_caret.v_text == f_row_to_text_(a_caret.v_row)) {
		a_caret.v_row_x = v_rows[--a_caret.v_row].v_pixel;
		return;
	}
	a_caret.v_row_x = 0;
	size_t text = f_row_to_text_(a_caret.v_row);
	typename t_text_model<T_attribute>::t_iterator mi = v_model->f_begin() + text;
	while (text < a_caret.v_text) {
		wchar_t c = *mi;
		a_caret.v_row_x += c == L'\t' ? f_tab_width(a_caret.v_row_x) : f_character_width(c);
		++text;
		++mi;
	}
	a_caret.v_line_x += a_caret.v_row_x;
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_caret_move_row_x_forward(t_caret& a_caret, size_t a_preferred_x) const
{
	a_caret.v_row_x = 0;
	size_t text = f_row_to_text(a_caret.v_row + 1);
	size_t row = a_caret.v_row + 1;
	if (row < v_rows.size() && row >= f_line_to_row(a_caret.v_line + 1)) --text;
	a_caret.v_text = f_row_to_text_(a_caret.v_row);
	typename t_text_model<T_attribute>::t_iterator mi = v_model->f_begin() + a_caret.v_text;
	while (a_caret.v_text < text) {
		wchar_t c = *mi;
		size_t w = c == L'\t' ? f_tab_width(a_caret.v_row_x) : f_character_width(c);
		if (a_caret.v_row_x + w > a_preferred_x) break;
		a_caret.v_row_x += w;
		++a_caret.v_text;
		++mi;
	}
	if (a_caret.v_row > f_line_to_row_(a_caret.v_line) && a_caret.v_text == f_row_to_text_(a_caret.v_row))
		a_caret.v_row_x = v_rows[--a_caret.v_row].v_pixel;
	else
		a_caret.v_line_x += a_caret.v_row_x;
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_caret_move_line_x_forward(t_caret& a_caret, size_t a_preferred_x) const
{
	a_caret.v_line_x = 0;
	size_t row = f_line_to_row(a_caret.v_line + 1) - 1;
	a_caret.v_row = f_line_to_row_(a_caret.v_line);
	while (true) {
		size_t w = v_rows[a_caret.v_row].v_pixel;
		if (a_caret.v_line_x + w > a_preferred_x) break;
		a_caret.v_line_x += w;
		if (a_caret.v_row >= row) {
			a_caret.v_text = f_row_to_text(a_caret.v_row + 1);
			if (a_caret.v_row + 1 < v_rows.size()) --a_caret.v_text;
			a_caret.v_row_x = w;
			return;
		}
		++a_caret.v_row;
	}
	a_caret.v_text = f_row_to_text_(a_caret.v_row);
	f_caret_move_row_x_forward(a_caret, a_preferred_x - a_caret.v_line_x);
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_loaded()
{
	f_layout();
	f_fire_resized();
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_replacing(size_t a_p, size_t a_n)
{
	if (v_caret_visible) {
		size_t h = v_font.f_height();
		f_fire_invalidated(v_caret.v_row * h, h);
	}
	if (a_p < v_work.v_text)
		f_caret_move_text_backward(v_work, v_work.v_text - a_p);
	else
		f_caret_move_text_forward(v_work, a_p - v_work.v_text);
	if (v_work.v_row < v_rows.gap_index())
		v_rows.move_backward(v_rows.gap_index() - v_work.v_row, t_convert(v_model->f_text_size()));
	else
		v_rows.move_forward(v_work.v_row - v_rows.gap_index(), t_convert(v_model->f_text_size()));
	v_first = v_model->f_text_size() - v_rows.gap()->v_text;
	if (v_work.v_line + 1 < v_lines.gap_index())
		v_lines.move_backward(v_lines.gap_index() - v_work.v_line - 1, t_convert(v_rows.size()));
	else
		v_lines.move_forward(v_work.v_line + 1 - v_lines.gap_index(), t_convert(v_rows.size()));
	t_caret caret = v_work;
	f_caret_move_text_forward(caret, a_n);
	v_rows_size = v_rows.size();
	v_rows.erase_forward(caret.v_row - v_work.v_row + 1);
	v_lines.erase_forward(caret.v_line - v_work.v_line);
	v_last = caret.v_text;
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_replaced(size_t a_n)
{
	size_t text = v_work.v_text;
	t_caret caret = v_work;
	caret.v_line_x -= caret.v_row_x;
	t_x x(v_first, caret.v_row_x);
	typename t_text_model<T_attribute>::t_iterator mi = v_model->f_begin() + text;
	while (a_n > 0) {
		wchar_t c = *mi;
		size_t w = c == L'\t' ? f_tab_width(x.v_pixel) : c == L'\n' ? v_font.f_width() : f_character_width(c);
		if (x.v_pixel + w > v_width && text > x.v_text) {
			v_rows.insert_forward(x);
			++caret.v_row;
			caret.v_line_x += x.v_pixel;
			x = t_x(text);
			continue;
		} else if (c == L'\n') {
			v_rows.insert_forward(x);
			v_lines.insert_forward(++caret.v_row);
			++caret.v_line;
			caret.v_line_x = 0;
			x = t_x(++text);
		} else {
			++text;
			x.v_pixel += w;
		}
		++mi;
		--a_n;
	}
	caret.v_text = text;
	caret.v_line_x += caret.v_row_x = x.v_pixel;
	typename t_text_model<T_attribute>::t_iterator mend = v_model->f_end();
	while (true) {
		if (mi == mend) {
			v_rows.erase_forward(v_rows.size() - v_rows.gap_index());
			break;
		}
		wchar_t c = *mi;
		size_t w = c == L'\t' ? f_tab_width(x.v_pixel) : c == L'\n' ? v_font.f_width() : f_character_width(c);
		if (x.v_pixel + w > v_width) {
			typename t_rows::const_iterator ri = v_rows.gap();
			typename t_rows::const_iterator rend = v_rows.end();
			size_t text0;
			while (ri != rend && (text0 = v_model->f_text_size() - ri->v_text) < text) ++ri;
			v_rows.erase_forward(ri - v_rows.gap());
			if (ri != rend && text0 == text) break;
			v_rows.insert_forward(x);
			x = t_x(text);
			continue;
		} else if (c == L'\n') {
			v_rows.erase_forward(v_rows.size() - v_rows.gap_index() - (v_lines.gap_index() < v_lines.size() ? *v_lines.gap() : 0));
			break;
		}
		++text;
		x.v_pixel += w;
		++mi;
	}
	v_rows.insert_forward(x);
	size_t h = v_font.f_height();
	size_t tail = (v_rows.size() - v_rows.gap_index()) * h;
	if (v_rows_size < v_rows.size()) {
		int dy = (v_rows.size() - v_rows_size) * h;
		f_fire_moved(v_rows.gap_index() * h - dy, tail + dy, dy);
		f_fire_invalidated(v_work.v_row * h, (v_rows.gap_index() - v_work.v_row) * h);
	} else {
		int dy = (v_rows_size - v_rows.size()) * h;
		f_fire_invalidated(v_work.v_row * h, (v_rows.gap_index() - v_work.v_row) * h);
		f_fire_moved(v_rows.gap_index() * h, tail + dy, -dy);
	}
	if (v_caret.v_text > v_last) {
		size_t n = v_caret.v_text - v_last;
		v_caret = caret;
		f_caret_move_text_forward(v_caret, n);
	} else if (v_caret.v_text >= v_work.v_text) {
		v_caret = caret;
	}
	v_preferred_line_x = v_caret.v_line_x;
	f_fire_resized();
	if (v_caret_visible) f_fire_invalidated(v_caret.v_row * h, h);
}

template<typename T_attribute, typename T_font>
template<typename T_context>
void t_wrapped_view<T_attribute, T_font>::f_draw(T_context& a_context, size_t a_top, size_t a_bottom)
{
	if (!v_model) return;
	size_t h = v_font.f_height();
	int y = a_top / static_cast<int>(h);
	if (y >= static_cast<int>(v_rows.size())) {
		a_context.f_draw_page_end(a_top, v_width, a_bottom - a_top);
		return;
	}
	typename t_rows::const_iterator ri = v_rows.begin() + y;
	typename t_rows::const_iterator rgap = v_rows.gap();
	typename t_rows::const_iterator rend = v_rows.end();
	size_t text = ri < rgap ? ri->v_text : v_model->f_text_size() - ri->v_text;
	typename t_text_model<T_attribute>::t_iterator mi = v_model->f_begin() + text;
	size_t si = v_model->f_text_to_segment(text);
	size_t sn = v_model->f_segments_size();
	if (si < sn) a_context.f_attribute(v_model->f_segment_to_attribute(si++));
	size_t text1 = si < sn ? v_model->f_segment_to_text(si) : v_model->f_text_size();
	y *= h;
	do {
		size_t text0 = ++ri == rend ? v_model->f_text_size() : ri < rgap ? ri->v_text : v_model->f_text_size() - ri->v_text;
		if (a_context.f_invalid(y, v_width, h)) {
			int x = 0;
			wchar_t cs[256];
			size_t n = 0;
			int x0 = 0;
			while (text < text0) {
				wchar_t c = *mi;
				if (c == L'\t') {
					if (n > 0) {
						a_context.f_draw_string(x0, y, x - x0, cs, n);
						n = 0;
					}
					size_t w = f_tab_width(x);
					a_context.f_draw_tab(x, y, w);
					x += w;
					x0 = x;
				} else if (c == L'\n') {
					if (n > 0) {
						a_context.f_draw_string(x0, y, x - x0, cs, n);
						n = 0;
					}
					a_context.f_draw_line_terminator(x, y);
					x += v_font.f_width();
					x0 = x;
				} else {
					if (c < 256 && iscntrl(c)) {
						cs[n] = L'^';
						x += v_font.f_width(L'^');
						if (++n >= sizeof(cs) / sizeof(wchar_t)) {
							a_context.f_draw_string(x0, y, x - x0, cs, n);
							n = 0;
							x0 = x;
						}
						c += L'@';
					}
					cs[n] = c;
					x += v_font.f_width(c);
					if (++n >= sizeof(cs) / sizeof(wchar_t)) {
						a_context.f_draw_string(x0, y, x - x0, cs, n);
						n = 0;
						x0 = x;
					}
				}
				++mi;
				if (++text >= text1) {
					if (n > 0) {
						a_context.f_draw_string(x0, y, x - x0, cs, n);
						n = 0;
						x0 = x;
					}
					if (si >= sn) break;
					a_context.f_attribute(v_model->f_segment_to_attribute(si));
					text1 = ++si < sn ? v_model->f_segment_to_text(si) : v_model->f_text_size();
				}
			}
			if (n > 0) a_context.f_draw_string(x0, y, x - x0, cs, n);
			a_context.f_draw_line_end(x, y, v_width - x);
		} else {
			mi += text0 - text;
			text = text0;
			while (text >= text1 && si < sn) {
				a_context.f_attribute(v_model->f_segment_to_attribute(si));
				text1 = ++si < sn ? v_model->f_segment_to_text(si) : v_model->f_text_size();
			}
		}
		y += h;
		if (ri == rend) {
			if (y < static_cast<int>(a_bottom) && a_context.f_invalid(y, v_width, a_bottom - y)) a_context.f_draw_page_end(y, v_width, a_bottom - y);
			break;
		}
	} while (y < static_cast<int>(a_bottom));
	if (v_caret_visible && a_context.f_invalid(v_caret.v_row * h, v_width, h)) a_context.f_draw_caret(v_caret.v_row_x, v_caret.v_row * h);
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_model__(t_text_model<T_attribute>* a_model)
{
	if (a_model == v_model) return;
	if (v_model) {
		v_model->f_remove_observer(this);
		v_rows.clear();
		v_lines.clear();
	}
	v_model = a_model;
	if (v_model) {
		v_model->f_add_observer(this);
		f_loaded();
	}
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_caret_move_text_forward(t_caret& a_caret, size_t a_n) const
{
	a_caret.v_text += std::min(a_n, v_model->f_text_size() - a_caret.v_text);
	do ++a_caret.v_row; while (a_caret.v_row < v_rows.size() && f_row_to_text_(a_caret.v_row) <= a_caret.v_text);
	--a_caret.v_row;
	do ++a_caret.v_line; while (a_caret.v_line < v_lines.size() && f_line_to_row_(a_caret.v_line) <= a_caret.v_row);
	--a_caret.v_line;
	f_caret_adjust_pixel(a_caret);
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_caret_move_text_backward(t_caret& a_caret, size_t a_n) const
{
	a_caret.v_text -= std::min(a_n, a_caret.v_text);
	while (f_row_to_text_(a_caret.v_row) > a_caret.v_text) --a_caret.v_row;
	while (f_line_to_row_(a_caret.v_line) > a_caret.v_row) --a_caret.v_line;
	f_caret_adjust_pixel(a_caret);
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_caret_move_line_forward(t_caret& a_caret, size_t a_n) const
{
	a_caret.v_line += std::min(a_n, v_lines.size() - 1 - a_caret.v_line);
	f_caret_move_line_x_forward(a_caret, v_preferred_line_x);
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_caret_move_line_backward(t_caret& a_caret, size_t a_n) const
{
	a_caret.v_line -= std::min(a_n, a_caret.v_line);
	f_caret_move_line_x_forward(a_caret, v_preferred_line_x);
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_caret_move_point(t_caret& a_caret, size_t a_x, size_t a_y) const
{
	size_t row = std::min(v_rows.size() - 1, static_cast<size_t>(a_y) / v_font.f_height());
	if (row < v_caret.v_row) {
		while (f_line_to_row_(a_caret.v_line) > row) --a_caret.v_line;
	} else {
		do ++a_caret.v_line; while (a_caret.v_line < v_lines.size() && f_line_to_row_(a_caret.v_line) <= row);
		--a_caret.v_line;
	}
	a_caret.v_row = f_line_to_row_(a_caret.v_line);
	a_caret.v_line_x = 0;
	while (a_caret.v_row < row) {
		a_caret.v_line_x += v_rows[a_caret.v_row].v_pixel;
		++a_caret.v_row;
	}
	f_caret_move_row_x_forward(a_caret, a_x);
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_caret_move_text(size_t a_text)
{
	if (a_text < v_caret.v_text)
		f_caret_move_text_backward(v_caret.v_text - a_text);
	else
		f_caret_move_text_forward(a_text - v_caret.v_text);
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_caret_move_text_forward(size_t a_n)
{
	size_t h = v_font.f_height();
	if (v_caret_visible) f_fire_invalidated(v_caret.v_row * h, h);
	f_caret_move_text_forward(v_caret, a_n);
	v_preferred_line_x = v_caret.v_line_x;
	if (v_caret_visible) f_fire_invalidated(v_caret.v_row * h, h);
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_caret_move_text_backward(size_t a_n)
{
	size_t h = v_font.f_height();
	if (v_caret_visible) f_fire_invalidated(v_caret.v_row * h, h);
	f_caret_move_text_backward(v_caret, a_n);
	v_preferred_line_x = v_caret.v_line_x;
	if (v_caret_visible) f_fire_invalidated(v_caret.v_row * h, h);
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_caret_move_line_forward(size_t a_n)
{
	size_t h = v_font.f_height();
	if (v_caret_visible) f_fire_invalidated(v_caret.v_row * h, h);
	f_caret_move_line_forward(v_caret, a_n);
	if (v_caret_visible) f_fire_invalidated(v_caret.v_row * h, h);
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_caret_move_line_backward(size_t a_n)
{
	size_t h = v_font.f_height();
	if (v_caret_visible) f_fire_invalidated(v_caret.v_row * h, h);
	f_caret_move_line_backward(v_caret, a_n);
	if (v_caret_visible) f_fire_invalidated(v_caret.v_row * h, h);
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_caret_move_line_x(size_t a_x)
{
	size_t h = v_font.f_height();
	if (v_caret_visible) f_fire_invalidated(v_caret.v_row * h, h);
	f_caret_move_line_x_forward(v_caret, v_preferred_line_x = a_x);
	if (v_caret_visible) f_fire_invalidated(v_caret.v_row * h, h);
}

template<typename T_attribute, typename T_font>
void t_wrapped_view<T_attribute, T_font>::f_caret_move_point(size_t a_x, size_t a_y)
{
	size_t h = v_font.f_height();
	if (v_caret_visible) f_fire_invalidated(v_caret.v_row * h, h);
	f_caret_move_point(v_caret, a_x, a_y);
	v_preferred_line_x = v_caret.v_line_x - v_caret.v_row_x + a_x;
	if (v_caret_visible) f_fire_invalidated(v_caret.v_row * h, h);
}

}

#endif
