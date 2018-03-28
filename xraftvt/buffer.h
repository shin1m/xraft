#ifndef XRAFTVT__BUFFER_H
#define XRAFTVT__BUFFER_H

#include <algorithm>

template<typename T_host>
struct t_buffer
{
	struct t_attribute
	{
		bool v_bold : 1;
		bool v_faint : 1;
		bool v_underlined : 1;
		bool v_blink : 1;
		bool v_inverse : 1;
		unsigned char v_foreground : 4;
		unsigned char v_background : 4;

		bool operator==(const t_attribute& a_x) const
		{
			return v_bold == a_x.v_bold && v_faint == a_x.v_faint && v_underlined == a_x.v_underlined && v_blink == a_x.v_blink && v_inverse == a_x.v_inverse && v_foreground == a_x.v_foreground && v_background == a_x.v_background;
		}
	};
	struct t_cell
	{
		wchar_t v_c;
		t_attribute v_a;
	};
	struct t_row
	{
		unsigned v_size = 0;
		bool v_wrapped = false;
		t_cell v_cells[1];

		t_row() = default;
		t_row(unsigned a_size, const t_cell* a_cells) : v_size(a_size)
		{
			std::copy(a_cells, a_cells + a_size, v_cells);
		}
		void f_split(unsigned a_x)
		{
			if (a_x >= v_size || v_cells[a_x].v_c != L'\0') return;
			do --a_x; while (v_cells[a_x].v_c == L'\0');
			do v_cells[a_x++].v_c = L' '; while (a_x < v_size && v_cells[a_x].v_c == L'\0');
		}
		void f_expand(unsigned a_size)
		{
			if (a_size <= v_size) return;
			std::fill(v_cells + v_size, v_cells + a_size, t_cell{L' ', {false, false, false, false, false, 0, 1}});
			v_size = a_size;
		}
		void f_insert(unsigned a_x, unsigned a_n, unsigned a_width)
		{
			if (v_size + a_n > a_width) {
				f_split(a_width - a_n);
				v_size = a_width - a_n;
			}
			std::copy_backward(v_cells + a_x, v_cells + v_size, v_cells + v_size + a_n);
			v_size += a_n;
		}
	};

	T_host& v_host;

private:
	struct t_state
	{
		t_row** v_rows;
		t_attribute v_saved_attribute;
		unsigned v_saved_cursor_x;
		unsigned v_saved_cursor_y;
	};

	unsigned v_log_capacity;
	unsigned v_width;
	unsigned v_height;
	char* v_buffer;
	t_row** v_log;
	unsigned v_log_begin = 0;
	unsigned v_log_size = 0;
	t_state v_normal;
	t_state v_alternate;
	t_state* v_current = &v_normal;

protected:
	t_attribute v_attribute{false, false, false, false, false, 0, 1};
	unsigned v_cursor_x = 0;
	unsigned v_cursor_y = 0;

