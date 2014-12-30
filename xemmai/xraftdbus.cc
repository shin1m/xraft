#include <dbus/xemmai/connection.h>

#include "application.h"

namespace xraft
{

namespace xemmai
{

using ::xemmai::t_object;
using ::xemmai::t_scan;

void f_watch(::dbus::xemmai::t_connection& a_connection)
{
	new dbus::t_bridge(a_connection);
}

class t_xraftdbus : public ::xemmai::t_extension
{
	template<typename T, typename T_super> friend class t_define;

public:
	t_xraftdbus(t_object* a_module) : ::xemmai::t_extension(a_module)
	{
		f_define<void (*)(::dbus::xemmai::t_connection&), f_watch>(this, L"watch");
	}
	virtual void f_scan(t_scan a_scan)
	{
	}
};

}

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new xraft::xemmai::t_xraftdbus(a_module);
}
