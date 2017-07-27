#include <clocale>
#include <cstdio>
#include <xraft/application.h>
#include <xraft/graphics.h>

using namespace xraft;

class t_dbus : public t_frame
{
	t_color v_background;
	dbus::t_connection v_connection;
	dbus::t_reply v_reply;

	virtual void f_on_paint(t_graphics& a_g)
	{
		t_extent extent = f_geometry();
		a_g.f_color(v_background.f_pixel());
		a_g.f_fill(0, 0, extent.v_width, extent.v_height);
	}
	virtual void f_on_key_press(t_modifier a_modifier, t_key a_key, char a_ascii)
	{
		if (a_key == e_key__SPACE) {
			dbus::t_message message = v_connection.f_send("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus.Introspectable", "Introspect", DBUS_TYPE_INVALID)();
			const char* p;
			message.f_get(DBUS_TYPE_STRING, &p, DBUS_TYPE_INVALID);
			std::printf("Introspect:\n%s", p);
		} else if (a_key == e_key__S) {
			v_reply = v_connection.f_send("org.freedesktop.DBus", "/org/freedesktop/DBus", "org.freedesktop.DBus.Introspectable", "Introspect", DBUS_TYPE_INVALID);
			std::printf("Sent: Introspect\n");
		} else if (a_key == e_key__R) {
			dbus::t_message message = v_reply();
			const char* p;
			message.f_get(DBUS_TYPE_STRING, &p, DBUS_TYPE_INVALID);
			std::printf("Received:\n%s", p);
		} else if (a_key == e_key__Q) {
			f_on_close();
		}
		t_extent extent = f_geometry();
		f_invalidate(0, 0, extent.v_width, extent.v_height);
	}
	virtual void f_on_close()
	{
		f_application()->f_exit();
	}
	void f_signal(dbus::t_message& a_message)
	{
		const char* p;
		a_message.f_get(DBUS_TYPE_STRING, &p, DBUS_TYPE_INVALID);
		std::printf("Hello: %s\n", p);
	}

public:
	t_dbus() : v_background("white")
	{
		v_connection.f_connect(DBUS_BUS_SESSION);
		new dbus::t_bridge(v_connection);
		v_connection.f_add_match(this, dbus::f_slot_member<t_dbus, &t_dbus::f_signal>, DBUS_MESSAGE_TYPE_SIGNAL, "/xraft/Foo", "xraft.Foo", "Hello");
	}
};

int main(int argc, char* argv[])
{
	std::setlocale(LC_ALL, "");
	std::vector<std::string> arguments(argv, argv + argc);
	t_application application(arguments);
	t_pointer<t_dbus> frame = new t_dbus();
	frame->f_caption__(L"D-Bus");
	frame->f_move(t_rectangle(0, 0, 320, 240));
	application.f_add(frame);
	frame->f_show();
	application.f_run();
	return 0;
}
