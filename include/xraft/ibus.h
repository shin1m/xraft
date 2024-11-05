#ifndef XRAFT__IBUS_H
#define XRAFT__IBUS_H

#include "dbus.h"

namespace xraft
{

namespace ibus
{

enum
{
	c_attribute_type__UNDERLINE = 1,
	c_attribute_type__FOREGROUND = 2,
	c_attribute_type__BACKGROUND = 3
};

enum
{
	c_attribute_underline__NONE = 0,
	c_attribute_underline__SINGLE = 1,
	c_attribute_underline__DOUBLE = 2,
	c_attribute_underline__LOW = 3,
	c_attribute_underline__ERROR = 4
};

enum
{
	c_modifier__HANDLED = 1 << 24,
	c_modifier__FORWARD = 1 << 25,
	c_modifier__SUPER = 1 << 26,
	c_modifier__HYPER = 1 << 27,
	c_modifier__META = 1 << 28,
	c_modifier__RELEASE = 1 << 30
};

enum
{
	c_capability__PREEDIT_TEXT = 1 << 0,
	c_capability__AUXILIARY_TEXT = 1 << 1,
	c_capability__LOOKUP_TABLE = 1 << 2,
	c_capability__FOCUS = 1 << 3,
	c_capability__PROPERTY = 1 << 4,
	c_capability__SURROUNDING_TEXT = 1 << 5
};

enum
{
	c_engine_preedit__CLEAR = 0,
	c_engine_preedit__COMMIT = 1
};

enum
{
	c_orientation__HORIZONTAL = 0,
	c_orientation__VERTICAL = 1,
	c_orientation__SYSTEM = 2
};

std::string f_address();

class t_variant_parser
{
	DBusMessageIter v_i;

	void f_initialize(DBusMessageIter& a_parent);

public:
	t_variant_parser(DBusMessageIter& a_parent)
	{
		f_initialize(a_parent);
	}
	t_variant_parser(t_variant_parser& a_parent)
	{
		f_initialize(a_parent.v_i);
	}
	operator DBusMessageIter&()
	{
		return v_i;
	}
	operator DBusMessageIter*()
	{
		return &v_i;
	}
};

class t_variant_builder
{
	dbus::t_container_builder v_i;
	dbus::t_container_builder v_j;

public:
	t_variant_builder(DBusMessageIter& a_parent, const char* a_signature, const char* a_name) : v_i(a_parent, DBUS_TYPE_VARIANT, a_signature), v_j(v_i, DBUS_TYPE_STRUCT, NULL)
	{
		v_j << a_name;
		dbus::t_container_builder(v_j, DBUS_TYPE_ARRAY, "{sv}");
	}
	operator DBusMessageIter&()
	{
		return v_j;
	}
};

}

}

#endif
