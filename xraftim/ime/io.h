#ifndef IO_H
#define IO_H

#include <algorithm>

template<typename T_device>
class t_reader
{
	T_device& v_device;
	char v_buffer[64];
	const char* v_p;
	size_t v_n;

public:
	t_reader(T_device& a_device) : v_device(a_device), v_p(v_buffer), v_n(v_device.f_read(v_buffer, sizeof(v_buffer)))
	{
	}
	void f_read(char* a_p, size_t a_n);
	template<typename T>
	void f_read(T& a_value)
	{
		f_read(reinterpret_cast<char*>(&a_value), sizeof(T));
	}
};

template<typename T_device>
void t_reader<T_device>::f_read(char* a_p, size_t a_n)
{
	while (true) {
		size_t n = v_buffer + v_n - v_p;
		if (a_n <= n) break;
		a_p = std::copy(v_p, v_p + n, a_p);
		a_n -= n;
		v_p = v_buffer;
		v_n = v_device.f_read(v_buffer, sizeof(v_buffer));
	}
	std::copy(v_p, v_p + a_n, a_p);
	v_p += a_n;
}

template<typename T_device>
class t_writer
{
	T_device& v_device;
	char v_buffer[64];
	DWORD v_n;

public:
	t_writer(T_device& a_device) : v_device(a_device), v_n(0)
	{
	}
	~t_writer()
	{
		v_device.f_write(v_buffer, v_n);
	}
	void f_write(const char* a_p, size_t a_n);
	template<typename T>
	void f_write(const T& a_value)
	{
		f_write(reinterpret_cast<const char*>(&a_value), sizeof(T));
	}
};

template<typename T_device>
void t_writer<T_device>::f_write(const char* a_p, size_t a_n)
{
	while (true) {
		size_t n = sizeof(v_buffer) - v_n;
		if (a_n <= n) break;
		std::copy(a_p, a_p + n, v_buffer + v_n);
		v_device.f_write(v_buffer, sizeof(v_buffer));
		v_n = 0;
		a_p += n;
		a_n -= n;
	}
	std::copy(a_p, a_p + a_n, v_buffer + v_n);
	v_n += a_n;
}

#endif
