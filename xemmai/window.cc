#include "window.h"

namespace xraft
{

namespace xemmai
{

struct t_widget : xraft::t_widget, t_wrapper<t_widget>
{
#define T_WINDOW t_widget
#include "window_override.h"
#undef T_WINDOW
};

struct t_shell : xraft::t_shell, t_wrapper<t_shell>
{
#define T_WINDOW t_shell
#include "window_override.h"
#undef T_WINDOW
	static void f_super__on_activate(xraft::t_shell& a_self)
	{
		if (dynamic_cast<t_shell*>(&a_self))
			a_self.xraft::t_shell::f_on_activate();
		else
			a_self.f_on_activate();
	}
	virtual void f_on_activate()
	{
		t_extension* extension = f_extension<t_extension>(f_as<t_type*>(f_self(this)->f_type())->v_module);
		f_self(this)->f_get(extension->v_symbol_on_activate)();
	}
	static void f_super__on_deactivate(xraft::t_shell& a_self)
	{
		if (dynamic_cast<t_shell*>(&a_self))
			a_self.xraft::t_shell::f_on_deactivate();
		else
			a_self.f_on_deactivate();
	}
	virtual void f_on_deactivate()
	{
		t_extension* extension = f_extension<t_extension>(f_as<t_type*>(f_self(this)->f_type())->v_module);
		f_self(this)->f_get(extension->v_symbol_on_deactivate)();
	}
};

struct t_frame : xraft::t_frame, t_wrapper<t_frame>
{
#define T_WINDOW t_frame
#include "window_override.h"
#undef T_WINDOW
	static void f_super__on_activate(xraft::t_frame& a_self)
	{
		if (dynamic_cast<t_frame*>(&a_self))
			a_self.xraft::t_frame::f_on_activate();
		else
			a_self.f_on_activate();
	}
	virtual void f_on_activate()
	{
		t_extension* extension = f_extension<t_extension>(f_as<t_type*>(f_self(this)->f_type())->v_module);
		f_self(this)->f_get(extension->v_symbol_on_activate)();
	}
	static void f_super__on_deactivate(xraft::t_frame& a_self)
	{
		if (dynamic_cast<t_frame*>(&a_self))
			a_self.xraft::t_frame::f_on_deactivate();
		else
			a_self.f_on_deactivate();
	}
	virtual void f_on_deactivate()
	{
		t_extension* extension = f_extension<t_extension>(f_as<t_type*>(f_self(this)->f_type())->v_module);
		f_self(this)->f_get(extension->v_symbol_on_deactivate)();
	}
	static void f_super__on_close(xraft::t_frame& a_self)
	{
		if (dynamic_cast<t_frame*>(&a_self))
			a_self.xraft::t_frame::f_on_close();
		else
			a_self.f_on_close();
	}
	virtual void f_on_close()
	{
		t_extension* extension = f_extension<t_extension>(f_as<t_type*>(f_self(this)->f_type())->v_module);
		f_self(this)->f_get(extension->v_symbol_on_close)();
	}
};

}

}

