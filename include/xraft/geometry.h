#ifndef XRAFT__GEOMETRY_H
#define XRAFT__GEOMETRY_H

#include <cstddef>

namespace xraft
{

struct t_point
{
	int v_x;
	int v_y;

	t_point() = default;
	t_point(int a_x, int a_y) : v_x(a_x), v_y(a_y)
	{
	}
	bool operator==(const t_point& a_x) const
	{
		return v_x == a_x.v_x && v_y == a_x.v_y;
	}
};

struct t_extent
{
	unsigned v_width;
	unsigned v_height;

	t_extent() = default;
	t_extent(unsigned a_width, unsigned a_height) : v_width(a_width), v_height(a_height)
	{
	}
	bool operator==(const t_extent& a_x) const
	{
		return v_width == a_x.v_width && v_height == a_x.v_height;
	}
};

struct t_rectangle : t_point, t_extent
{
	t_rectangle() = default;
	t_rectangle(const t_point& a_point, const t_extent& a_extent) : t_point(a_point), t_extent(a_extent)
	{
	}
	t_rectangle(int a_x, int a_y, unsigned a_width, unsigned a_height) : t_point(a_x, a_y), t_extent(a_width, a_height)
	{
	}
	bool operator==(const t_rectangle& a_x) const
	{
		return static_cast<const t_point&>(*this) == a_x && static_cast<const t_extent&>(*this) == a_x;
	}
};

}

#endif
