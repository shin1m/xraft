#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <dirent.h>
#include <xraft/converter.h>
#include <xraft/ibus.h>

#include "basic_dictionary.h"
#include "lock.h"
#include "engine.h"

namespace
{

struct t_ibus_dictionary : t_basic_dictionary
{
	size_t v_n;
	mutable t_mutex v_mutex;

	t_ibus_dictionary(const std::vector<std::string>& a_publics, const std::string& a_private) : t_basic_dictionary(a_publics, a_private), v_n(0)
	{
	}
	virtual void f_load();
	virtual void f_save() const;
	virtual void f_search(const wchar_t* a_entry, size_t a_n, size_t a_okuri, std::deque<t_candidate>& a_candidates) const;
	virtual void f_register(const wchar_t* a_entry, size_t a_n, size_t a_okuri, const wchar_t* a_text, size_t a_m);
};

void t_ibus_dictionary::f_load()
{
	t_lock<t_mutex> lock(v_mutex);
	t_basic_dictionary::f_load();
}

void t_ibus_dictionary::f_save() const
{
	t_lock<t_mutex> lock(v_mutex);
	t_basic_dictionary::f_save();
}

void t_ibus_dictionary::f_search(const wchar_t* a_entry, size_t a_n, size_t a_okuri, std::deque<t_candidate>& a_candidates) const
{
	t_lock<t_mutex> lock(v_mutex);
	t_basic_dictionary::f_search(a_entry, a_n, a_okuri, a_candidates);
}

void t_ibus_dictionary::f_register(const wchar_t* a_entry, size_t a_n, size_t a_okuri, const wchar_t* a_text, size_t a_m)
{
	t_lock<t_mutex> lock(v_mutex);
	t_basic_dictionary::f_register(a_entry, a_n, a_okuri, a_text, a_m);
}

t_ibus_dictionary* v_dictionary;
dbus::t_connection v_bus;
size_t v_engines;

class t_signal_emitter
{
	dbus::t_message v_message;
	DBusMessageIter v_i;

public:
	t_signal_emitter(const char* a_path, const char* a_name) : v_message(dbus_message_new_signal(a_path, "org.freedesktop.IBus.Engine", a_name))
	{
		if (!v_message) throw std::runtime_error("dbus_message_new_signal failed.");
		dbus_message_iter_init_append(v_message, &v_i);
	}
	~t_signal_emitter() noexcept(false)
	{
		if (v_message && dbus_connection_send(v_bus, v_message, NULL) == FALSE) throw std::runtime_error("dbus_connection_send failed.");
	}
	operator DBusMessageIter&()
	{
		return v_i;
	}
};

class t_ibus_engine : public t_engine
{
	static void f_unregister(DBusConnection* a_connection, void* a_data);
	static DBusHandlerResult f_message(DBusConnection* a_connection, DBusMessage* a_message, void* a_data);

	t_converter<wchar_t, char> v_converter{"wchar_t", "utf-8"};
	std::string v_path;
	bool v_choosing;
	std::vector<wchar_t> v_cs;
	std::vector<t_attribute> v_as;
	bool v_forwarded;

