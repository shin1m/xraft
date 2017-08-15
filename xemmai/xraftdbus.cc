#include <xemmaix/dbus/connection.h>

#include "application.h"

namespace xemmaix
{

namespace xraftdbus
{

using namespace xemmai;

void f_watch(xemmaix::dbus::t_connection& a_connection)
{
	new xraft::dbus::t_bridge(a_connection);
}

class t_extension : public xemmai::t_extension
{
	template<typename T, typename T_super> friend class t_define;

public:
	t_extension(xemmai::t_object* a_module) : xemmai::t_extension(a_module)
	{
		f_define<void(*)(xemmaix::dbus::t_connection&), f_watch>(this, L"watch");
	}
	virtual void f_scan(t_scan a_scan)
	{
	}
};

}

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new xemmaix::xraftdbus::t_extension(a_module);
}
