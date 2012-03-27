#ifndef XRAFT__INPUT_CONTEXT_X11_H
#define XRAFT__INPUT_CONTEXT_X11_H

#include <vector>
#include <X11/Xlib.h>

#include "drawable.h"
#include "geometry.h"
#include "dbus.h"

namespace xraft
{

class t_application;
class t_window;
class t_shell;

class t_input_context : public t_object
{
	friend class t_application;
	friend class t_window;
	friend class t_shell;

	struct t_component : t_drawable
	{
		using t_drawable::v_handle;
		virtual ~t_component();
		void f_show(int a_x, int a_y, unsigned a_width, unsigned a_height);
		void f_hide();
	};
	struct t_line
	{
		size_t v_i;
		size_t v_j;
		int v_x;

		t_line(size_t a_i, int a_x) : v_i(a_i), v_x(a_x)
		{
		}
	};

	static void f_text(DBusMessageIter& a_i, std::vector<wchar_t>& a_cs, std::vector<t_input_attribute>& a_as);
	static void f_negotiate();

	std::string v_context;
	bool v_enabled;
	std::vector<wchar_t> v_cs;
	std::vector<t_input_attribute> v_as;

	virtual ~t_input_context();
	void f_create();
	void f_on_disconnected();
	void f_on_commit_text(dbus::t_message& a_message);
	void f_on_enabled(dbus::t_message& a_message);
	void f_on_disabled(dbus::t_message& a_message);
	void f_on_update_preedit_text(dbus::t_message& a_message);
	dbus::t_reply f_send(const char* a_method, int a_type, ...) const;
	void f_focus();
	void f_blur();
	void f_process(XKeyEvent& a_xkey);
	void f_draw(const t_pointer<t_component>& a_component, const wchar_t* a_cs, const t_input_attribute* a_as, const t_line* a_lines, size_t a_n, unsigned a_width);
	void f_preedit();

public:
	t_input_context() : v_enabled(false)
	{
	}
	bool f_open() const
	{
		return v_enabled;
	}
	void f_open__(bool a_open);
	void f_composition(std::vector<wchar_t>& a_cs, std::vector<t_input_attribute>& a_as)
	{
		a_cs = v_cs;
		a_as = v_as;
	}
};

}

#endif
