#include "application.h"
#include "graphics.h"
#include "opengl.h"
#include "pixmap.h"
#include "region.h"
#include "timer.h"

namespace xemmaix
{

namespace xraft
{

t_entry::t_entry()
{
	v_previous = static_cast<t_application*>(f_application());
	v_next = v_previous->v_next;
	v_previous->v_next = v_next->v_previous = this;
}

void t_entry::f_dispose()
{
	f_unlink();
}

void t_proxy::f_destroy()
{
	if (v_previous) f_unlink();
	if (v_n > 0) f_as<::xraft::t_object*>(v_object)->f_release();
	v_object.f_pointer__(nullptr);
	delete this;
}

void t_proxy::f_dispose()
{
	t_entry::f_dispose();
	if (v_n > 0) {
		v_n = 0;
		f_as<::xraft::t_object*>(v_object)->f_release();
	}
}

void t_extension::f_main(t_extension* a_extension, const t_value& a_arguments, const t_value& a_callable)
{
	t_application application(a_extension, a_arguments);
	t_scoped object = xemmai::t_object::f_allocate(a_extension->f_type<::xraft::t_application>());
	object.f_pointer__(&application);
	a_extension->v_application = object;
	try {
		a_callable(object);
		a_extension->v_application = nullptr;
		t_with_lock_for_write lock(object);
		object.f_pointer__(nullptr);
	} catch (...) {
		a_extension->v_application = nullptr;
		t_with_lock_for_write lock(object);
		object.f_pointer__(nullptr);
		throw;
	}
}

t_extension::t_extension(xemmai::t_object* a_module) : xemmai::t_extension(a_module)
{
	v_symbol_on_move = t_symbol::f_instantiate(L"on_move");
	v_symbol_on_show = t_symbol::f_instantiate(L"on_show");
	v_symbol_on_hide = t_symbol::f_instantiate(L"on_hide");
	v_symbol_on_paint = t_symbol::f_instantiate(L"on_paint");
	v_symbol_on_focus_enter = t_symbol::f_instantiate(L"on_focus_enter");
	v_symbol_on_focus_leave = t_symbol::f_instantiate(L"on_focus_leave");
	v_symbol_on_key_press = t_symbol::f_instantiate(L"on_key_press");
	v_symbol_on_key_release = t_symbol::f_instantiate(L"on_key_release");
	v_symbol_on_input_compose = t_symbol::f_instantiate(L"on_input_compose");
	v_symbol_on_input_commit = t_symbol::f_instantiate(L"on_input_commit");
	v_symbol_on_input_spot = t_symbol::f_instantiate(L"on_input_spot");
	v_symbol_on_button_press = t_symbol::f_instantiate(L"on_button_press");
	v_symbol_on_button_release = t_symbol::f_instantiate(L"on_button_release");
	v_symbol_on_pointer_enter = t_symbol::f_instantiate(L"on_pointer_enter");
	v_symbol_on_pointer_leave = t_symbol::f_instantiate(L"on_pointer_leave");
	v_symbol_on_pointer_move = t_symbol::f_instantiate(L"on_pointer_move");
	v_symbol_add = t_symbol::f_instantiate(L"add");
	v_symbol_remove = t_symbol::f_instantiate(L"remove");
	v_symbol_on_activate = t_symbol::f_instantiate(L"on_activate");
	v_symbol_on_deactivate = t_symbol::f_instantiate(L"on_deactivate");
	v_symbol_on_close = t_symbol::f_instantiate(L"on_close");
	v_symbol_on_create = t_symbol::f_instantiate(L"on_create");
	v_symbol_on_destroy = t_symbol::f_instantiate(L"on_destroy");
	t_type_of<::xraft::t_application>::f_define(this);
	t_type_of<t_point>::f_define(this);
	t_type_of<t_extent>::f_define(this);
	t_type_of<t_rectangle>::f_define(this);
	t_type_of<::xraft::t_object>::f_define(this);
	t_type_of<t_font>::f_define(this);
	t_type_of<t_color>::f_define(this);
	t_type_of<t_graphics>::f_define(this);
	t_type_of<t_graphics::t_function>::f_define(this);
	t_type_of<t_input_attribute>::f_define(this);
	t_type_of<t_input_context>::f_define(this);
	t_type_of<t_drawable>::f_define(this);
	t_type_of<t_bitmap>::f_define(this);
	t_type_of<t_pixmap>::f_define(this);
	t_type_of<t_region>::f_define(this);
	t_type_of<t_key>::f_define(this);
	t_type_of<t_modifier>::f_define(this);
	t_type_of<t_button>::f_define(this);
	t_type_of<t_timer>::f_define(this);
	t_type_of<t_cross_mode>::f_define(this);
	t_type_of<t_cross_detail>::f_define(this);
	t_type_of<t_window>::f_define(this);
	t_type_of<::xraft::t_widget>::f_define(this);
	t_type_of<::xraft::t_shell>::f_define(this);
	t_type_of<::xraft::t_frame>::f_define(this);
	t_type_of<t_opengl_format>::f_define(this);
	t_type_of<::xraft::t_opengl_widget>::f_define(this);
	t_type_of<t_opengl_context>::f_define(this);
	f_define<void (*)(t_extension*, const t_value&, const t_value&), f_main>(this, L"main");
	f_define<t_scoped(*)(t_extension*), f_application>(this, L"application");
}

void t_extension::f_scan(t_scan a_scan)
{
	a_scan(v_symbol_on_move);
	a_scan(v_symbol_on_show);
	a_scan(v_symbol_on_hide);
	a_scan(v_symbol_on_paint);
	a_scan(v_symbol_on_focus_enter);
	a_scan(v_symbol_on_focus_leave);
	a_scan(v_symbol_on_key_press);
	a_scan(v_symbol_on_key_release);
	a_scan(v_symbol_on_input_compose);
	a_scan(v_symbol_on_input_commit);
	a_scan(v_symbol_on_input_spot);
	a_scan(v_symbol_on_button_press);
	a_scan(v_symbol_on_button_release);
	a_scan(v_symbol_on_pointer_enter);
	a_scan(v_symbol_on_pointer_leave);
	a_scan(v_symbol_on_pointer_move);
	a_scan(v_symbol_add);
	a_scan(v_symbol_remove);
	a_scan(v_symbol_on_activate);
	a_scan(v_symbol_on_deactivate);
	a_scan(v_symbol_on_close);
	a_scan(v_symbol_on_create);
	a_scan(v_symbol_on_destroy);
	a_scan(v_type_application);
	a_scan(v_type_point);
	a_scan(v_type_extent);
	a_scan(v_type_rectangle);
	a_scan(v_type_object);
	a_scan(v_type_font);
	a_scan(v_type_color);
	a_scan(v_type_graphics);
	a_scan(v_type_graphics__function);
	a_scan(v_type_input_attribute);
	a_scan(v_type_input_context);
	a_scan(v_type_drawable);
	a_scan(v_type_bitmap);
	a_scan(v_type_pixmap);
	a_scan(v_type_region);
	a_scan(v_type_key);
	a_scan(v_type_modifier);
	a_scan(v_type_button);
	a_scan(v_type_timer);
	a_scan(v_type_cross_mode);
	a_scan(v_type_cross_detail);
	a_scan(v_type_window);
	a_scan(v_type_widget);
	a_scan(v_type_shell);
	a_scan(v_type_frame);
	a_scan(v_type_opengl_format);
	a_scan(v_type_opengl_widget);
	a_scan(v_type_opengl_context);
}

}

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new xemmaix::xraft::t_extension(a_module);
}
