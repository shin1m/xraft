#include "application.h"
#include "graphics.h"
#include "opengl.h"
#include "pixmap.h"
#include "region.h"
#include "timer.h"

namespace xemmaix::xraft
{

t_entry::t_entry() : v_previous(static_cast<t_application*>(f_application())), v_next(v_previous->v_next)
{
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
	v_object->f_as<::xraft::t_object*>() = nullptr;
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

void t_library::f_main(t_library* a_library, const t_pvalue& a_arguments, const t_pvalue& a_callable)
{
	t_application application(a_library, a_arguments);
	auto object = f_engine()->f_allocate(sizeof(::xraft::t_application*));
	object->f_as<::xraft::t_application*>() = &application;
	object->f_be(a_library->f_type<::xraft::t_application>());
	a_library->v_application = object;
	try {
		a_callable(object);
		a_library->v_application = nullptr;
		std::lock_guard lock(a_library->v_mutex);
		object->f_as<::xraft::t_application*>() = nullptr;
	} catch (...) {
		a_library->v_application = nullptr;
		std::lock_guard lock(a_library->v_mutex);
		object->f_as<::xraft::t_application*>() = nullptr;
		throw;
	}
}

void t_library::f_scan(t_scan a_scan)
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

std::vector<std::pair<t_root, t_rvalue>> t_library::f_define()
{
	v_symbol_on_move = t_symbol::f_instantiate(L"on_move"sv);
	v_symbol_on_show = t_symbol::f_instantiate(L"on_show"sv);
	v_symbol_on_hide = t_symbol::f_instantiate(L"on_hide"sv);
	v_symbol_on_paint = t_symbol::f_instantiate(L"on_paint"sv);
	v_symbol_on_focus_enter = t_symbol::f_instantiate(L"on_focus_enter"sv);
	v_symbol_on_focus_leave = t_symbol::f_instantiate(L"on_focus_leave"sv);
	v_symbol_on_key_press = t_symbol::f_instantiate(L"on_key_press"sv);
	v_symbol_on_key_release = t_symbol::f_instantiate(L"on_key_release"sv);
	v_symbol_on_input_compose = t_symbol::f_instantiate(L"on_input_compose"sv);
	v_symbol_on_input_commit = t_symbol::f_instantiate(L"on_input_commit"sv);
	v_symbol_on_input_spot = t_symbol::f_instantiate(L"on_input_spot"sv);
	v_symbol_on_button_press = t_symbol::f_instantiate(L"on_button_press"sv);
	v_symbol_on_button_release = t_symbol::f_instantiate(L"on_button_release"sv);
	v_symbol_on_pointer_enter = t_symbol::f_instantiate(L"on_pointer_enter"sv);
	v_symbol_on_pointer_leave = t_symbol::f_instantiate(L"on_pointer_leave"sv);
	v_symbol_on_pointer_move = t_symbol::f_instantiate(L"on_pointer_move"sv);
	v_symbol_add = t_symbol::f_instantiate(L"add"sv);
	v_symbol_remove = t_symbol::f_instantiate(L"remove"sv);
	v_symbol_on_activate = t_symbol::f_instantiate(L"on_activate"sv);
	v_symbol_on_deactivate = t_symbol::f_instantiate(L"on_deactivate"sv);
	v_symbol_on_close = t_symbol::f_instantiate(L"on_close"sv);
	v_symbol_on_create = t_symbol::f_instantiate(L"on_create"sv);
	v_symbol_on_destroy = t_symbol::f_instantiate(L"on_destroy"sv);
	t_type_of<::xraft::t_application>::f_define(this);
	t_type_of<t_point>::f_define(this);
	t_type_of<t_extent>::f_define(this);
	t_type_of<t_rectangle>::f_define(this);
	t_type_of<::xraft::t_object>::f_define(this);
	t_type_of<t_font>::f_define(this);
	t_type_of<::xraft::t_color>::f_define(this);
	t_type_of<t_graphics>::f_define(this);
	t_type_of<t_input_context>::f_define(this);
	t_type_of<t_drawable>::f_define(this);
	t_type_of<t_bitmap>::f_define(this);
	t_type_of<t_pixmap>::f_define(this);
	t_type_of<t_region>::f_define(this);
	t_type_of<t_timer>::f_define(this);
	t_type_of<t_window>::f_define(this);
	t_type_of<::xraft::t_widget>::f_define(this);
	t_type_of<::xraft::t_shell>::f_define(this);
	t_type_of<::xraft::t_frame>::f_define(this);
	t_type_of<t_opengl_format>::f_define(this);
	t_type_of<::xraft::t_opengl_widget>::f_define(this);
	t_type_of<t_opengl_context>::f_define(this);
	return t_define(this)
		(L"Application"sv, static_cast<xemmai::t_object*>(v_type_application))
		(L"Point"sv, static_cast<xemmai::t_object*>(v_type_point))
		(L"Extent"sv, static_cast<xemmai::t_object*>(v_type_extent))
		(L"Rectangle"sv, static_cast<xemmai::t_object*>(v_type_rectangle))
		(L"Object"sv, static_cast<xemmai::t_object*>(v_type_object))
		(L"Font"sv, static_cast<xemmai::t_object*>(v_type_font))
		(L"Color"sv, static_cast<xemmai::t_object*>(v_type_color))
		(L"Graphics"sv, static_cast<xemmai::t_object*>(v_type_graphics))
		(L"Function"sv, t_type_of<t_graphics::t_function>::f_define(this))
		(L"InputAttribute"sv, t_type_of<t_input_attribute>::f_define(this))
		(L"InputContext"sv, static_cast<xemmai::t_object*>(v_type_input_context))
		(L"Drawable"sv, static_cast<xemmai::t_object*>(v_type_drawable))
		(L"Bitmap"sv, static_cast<xemmai::t_object*>(v_type_bitmap))
		(L"Pixmap"sv, static_cast<xemmai::t_object*>(v_type_pixmap))
		(L"Region"sv, static_cast<xemmai::t_object*>(v_type_region))
		(L"Key"sv, t_type_of<t_key>::f_define(this))
		(L"Modifier"sv, t_type_of<t_modifier>::f_define(this))
		(L"Button"sv, t_type_of<t_button>::f_define(this))
		(L"Timer"sv, static_cast<xemmai::t_object*>(v_type_timer))
		(L"CrossMode"sv, t_type_of<t_cross_mode>::f_define(this))
		(L"CrossDetail"sv, t_type_of<t_cross_detail>::f_define(this))
		(L"Window"sv, static_cast<xemmai::t_object*>(v_type_window))
		(L"Widget"sv, static_cast<xemmai::t_object*>(v_type_widget))
		(L"Shell"sv, static_cast<xemmai::t_object*>(v_type_shell))
		(L"Frame"sv, static_cast<xemmai::t_object*>(v_type_frame))
		(L"GLFormat"sv, static_cast<xemmai::t_object*>(v_type_opengl_format))
		(L"GLWidget"sv, static_cast<xemmai::t_object*>(v_type_opengl_widget))
		(L"GLContext"sv, static_cast<xemmai::t_object*>(v_type_opengl_context))
		(L"main"sv, t_static<void (*)(t_library*, const t_pvalue&, const t_pvalue&), f_main>())
		(L"application"sv, t_static<t_pvalue(*)(t_library*), f_application>())
	;
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	return xemmai::f_new<xemmaix::xraft::t_library>(a_handle);
}
