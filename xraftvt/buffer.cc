#include "common.h"

#include <algorithm>

t_row::t_row(unsigned a_size, const t_cell* a_cells) : v_size(a_size), v_wrapped(false)
{
	std::copy(a_cells, a_cells + a_size, v_cells);
}

void t_row::f_split(unsigned a_x)
{
	if (a_x >= v_size || v_cells[a_x].v_c != L'\0') return;
	do --a_x; while (v_cells[a_x].v_c == L'\0');
	do v_cells[a_x++].v_c = L' '; while (a_x < v_size && v_cells[a_x].v_c == L'\0');
}

void t_row::f_expand(unsigned a_size)
{
	if (a_size <= v_size) return;
	std::fill(v_cells + v_size, v_cells + a_size, t_cell(L' ', ::t_attribute(false, false, false, false, 0, 7)));
	v_size = a_size;
}

void t_row::f_insert(unsigned a_x, unsigned a_n, unsigned a_width)
{
	if (v_size + a_n > a_width) {
		f_split(a_width - a_n);
		v_size = a_width - a_n;
	}
	std::copy_backward(v_cells + a_x, v_cells + v_size, v_cells + v_size + a_n);
	v_size += a_n;
}

t_buffer::t_buffer(unsigned a_log, unsigned a_width, unsigned a_height) :
v_log_capacity(a_log), v_width(a_width), v_height(a_height),
v_log_begin(0), v_log_size(0)
{
	unsigned m = sizeof(t_row) + sizeof(t_cell) * v_width;
	unsigned n = v_height * 2;
	char* p = v_buffer = new char[m * (v_log_capacity + n)];
	v_log = new t_row*[v_log_capacity];
	for (unsigned i = 0; i < v_log_capacity; ++i) {
		v_log[i] = new(p) t_row();
		p += m;
	}
	v_normal = v_current = new t_row*[n];
	for (unsigned i = 0; i < n; ++i) {
		v_normal[i] = new(p) t_row();
		p += m;
	}
	v_alternate = v_normal + v_height;
}

t_buffer::~t_buffer()
{
	delete[] v_log;
	delete[] v_normal;
	delete[] v_buffer;
}

void t_buffer::f_resize(unsigned a_width, unsigned a_height)
{
	if (a_width < 1) a_width = 1;
	if (a_height < 1) a_height = 1;
	if (a_width == v_width && a_height == v_height) return;
	unsigned m = sizeof(t_row) + sizeof(t_cell) * a_width;
	unsigned n = a_height * 2;
	char* buffer = new char[m * (v_log_capacity + n)];
	char* p = buffer;
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
	t_row** normal = new t_row*[n];
	for (unsigned i = 0; i < h; ++i) {
		t_row* row = v_normal[i];
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
	t_row** alternate = normal + a_height;
	for (unsigned i = 0; i < h; ++i) {
		t_row* row = v_alternate[i];
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
	bool b = f_alternate();
	delete[] v_normal;
	delete[] v_buffer;
	v_width = a_width;
	v_height = a_height;
	v_buffer = buffer;
	v_normal = normal;
	v_alternate = alternate;
	f_alternate__(b);
}

void t_buffer::f_scroll_log()
{
	t_row* p;
	if (v_log_size < v_log_capacity) {
		p = v_log[v_log_size];
		v_log[v_log_size] = v_current[0];
		++v_log_size;
	} else {
		p = v_log[v_log_begin];
		v_log[v_log_begin] = v_current[0];
		++v_log_begin;
		v_log_begin %= v_log_capacity;
	}
	std::copy(v_current + 1, v_current + v_height, v_current);
	p->v_size = 0;
	p->v_wrapped = false;
	v_current[v_height - 1] = p;
}

void t_buffer::f_scroll_up(unsigned a_y, unsigned a_height, unsigned a_n)
{
	if ((a_n = std::min(a_n, a_height)) <= 0) return;
	t_row** pp = v_current + a_y;
	std::rotate(pp, pp + a_n, pp + a_height);
	for (unsigned i = a_height - a_n; i < a_height; ++i) {
		pp[i]->v_size = 0;
		pp[i]->v_wrapped = false;
	}
}

void t_buffer::f_scroll_down(unsigned a_y, unsigned a_height, unsigned a_n)
{
	if ((a_n = std::min(a_n, a_height)) <= 0) return;
	t_row** pp = v_current + a_y;
	std::rotate(pp, pp + a_height - a_n, pp + a_height);
	for (unsigned i = 0; i < a_n; ++i) {
		pp[i]->v_size = 0;
		pp[i]->v_wrapped = false;
	}
}

void t_buffer::f_erase(unsigned a_y, unsigned a_height)
{
	t_row** pp = v_current + a_y;
	for (unsigned i = 0; i < a_height; ++i) {
		pp[i]->v_size = 0;
		pp[i]->v_wrapped = false;
	}
}

void t_buffer::f_insert(unsigned a_x, unsigned a_y, unsigned a_n)
{
	if (a_x >= v_width) return;
	t_row* row = v_current[a_y];
	row->f_expand(a_x);
	row->f_split(a_x);
	t_cell* p = row->v_cells;
	t_cell cell(L' ', ::t_attribute(false, false, false, false, 0, 7));
	if (a_x + a_n >= v_width) {
		std::fill(p + a_x, p + v_width, cell);
		row->v_size = v_width;
	} else {
		row->f_insert(a_x, a_n, v_width);
		p += a_x;
		std::fill(p, p + a_n, cell);
	}
}

void t_buffer::f_put(unsigned a_x, unsigned a_y, const t_cell& a_cell, bool a_shift)
{
	if (a_x >= v_width) return;
	int n = wcwidth(a_cell.v_c);
	if (n <= 0) return;
	t_row* row = v_current[a_y];
	row->f_expand(a_x);
	row->f_split(a_x);
	t_cell* p = row->v_cells;
	if (a_x + n > v_width) {
		std::fill(p + a_x, p + v_width, t_cell(L' ', a_cell.v_a));
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
		std::fill(p + 1, p + n, t_cell(L'\0', a_cell.v_a));
	}
}

void t_buffer::f_erase(unsigned a_x, unsigned a_y, unsigned a_n, bool a_shift)
{
	t_row* row = v_current[a_y];
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
			std::fill(p, p + a_n, t_cell(L' ', ::t_attribute(false, false, false, false, 0, 7)));
		}
	}
}