	t_ibus_engine(t_dictionary& a_dictionary) : t_engine(a_dictionary), v_choosing(false)
	{
		char path[46];
		std::sprintf(path, "/org/freedesktop/IBus/Engine/%zx", this);
		v_path = path;
	}
	~t_ibus_engine()
	{
		if (--v_engines > 0) return;
		::v_dictionary->f_save();
		delete ::v_dictionary;
		::v_dictionary = 0;
	}
	void f_text(DBusMessageIter& a_i, const wchar_t* a_cs, size_t a_n);
	void f_text(DBusMessageIter& a_i, const wchar_t* a_cs, const t_attribute* a_as, size_t a_n);
	void f_emit(const char* a_name)
	{
		v_bus.f_emit(v_path.c_str(), "org.freedesktop.IBus.Engine", a_name, DBUS_TYPE_INVALID);
	}
	void f_commit_text(const wchar_t* a_cs, size_t a_n);
	void f_update_preedit_text(const wchar_t* a_cs, const t_attribute* a_as, size_t a_n, dbus_uint32_t a_cursor, bool a_visible, dbus_uint32_t a_mode);
	void f_update_auxiliary_text(const wchar_t* a_cs, size_t a_n, bool a_visible);
	bool f_process(dbus_uint32_t a_value, dbus_uint32_t a_code, dbus_uint32_t a_state)
	{
		if ((a_state & ibus::e_modifier__RELEASE) != 0) return false;
		a_code += 8;
		char ascii = a_value < 128 ? a_value : 0;
		if (a_value >= 'a' && a_value <= 'z') a_value -= 'a' - 'A';
		if ((a_state & e_modifier__CONTROL) != 0 && a_value >= '@') ascii = a_value - '@';
		v_forwarded = false;
		f_key_pressed(static_cast<t_modifier>(a_state), static_cast<t_key>(a_value), ascii);
		return !v_forwarded;
	}
	void f_focus_in();
	void f_hide();

protected:
	virtual void f_on_forward();
	virtual void f_on_compose(size_t a_i, size_t a_m, const wchar_t* a_cs, const t_attribute* a_as, size_t a_n);
	virtual void f_on_commit(const wchar_t* a_cs, size_t a_n);
	virtual void f_on_status();
	virtual void f_on_candidates();
	virtual void f_on_choose();

public:
	static t_ibus_engine* f_create();

