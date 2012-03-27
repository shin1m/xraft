#ifndef XRAFT__PIXMAP_H
#define XRAFT__PIXMAP_H

#include "application.h"

namespace xraft
{

class t_bitmap : public t_drawable
{
	unsigned v_width;
	unsigned v_height;

protected:
	virtual ~t_bitmap();

public:
	XRAFT__EXPORT t_bitmap(unsigned a_width, unsigned a_height, const char* a_data = 0, int a_bytes_per_line = 0);
	unsigned f_width() const
	{
		return v_width;
	}
	unsigned f_height() const
	{
		return v_height;
	}
};

class t_pixmap : public t_drawable
{
	unsigned v_width;
	unsigned v_height;
	bool v_alpha;

protected:
	virtual ~t_pixmap();

public:
	XRAFT__EXPORT t_pixmap(unsigned a_width, unsigned a_height, bool a_alpha = false, const char* a_data = 0, int a_bytes_per_line = 0);
	unsigned f_width() const
	{
		return v_width;
	}
	unsigned f_height() const
	{
		return v_height;
	}
	bool f_alpha() const
	{
		return v_alpha;
	}
};

}

#endif
