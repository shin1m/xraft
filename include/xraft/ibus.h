#ifndef XRAFT__IBUS_H
#define XRAFT__IBUS_H

#include "dbus.h"

namespace xraft
{

namespace ibus
{

enum
{
	e_attribute_type__UNDERLINE = 1,
	e_attribute_type__FOREGROUND = 2,
	e_attribute_type__BACKGROUND = 3
};

enum
{
	e_attribute_underline__NONE = 0,
	e_attribute_underline__SINGLE = 1,
	e_attribute_underline__DOUBLE = 2,
	e_attribute_underline__LOW = 3,
	e_attribute_underline__ERROR = 4
};

enum
{
	e_modifier__HANDLED = 1 << 24,
	e_modifier__FORWARD = 1 << 25,
	e_modifier__SUPER = 1 << 26,
	e_modifier__HYPER = 1 << 27,
	e_modifier__META = 1 << 28,
	e_modifier__RELEASE = 1 << 30
};

enum
{
	e_capability__PREEDIT_TEXT = 1 << 0,
	e_capability__AUXILIARY_TEXT = 1 << 1,
	e_capability__LOOKUP_TABLE = 1 << 2,
	e_capability__FOCUS = 1 << 3,
	e_capability__PROPERTY = 1 << 4,
	e_capability__SURROUNDING_TEXT = 1 << 5
};

enum
{
	e_engine_preedit__CLEAR = 0,
	e_engine_preedit__COMMIT = 1
};

enum
{
	e_orientation__HORIZONTAL = 0,
	e_orientation__VERTICAL = 1,
	e_orientation__SYSTEM = 2
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