	bool f_alternate() const
	{
		return v_current == &v_alternate;
	}
	void f_alternate__(bool a_on)
	{
		v_current = a_on ? &v_alternate : &v_normal;
		v_host.f_invalidate(0, v_height);
	}
	void f_save_cursor()
	{
		v_current->v_saved_attribute = v_attribute;
		v_current->v_saved_cursor_x = v_cursor_x;
		v_current->v_saved_cursor_y = v_cursor_y;
	}
	void f_restore_cursor()
	{
		v_attribute = v_current->v_saved_attribute;
		v_cursor_x = v_current->v_saved_cursor_x;
		v_cursor_y = v_current->v_saved_cursor_y;
	}
	void f_scroll_log()
	{
		v_host.f_invalidate(v_cursor_y, 1);
		bool append = v_log_size < v_log_capacity;
		t_row* row;
		auto rows = v_current->v_rows;
		if (append) {
			row = v_log[v_log_size];
			v_log[v_log_size] = rows[0];
			++v_log_size;
		} else {
			row = v_log[v_log_begin];
			v_log[v_log_begin] = rows[0];
			++v_log_begin;
			v_log_begin %= v_log_capacity;
		}
		std::copy(rows + 1, rows + v_height, rows);
		row->v_size = 0;
		row->v_wrapped = false;
		rows[v_height - 1] = row;
		v_host.f_scroll(0, v_height, -1);
		v_host.f_invalidate(v_cursor_y, 1);
		if (append) v_host.f_log();
	}
	void f_scroll_up(unsigned a_y, unsigned a_height, unsigned a_n)
	{
		if ((a_n = std::min(a_n, a_height)) <= 0) return;
		auto rows = v_current->v_rows + a_y;
		std::rotate(rows, rows + a_n, rows + a_height);
		for (unsigned i = a_height - a_n; i < a_height; ++i) {
			rows[i]->v_size = 0;
			rows[i]->v_wrapped = false;
		}
		if (v_cursor_y >= a_y && v_cursor_y < a_y + static_cast<int>(a_height)) {
			v_host.f_invalidate(v_cursor_y, 1);
			v_host.f_scroll(a_y, a_height, -a_n);
			v_host.f_invalidate(v_cursor_y, 1);
		} else {
			v_host.f_scroll(a_y, a_height, -a_n);
		}
	}
	void f_scroll_down(unsigned a_y, unsigned a_height, unsigned a_n)
	{
		if ((a_n = std::min(a_n, a_height)) <= 0) return;
		auto rows = v_current->v_rows + a_y;
		std::rotate(rows, rows + a_height - a_n, rows + a_height);
		for (unsigned i = 0; i < a_n; ++i) {
			rows[i]->v_size = 0;
			rows[i]->v_wrapped = false;
		}
		if (v_cursor_y >= a_y && v_cursor_y < a_y + static_cast<int>(a_height)) {
			v_host.f_invalidate(v_cursor_y, 1);
			v_host.f_scroll(a_y, a_height, a_n);
			v_host.f_invalidate(v_cursor_y, 1);
		} else {
			v_host.f_scroll(a_y, a_height, a_n);
		}
	}
	void f_erase(unsigned a_y, unsigned a_height)
	{
		auto rows = v_current->v_rows + a_y;
		for (unsigned i = 0; i < a_height; ++i) {
			rows[i]->v_size = 0;
			rows[i]->v_wrapped = false;
		}
		v_host.f_invalidate(a_y, a_height);
	}
	void f_insert(unsigned a_x, unsigned a_y, unsigned a_n)
	{
		if (a_x >= v_width) return;
		t_row* row = v_current->v_rows[a_y];
		row->f_expand(a_x);
		row->f_split(a_x);
		t_cell* p = row->v_cells;
		t_cell cell{L' ', {false, false, false, false, false, 0, 1}};
		if (a_x + a_n >= v_width) {
			std::fill(p + a_x, p + v_width, cell);
			row->v_size = v_width;
		} else {
			row->f_insert(a_x, a_n, v_width);
			p += a_x;
			std::fill(p, p + a_n, cell);
		}
		v_host.f_invalidate(a_y, 1);
	}
	void f_put(unsigned a_x, unsigned a_y, const t_cell& a_cell, bool a_shift)
	{
		if (a_x >= v_width) return;
		int n = v_host.f_width(a_cell.v_c);
		if (n <= 0) return;
		t_row* row = v_current->v_rows[a_y];
		row->f_expand(a_x);
		row->f_split(a_x);
		t_cell* p = row->v_cells;
		if (a_x + n > v_width) {
			std::fill(p + a_x, p + v_width, t_cell{L' ', a_cell.v_a});
			row->v_size = v_width;
		} else {
			if (a_shift)
				row->f_insert(a_x, n, v_width);
			else if (a_x + n < row->v_size)
				row->f_split(a_x + n);
			else
				row->v_size = a_x + n;
			p += a_x;
			*p = a_cell;
			std::fill(p + 1, p + n, t_cell{L'\0', a_cell.v_a});
		}
		v_host.f_invalidate(a_y, 1);
	}
	void f_erase(unsigned a_x, unsigned a_y, unsigned a_n, bool a_shift)
	{
		t_row* row = v_current->v_rows[a_y];
		if (a_x >= row->v_size) return;
		row->f_split(a_x);
		if (a_x + a_n >= row->v_size) {
			row->v_size = a_x;
		} else {
			row->f_split(a_x + a_n);
			t_cell* p = row->v_cells;
			if (a_shift) {
				std::copy(p + a_x + a_n, p + row->v_size, p + a_x);
				row->v_size -= a_n;
			} else {
				p += a_x;
				std::fill(p, p + a_n, t_cell{L' ', {false, false, false, false, false, 0, 1}});
			}
		}
		v_host.f_invalidate(a_y, 1);
	}

public:
	t_buffer(T_host& a_host, unsigned a_log, unsigned a_width, unsigned a_height) : v_host(a_host), v_log_capacity(a_log), v_width(a_width), v_height(a_height)
	{
		unsigned m = sizeof(t_row) + sizeof(t_cell) * v_width;
		unsigned n = v_height * 2;
		auto p = v_buffer = new char[m * (v_log_capacity + n)];
		v_log = new t_row*[v_log_capacity];
		for (unsigned i = 0; i < v_log_capacity; ++i) {
			v_log[i] = new(p) t_row();
			p += m;
		}
		auto rows = v_normal.v_rows = new t_row*[n];
		for (unsigned i = 0; i < n; ++i) {
			rows[i] = new(p) t_row();
			p += m;
		}
		v_alternate.v_rows = rows + v_height;
	}
	~t_buffer()
	{
		delete[] v_log;
		delete[] v_normal.v_rows;
		delete[] v_buffer;
	}
	unsigned f_width() const
	{
		return v_width;
	}
	unsigned f_height() const
	{
		return v_height;
	}
	unsigned f_log_size() const
	{
		return v_log_size;
	}
	const t_row* f_log(unsigned a_y) const
	{
		return v_log[(v_log_begin + a_y) % v_log_capacity];
	}
	const t_row* f_at(unsigned a_y)
	{
		return v_current->v_rows[a_y];
	}
	unsigned f_cursor_x() const
	{
		return v_cursor_x;
	}
	unsigned f_cursor_y() const
	{
		return v_cursor_y;
	}
	void f_resize(unsigned a_width, unsigned a_height)
	{
		if (a_width < 1) a_width = 1;
		if (a_height < 1) a_height = 1;
		if (a_width == v_width && a_height == v_height) return;
		unsigned m = sizeof(t_row) + sizeof(t_cell) * a_width;
		unsigned n = a_height * 2;
		auto buffer = new char[m * (v_log_capacity + n)];
		auto p = buffer;
		for (unsigned i = 0; i < v_log_capacity; ++i) {
			t_row* row = v_log[i];
			if (a_width < row->v_size) {
				row->f_split(a_width);
				v_log[i] = new(p) t_row(a_width, row->v_cells);
			} else {
				v_log[i] = new(p) t_row(row->v_size, row->v_cells);
			}
			p += m;
		}
		unsigned h = std::min(v_height, a_height);
		auto normal = new t_row*[n];
		for (unsigned i = 0; i < h; ++i) {
			t_row* row = v_normal.v_rows[i];
			if (a_width < row->v_size) {
				row->f_split(a_width);
				normal[i] = new(p) t_row(a_width, row->v_cells);
			} else {
				normal[i] = new(p) t_row(row->v_size, row->v_cells);
			}
			p += m;
		}
		for (unsigned i = h; i < a_height; ++i) {
			normal[i] = new(p) t_row();
			p += m;
		}
		auto alternate = normal + a_height;
		for (unsigned i = 0; i < h; ++i) {
			t_row* row = v_alternate.v_rows[i];
			if (a_width < row->v_size) {
				row->f_split(a_width);
				alternate[i] = new(p) t_row(a_width, row->v_cells);
			} else {
				alternate[i] = new(p) t_row(row->v_size, row->v_cells);
			}
			p += m;
		}
		for (unsigned i = h; i < a_height; ++i) {
			alternate[i] = new(p) t_row();
			p += m;
		}
		delete[] v_normal.v_rows;
		delete[] v_buffer;
		v_width = a_width;
		v_height = a_height;
		v_buffer = buffer;
		v_normal.v_rows = normal;
		v_alternate.v_rows = alternate;
		if (v_cursor_x >= v_width) v_cursor_x = v_width - 1;
		if (v_cursor_y >= v_height) v_cursor_y = v_height - 1;
	}
};

#endif