	const std::string& f_path() const
	{
		return v_path;
	}
};

void t_ibus_engine::f_text(DBusMessageIter& a_i, const wchar_t* a_cs, size_t a_n)
{
	std::vector<char> cs;
	v_converter(a_cs, a_cs + a_n, std::back_inserter(cs));
	cs.push_back('\0');
	ibus::t_variant_builder b0(a_i, "(sa{sv}sv)", "IBusText");
	b0 << &cs[0];
	ibus::t_variant_builder b1(b0, "(sa{sv}av)", "IBusAttrList");
	dbus::t_container_builder(b1, DBUS_TYPE_ARRAY, "v");
}

void t_ibus_engine::f_text(DBusMessageIter& a_i, const wchar_t* a_cs, const t_attribute* a_as, size_t a_n)
{
	std::vector<char> cs;
	v_converter(a_cs, a_cs + a_n, std::back_inserter(cs));
	cs.push_back('\0');
	ibus::t_variant_builder b0(a_i, "(sa{sv}sv)", "IBusText");
	b0 << &cs[0];
	ibus::t_variant_builder b1(b0, "(sa{sv}av)", "IBusAttrList");
	dbus::t_container_builder b2(b1, DBUS_TYPE_ARRAY, "v");
	{
		ibus::t_variant_builder b3(b2, "(sa{sv}uuuu)", "IBusAttribute");
		b3 << dbus_uint32_t(ibus::e_attribute_type__UNDERLINE) << dbus_uint32_t(ibus::e_attribute_underline__SINGLE) << dbus_uint32_t(0) << dbus_uint32_t(a_n);
	}
	size_t i = 0;
	while (i < a_n) {
		t_attribute a = *a_as;
		size_t j = i;
		do ++i; while (i < a_n && *++a_as == a);
		dbus_uint32_t type;
		switch (a) {
		case e_attribute__ROMAN:
			type = ibus::e_attribute_type__FOREGROUND;
			break;
		case e_attribute__ENTRY:
			type = ibus::e_attribute_type__BACKGROUND;
			break;
		default:
			continue;
		}
		ibus::t_variant_builder b3(b2, "(sa{sv}uuuu)", "IBusAttribute");
		b3 << type << dbus_uint32_t(0x007f7f7f) << dbus_uint32_t(j) << dbus_uint32_t(i);
	}
}

void t_ibus_engine::f_commit_text(const wchar_t* a_cs, size_t a_n)
{
	t_signal_emitter e(v_path.c_str(), "CommitText");
	f_text(e, a_cs, a_n);
}

void t_ibus_engine::f_update_preedit_text(const wchar_t* a_cs, const t_attribute* a_as, size_t a_n, dbus_uint32_t a_cursor, bool a_visible, dbus_uint32_t a_mode)
{
	t_signal_emitter e(v_path.c_str(), "UpdatePreeditText");
	f_text(e, a_cs, a_as, a_n);
	e << a_cursor << a_visible << a_mode;
}

void t_ibus_engine::f_update_auxiliary_text(const wchar_t* a_cs, size_t a_n, bool a_visible)
{
	t_signal_emitter e(v_path.c_str(), "UpdateAuxiliaryText");
	f_text(e, a_cs, a_n);
	e << a_visible;
}

void t_ibus_engine::f_on_forward()
{
	v_forwarded = true;
}

void t_ibus_engine::f_on_compose(size_t a_i, size_t a_m, const wchar_t* a_cs, const t_attribute* a_as, size_t a_n)
{
	{
		auto i = v_cs.begin() + a_i;
		v_cs.insert(v_cs.erase(i, i + a_m), a_cs, a_cs + a_n);
	}
	{
		auto i = v_as.begin() + a_i;
		v_as.insert(v_as.erase(i, i + a_m), a_as, a_as + a_n);
	}
	f_update_preedit_text(&v_cs[0], &v_as[0], v_cs.size(), f_caret(), !v_cs.empty(), ibus::e_engine_preedit__CLEAR);
}

void t_ibus_engine::f_on_commit(const wchar_t* a_cs, size_t a_n)
{
	if (!v_cs.empty()) {
		v_cs.clear();
		v_as.clear();
		f_update_preedit_text(0, 0, 0, 0, false, ibus::e_engine_preedit__CLEAR);
	}
	if (a_n > 0) f_commit_text(a_cs, a_n);
}

void t_ibus_engine::f_on_status()
{
	if (v_choosing) return;
	std::wstring s = f_status();
	std::vector<wchar_t> cs(s.begin(), s.end());
	size_t n = f_states().size() - 1;
	while (n > 0) {
		cs.push_back(L'\n');
		cs.push_back(L'?');
		s = f_entry(--n);
		cs.insert(cs.end(), s.begin(), s.end());
	}
	f_update_auxiliary_text(&cs[0], cs.size(), !cs.empty());
}

void t_ibus_engine::f_on_candidates()
{
	if (f_candidates().empty()) {
		v_choosing = false;
		f_on_status();
		t_signal_emitter e(v_path.c_str(), "UpdateLookupTable");
		{
			ibus::t_variant_builder b0(e, "(sa{sv}uubbiavav)", "IBusLookupTable");
			b0 << dbus_uint32_t(0) << dbus_uint32_t(0) << false << false << dbus_int32_t(ibus::e_orientation__SYSTEM);
			dbus::t_container_builder(b0, DBUS_TYPE_ARRAY, "v");
			dbus::t_container_builder(b0, DBUS_TYPE_ARRAY, "v");
		}
		e << false;
	} else {
		v_choosing = true;
		f_on_choose();
	}
}

void t_ibus_engine::f_on_choose()
{
	{
		wchar_t cs[64];
		std::swprintf(cs, 64, L" %zd/%zd", f_chosen() + 1, f_candidates().size());
		std::wstring s = f_status() + cs;
		f_update_auxiliary_text(s.c_str(), s.size(), true);
	}
	size_t first = f_chosen() / 8 * 8;
	size_t last = std::min(first + 8, f_candidates().size());
	t_signal_emitter e(v_path.c_str(), "UpdateLookupTable");
	{
		ibus::t_variant_builder b0(e, "(sa{sv}uubbiavav)", "IBusLookupTable");
		b0 << dbus_uint32_t(8) << dbus_uint32_t(f_chosen() - first) << true << false << dbus_int32_t(ibus::e_orientation__SYSTEM);
		{
			dbus::t_container_builder b1(b0, DBUS_TYPE_ARRAY, "v");
			for (size_t i = first; i < last; ++i) {
				const t_candidate& candidate = f_candidates()[i];
				const std::wstring& text = candidate.v_text;
				std::vector<wchar_t> cs(text.begin(), text.end());
				const std::vector<std::wstring>& annotations = candidate.v_annotations;
				for (const auto& x : annotations) {
					cs.push_back(L' ');
					cs.push_back(L';');
					cs.insert(cs.end(), x.begin(), x.end());
				}
				f_text(b1, &cs[0], cs.size());
			}
		}
		dbus::t_container_builder(b0, DBUS_TYPE_ARRAY, "v");
	}
	e << true;
}

void t_ibus_engine::f_focus_in()
{
	if (!v_cs.empty()) f_update_preedit_text(&v_cs[0], &v_as[0], v_cs.size(), f_caret(), true, ibus::e_engine_preedit__CLEAR);
	if (v_choosing)
		f_on_choose();
	else
		f_on_status();
}

void t_ibus_engine::f_hide()
{
	f_update_auxiliary_text(0, 0, false);
}

void t_ibus_engine::f_unregister(DBusConnection* a_connection, void* a_data)
{
	delete static_cast<t_ibus_engine*>(a_data);
}

DBusHandlerResult t_ibus_engine::f_message(DBusConnection* a_connection, DBusMessage* a_message, void* a_data)
{
//std::fprintf(stderr, "message: %s, %s, %s\n", dbus_message_get_path(a_message), dbus_message_get_interface(a_message), dbus_message_get_member(a_message));
	if (dbus_message_has_interface(a_message, "org.freedesktop.IBus.Engine") != FALSE) {
		t_ibus_engine* engine = static_cast<t_ibus_engine*>(a_data);
		if (dbus_message_has_member(a_message, "ProcessKeyEvent") != FALSE) {
			dbus::t_message method(a_message, dbus::t_own());
			dbus_uint32_t value;
			dbus_uint32_t code;
			dbus_uint32_t state;
			method.f_get(DBUS_TYPE_UINT32, &value, DBUS_TYPE_UINT32, &code, DBUS_TYPE_UINT32, &state, DBUS_TYPE_INVALID);
			dbus_bool_t handled = engine->f_process(value, code, state) ? TRUE : FALSE;
			v_bus.f_reply(a_message, DBUS_TYPE_BOOLEAN, &handled, DBUS_TYPE_INVALID);
			return DBUS_HANDLER_RESULT_HANDLED;
		} else if (dbus_message_has_member(a_message, "FocusIn") != FALSE) {
			engine->f_focus_in();
			return DBUS_HANDLER_RESULT_HANDLED;
		} else if (dbus_message_has_member(a_message, "Reset") != FALSE) {
			engine->f_reset();
			return DBUS_HANDLER_RESULT_HANDLED;
		} else if (dbus_message_has_member(a_message, "SetCursorLocation") != FALSE) {
			dbus::t_message method(a_message, dbus::t_own());
			dbus_int32_t x;
			dbus_int32_t y;
			dbus_int32_t w;
			dbus_int32_t h;
			method.f_get(DBUS_TYPE_INT32, &x, DBUS_TYPE_INT32, &y, DBUS_TYPE_INT32, &w, DBUS_TYPE_INT32, &h, DBUS_TYPE_INVALID);
//std::fprintf(stderr, "SetCursorLocation: %d, %d, %d, %d\n", x, y, w, h);
			if (h <= 0) engine->f_hide();
			return DBUS_HANDLER_RESULT_HANDLED;
		}
	}
	if (dbus_message_is_method_call(a_message, "org.freedesktop.IBus.Service", "Destroy") != FALSE) {
		if (dbus_connection_unregister_object_path(v_bus, dbus_message_get_path(a_message)) == FALSE) throw std::runtime_error("dbus_connection_unregister_object_path failed.");
		return DBUS_HANDLER_RESULT_HANDLED;
	}
	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

t_ibus_engine* t_ibus_engine::f_create()
{
	if (v_engines <= 0) {
		std::string home = std::string(std::getenv("HOME")) + "/.xraftim";
		std::string path = home + "/public";
		std::vector<std::string> publics;
		DIR* dp = opendir(path.c_str());
		if (dp != NULL) {
			while (true) {
				struct dirent* d = readdir(dp);
				if (d == NULL) break;
				publics.push_back(path + '/' + d->d_name);
			}
			closedir(dp);
		}
		std::sort(publics.begin(), publics.end());
		::v_dictionary = new t_ibus_dictionary(publics, home + "/private");
		::v_dictionary->f_load();
	}
	++v_engines;
	t_ibus_engine* engine = new t_ibus_engine(*::v_dictionary);
	DBusObjectPathVTable table = {f_unregister, f_message};
	if (dbus_connection_register_object_path(v_bus, engine->f_path().c_str(), &table, engine) == FALSE) {
		delete engine;
		throw std::runtime_error("dbus_connection_register_object_path failed.");
	}
	return engine;
}

DBusHandlerResult f_factory(DBusConnection* a_connection, DBusMessage* a_message, void* a_data)
{
//std::fprintf(stderr, "factory: %s, %s, %s\n", dbus_message_get_path(a_message), dbus_message_get_interface(a_message), dbus_message_get_member(a_message));
	if (dbus_message_is_method_call(a_message, "org.freedesktop.IBus.Factory", "CreateEngine") != FALSE) {
		t_ibus_engine* engine = t_ibus_engine::f_create();
		const char* path = engine->f_path().c_str();
		v_bus.f_reply(a_message, DBUS_TYPE_OBJECT_PATH, &path, DBUS_TYPE_INVALID);
		return DBUS_HANDLER_RESULT_HANDLED;
	}
	return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

}

int main(int argc, char* argv[])
{
	bool ibus = argc > 1 && std::strcmp(argv[1], "--ibus") == 0;
	v_bus.f_open(ibus::f_address().c_str());
	{
		DBusObjectPathVTable table = {NULL, f_factory};
		if (dbus_connection_register_object_path(v_bus, "/org/freedesktop/IBus/Factory", &table, NULL) == FALSE) throw std::runtime_error("dbus_connection_register_object_path failed.");
	}
	if (ibus) {
		if (dbus_bus_register(v_bus, NULL) != TRUE) throw std::runtime_error("dbus_bus_register failed.");
		if (dbus_bus_request_name(v_bus, "xraft.xraftim", DBUS_NAME_FLAG_REPLACE_EXISTING | DBUS_NAME_FLAG_ALLOW_REPLACEMENT, NULL) == -1) throw std::runtime_error("dbus_bus_request_name failed.");
	} else {
		dbus::t_message message = dbus_message_new_method_call("org.freedesktop.IBus", "/org/freedesktop/IBus", "org.freedesktop.IBus", "RegisterComponent");
		if (!message) throw std::runtime_error("dbus_message_new_method_call failed.");
		DBusMessageIter i;
		dbus_message_iter_init_append(message, &i);
		{
			ibus::t_variant_builder b0(i, "(sa{sv}ssssssssavav)", "IBusComponent");
			b0 << "xraft.xraftim";
			b0 << "XRAFT Input Method";
			b0 << "0.0.0";
			b0 << "MIT";
			b0 << "Shin-ichi MORITA <shin1morita@gmail.com>";
			b0 << "http://github.com/shin1m/xraft";
			b0 << "";
			b0 << "xraftim";
			dbus::t_container_builder(b0, DBUS_TYPE_ARRAY, "v");
			dbus::t_container_builder b1(b0, DBUS_TYPE_ARRAY, "v");
			ibus::t_variant_builder b2(b1, "(sa{sv}ssssssssussssss)", "IBusEngineDesc");
			b2 << "xraftim";
			b2 << "xraftim";
			b2 << "XRAFT Input Method";
			b2 << "ja";
			b2 << "MIT";
			b2 << "Shin-ichi MORITA <shin1morita@gmail.com>";
			b2 << "";
			b2 << "jp";
			b2 << dbus_uint32_t(0);
			b2 << "";
			b2 << "";
			b2 << "";
			b2 << "";
			b2 << "";
			b2 << "";
		}
		if (dbus_connection_send(v_bus, message, NULL) == FALSE) throw std::runtime_error("dbus_connection_send failed.");
		const char* name = "xraftim";
		v_bus.f_send("org.freedesktop.IBus", "/org/freedesktop/IBus", "org.freedesktop.IBus", "SetGlobalEngine", DBUS_TYPE_STRING, &name, DBUS_TYPE_INVALID);
	}
	while (dbus_connection_read_write_dispatch(v_bus, -1) != FALSE);
	return 0;
}
