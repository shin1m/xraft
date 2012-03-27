#include <xraft/ibus.h>

#include <cstdio>
#include <cstdlib>
#include <vector>

namespace xraft
{

namespace ibus
{

std::string f_address()
{
	std::string hostname = "unix";
	std::string number = "0";
	std::string display = std::getenv("DISPLAY");
	size_t i = display.find(":");
	if (i > 0) hostname = display.substr(0, i);
	if (i != std::string::npos) {
		size_t j = display.find(".", ++i);
		if (j > i) number = display.substr(i, j - i);
	}
	char* id = dbus_get_local_machine_id();
	std::string path = std::string(std::getenv("HOME")) + "/.config/ibus/bus/" + id + "-" + hostname + "-" + number;
	dbus_free(id);
//std::fprintf(stderr, "CONFIG: %s\n", path.c_str());
	std::string address;
	std::FILE* fp = std::fopen(path.c_str(), "r");
	if (fp != NULL) {
		int c = std::fgetc(fp);
		while (c != EOF) {
			while (std::isspace(c)) c = std::fgetc(fp);
			if (c == '#') {
				do c = std::fgetc(fp); while (c != EOF && c != '\n');
				if (c == '\n') c = std::fgetc(fp);
				continue;
			}
			std::vector<char> cs;
			while (c != EOF && c != '\n' && c != '=') {
				cs.push_back(c);
				c = std::fgetc(fp);
			}
			if (c == EOF) break;
			if (c == '\n') {
				c = std::fgetc(fp);
				continue;
			}
			c = std::fgetc(fp);
			std::string s(cs.begin(), cs.end());
			cs.clear();
			while (c != EOF && c != '\n') {
				cs.push_back(c);
				c = std::fgetc(fp);
			}
			if (c == '\n') c = std::fgetc(fp);
			if (s == "IBUS_ADDRESS") address.assign(cs.begin(), cs.end());
		}
		std::fclose(fp);
	}
//std::fprintf(stderr, "ADDRESS: %s\n", address.c_str());
	return address;
}

void t_variant_parser::f_initialize(DBusMessageIter& a_parent)
{
	if (dbus_message_iter_get_arg_type(&a_parent) != DBUS_TYPE_VARIANT) throw std::runtime_error("dbus_message_iter_get_arg_type must be DBUS_TYPE_VARIANT.");
	DBusMessageIter i;
	dbus_message_iter_recurse(&a_parent, &i);
	if (dbus_message_iter_get_arg_type(&i) != DBUS_TYPE_STRUCT) throw std::runtime_error("dbus_message_iter_get_arg_type must be DBUS_TYPE_STRUCT.");
	dbus_message_iter_recurse(&i, &v_i);
	const char* type;
	v_i >> type;
//std::fprintf(stderr, "Type: %s\n", type);
	if (dbus_message_iter_get_arg_type(&v_i) != DBUS_TYPE_ARRAY) throw std::runtime_error("dbus_message_iter_get_arg_type must be DBUS_TYPE_ARRAY.");
	dbus_message_iter_next(&v_i);
}

}

}