namespace xemmai
{

using xraft::xemmai::t_with_application_thread;

void t_type_of<t_cross_mode>::f_define(t_extension* a_extension)
{
	t_define<t_cross_mode, ptrdiff_t>(a_extension, L"CrossMode")
		(L"NORMAL", e_cross_mode__NORMAL)
		(L"GRAB", e_cross_mode__GRAB)
		(L"UNGRAB", e_cross_mode__UNGRAB)
	;
}

void t_type_of<t_cross_detail>::f_define(t_extension* a_extension)
{
	t_define<t_cross_detail, ptrdiff_t>(a_extension, L"CrossDetail")
		(L"OUTER", e_cross_detail__OUTER)
		(L"INNER", e_cross_detail__INNER)
		(L"THROUGH", e_cross_detail__THROUGH)
	;
}

void t_type_of<t_window>::f_define(t_extension* a_extension)
{
	t_define<t_window, t_drawable>(a_extension, L"Window")
		(L"count", t_member<size_t (t_window::*)() const, &t_window::f_count, t_with_application_thread>())
		(L"at", t_member<const xraft::t_pointer<t_widget>& (t_window::*)(size_t) const, &t_window::f_at, t_with_application_thread>())
		(a_extension->v_symbol_add,
			t_member<void (*)(t_window&, const xraft::t_pointer<t_widget>&), f_add, t_with_application_thread>(),
			t_member<void (*)(t_window&, const xraft::t_pointer<t_widget>&, size_t), f_add, t_with_application_thread>()
		)
		(a_extension->v_symbol_remove, t_member<void (t_window::*)(size_t), &t_window::f_remove, t_with_application_thread>())
		(L"geometry", t_member<const t_rectangle& (t_window::*)() const, &t_window::f_geometry, t_with_application_thread>())
		(L"move", t_member<void (t_window::*)(const t_rectangle&), &t_window::f_move, t_with_application_thread>())
		(L"visible", t_member<bool (t_window::*)() const, &t_window::f_visible, t_with_application_thread>())
		(L"show", t_member<void (t_window::*)(), &t_window::f_show, t_with_application_thread>())
		(L"hide", t_member<void (t_window::*)(), &t_window::f_hide, t_with_application_thread>())
		(L"invalidate", t_member<void (t_window::*)(int, int, unsigned, unsigned), &t_window::f_invalidate, t_with_application_thread>())
		(L"scroll", t_member<void (t_window::*)(int, int, unsigned, unsigned, int, int), &t_window::f_scroll, t_with_application_thread>())
		(L"from_screen", t_member<t_point (t_window::*)(const t_point&) const, &t_window::f_from_screen, t_with_application_thread>())
		(L"to_screen", t_member<t_point (t_window::*)(const t_point&) const, &t_window::f_to_screen, t_with_application_thread>())
		(L"input_context", t_member<const xraft::t_pointer<t_input_context>& (t_window::*)() const, &t_window::f_input_context, t_with_application_thread>())
		(L"input_context__", t_member<void (t_window::*)(const xraft::t_pointer<t_input_context>&), &t_window::f_input_context__, t_with_application_thread>())
		(L"cursor", t_member<t_cursor (t_window::*)() const, &t_window::f_cursor, t_with_application_thread>())
		(L"cursor__", t_member<void (t_window::*)(t_cursor), &t_window::f_cursor__, t_with_application_thread>())
	;
}

void t_type_of<t_widget>::f_define(t_extension* a_extension)
{
	t_define<t_widget, t_window>(a_extension, L"Widget")
#define T_WINDOW t_widget
#include "window_define.h"
#undef T_WINDOW
		(L"parent", t_member<xraft::t_pointer<t_window> (t_widget::*)() const, &t_widget::f_parent, t_with_application_thread>())
	;
}

t_type* t_type_of<t_widget>::f_derive(::xemmai::t_object* a_this)
{
	return new t_type_of(v_module, a_this);
}

t_scoped t_type_of<t_widget>::f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped (*)(::xemmai::t_object*), xraft::xemmai::t_widget::f_construct>::t_bind<t_widget>::f_do(a_class, a_stack, 0);
}

void t_type_of<t_shell>::f_define(t_extension* a_extension)
{
	t_define<t_shell, t_window>(a_extension, L"Shell")
#define T_WINDOW t_shell
#include "window_define.h"
#undef T_WINDOW
		(a_extension->v_symbol_on_activate, t_member<void (*)(t_shell&), xraft::xemmai::t_shell::f_super__on_activate, t_with_application_thread>())
		(a_extension->v_symbol_on_deactivate, t_member<void (*)(t_shell&), xraft::xemmai::t_shell::f_super__on_deactivate, t_with_application_thread>())
	;
}

t_type* t_type_of<t_shell>::f_derive(::xemmai::t_object* a_this)
{
	return new t_type_of(v_module, a_this);
}

t_scoped t_type_of<t_shell>::f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped (*)(::xemmai::t_object*), xraft::xemmai::t_shell::f_construct>::t_bind<t_shell>::f_do(a_class, a_stack, 0);
}

void t_type_of<t_frame>::f_define(t_extension* a_extension)
{
	t_define<t_frame, t_shell>(a_extension, L"Frame")
#define T_WINDOW t_frame
#include "window_define.h"
#undef T_WINDOW
		(a_extension->v_symbol_on_activate, t_member<void (*)(t_frame&), xraft::xemmai::t_frame::f_super__on_activate, t_with_application_thread>())
		(a_extension->v_symbol_on_deactivate, t_member<void (*)(t_frame&), xraft::xemmai::t_frame::f_super__on_deactivate, t_with_application_thread>())
		(a_extension->v_symbol_on_close, t_member<void (*)(t_frame&), xraft::xemmai::t_frame::f_super__on_close, t_with_application_thread>())
		(L"caption", t_member<const std::wstring& (t_frame::*)() const, &t_frame::f_caption, t_with_application_thread>())
		(L"caption__", t_member<void (t_frame::*)(const std::wstring&), &t_frame::f_caption__, t_with_application_thread>())
	;
}

t_type* t_type_of<t_frame>::f_derive(::xemmai::t_object* a_this)
{
	return new t_type_of(v_module, a_this);
}

t_scoped t_type_of<t_frame>::f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped (*)(::xemmai::t_object*), xraft::xemmai::t_frame::f_construct>::t_bind<t_frame>::f_do(a_class, a_stack, 0);
}

}
