#ifndef XRAFT__INPUT_CONTEXT_WIN32_H
#define XRAFT__INPUT_CONTEXT_WIN32_H

#include <vector>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

namespace xraft
{

class t_window;

class t_input_context : public t_object
{
	friend class t_window;

	HIMC v_himc;

protected:
	XRAFT__EXPORT virtual ~t_input_context();

public:
	t_input_context() : v_himc(ImmCreateContext())
	{
	}
	bool f_open() const
	{
		return ImmGetOpenStatus(v_himc) != FALSE;
	}
	void f_open__(bool a_open)
	{
		ImmSetOpenStatus(v_himc, a_open ? TRUE : FALSE);
	}
	XRAFT__EXPORT void f_composition(std::vector<wchar_t>& a_cs, std::vector<t_input_attribute>& a_as);
};

}

#endif
