#include <xraft/application.h>

namespace xraft
{

t_timer::~t_timer()
{
	f_stop();
}

void t_timer::f_add()
{
	SetTimer(f_application()->v_hwnd, reinterpret_cast<UINT_PTR>(this), v_interval, NULL);
}

void t_timer::f_stop()
{
	KillTimer(f_application()->v_hwnd, reinterpret_cast<UINT_PTR>(this));
}

}
