#include "window.h"

namespace xemmaix::xraft
{

struct t_widget : ::xraft::t_widget, t_wrapper<t_widget>
{
#define T_WINDOW t_widget
#include "window_override.h"
#undef T_WINDOW
};

struct t_shell : ::xraft::t_shell, t_wrapper<t_shell>
{
#define T_WINDOW t_shell
#include "window_override.h"
#undef T_WINDOW
	static void f_super__on_activate(::xraft::t_shell& a_self)
	{
		if (dynamic_cast<t_shell*>(&a_self))
			a_self.::xraft::t_shell::f_on_activate();
		else
			a_self.f_on_activate();
	}
	virtual void f_on_activate()
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_activate, index);
	}
	static void f_super__on_deactivate(::xraft::t_shell& a_self)
	{
		if (dynamic_cast<t_shell*>(&a_self))
			a_self.::xraft::t_shell::f_on_deactivate();
		else
			a_self.f_on_deactivate();
	}
	virtual void f_on_deactivate()
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_deactivate, index);
	}
};

struct t_frame : ::xraft::t_frame, t_wrapper<t_frame>
{
#define T_WINDOW t_frame
#include "window_override.h"
#undef T_WINDOW
	static void f_super__on_activate(::xraft::t_frame& a_self)
	{
		if (dynamic_cast<t_frame*>(&a_self))
			a_self.::xraft::t_frame::f_on_activate();
		else
			a_self.f_on_activate();
	}
	virtual void f_on_activate()
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_activate, index);
	}
	static void f_super__on_deactivate(::xraft::t_frame& a_self)
	{
		if (dynamic_cast<t_frame*>(&a_self))
			a_self.::xraft::t_frame::f_on_deactivate();
		else
			a_self.f_on_deactivate();
	}
	virtual void f_on_deactivate()
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_deactivate, index);
	}
	static void f_super__on_close(::xraft::t_frame& a_self)
	{
		if (dynamic_cast<t_frame*>(&a_self))
			a_self.::xraft::t_frame::f_on_close();
		else
			a_self.f_on_close();
	}
	virtual void f_on_close()
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_close, index);
	}
};

}

namespace xemmai
{

t_object* t_type_of<xraft::t_cross_mode>::f_define(t_library* a_library)
{
	using namespace xraft;
	return t_base::f_define(a_library, [](auto a_fields)
	{
		a_fields
		(L"NORMAL"sv, c_cross_mode__NORMAL)
		(L"GRAB"sv, c_cross_mode__GRAB)
		(L"UNGRAB"sv, c_cross_mode__UNGRAB)
		;
	});
}

t_object* t_type_of<xraft::t_cross_detail>::f_define(t_library* a_library)
{
	using namespace xraft;
	return t_base::f_define(a_library, [](auto a_fields)
	{
		a_fields
		(L"OUTER"sv, c_cross_detail__OUTER)
		(L"INNER"sv, c_cross_detail__INNER)
		(L"THROUGH"sv, c_cross_detail__THROUGH)
		;
	});
}

void t_type_of<xraft::t_window>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
	(L"count"sv, t_member<size_t(t_window::*)() const, &t_window::f_count>())
	(L"at"sv, t_member<const xraft::t_pointer<t_widget>&(t_window::*)(size_t) const, &t_window::f_at>())
	(a_library->v_symbol_add,
		t_member<void(*)(t_window&, const xraft::t_pointer<t_widget>&), f__add>(),
		t_member<void(*)(t_window&, const xraft::t_pointer<t_widget>&, size_t), f__add>()
	)
	(a_library->v_symbol_remove, t_member<void(t_window::*)(size_t), &t_window::f_remove>())
	(L"geometry"sv, t_member<const t_rectangle&(t_window::*)() const, &t_window::f_geometry>())
	(L"move"sv, t_member<void(t_window::*)(const t_rectangle&), &t_window::f_move>())
	(L"visible"sv, t_member<bool(t_window::*)() const, &t_window::f_visible>())
	(L"show"sv, t_member<void(t_window::*)(), &t_window::f_show>())
	(L"hide"sv, t_member<void(t_window::*)(), &t_window::f_hide>())
	(L"invalidate"sv, t_member<void(t_window::*)(int, int, unsigned, unsigned), &t_window::f_invalidate>())
	(L"scroll"sv, t_member<void(t_window::*)(int, int, unsigned, unsigned, int, int), &t_window::f_scroll>())
	(L"from_screen"sv, t_member<t_point(t_window::*)(const t_point&) const, &t_window::f_from_screen>())
	(L"to_screen"sv, t_member<t_point(t_window::*)(const t_point&) const, &t_window::f_to_screen>())
	(L"input_context"sv, t_member<const xraft::t_pointer<t_input_context>&(t_window::*)() const, &t_window::f_input_context>())
	(L"input_context__"sv, t_member<void(t_window::*)(const xraft::t_pointer<t_input_context>&), &t_window::f_input_context__>())
	(L"cursor"sv, t_member<t_cursor(t_window::*)() const, &t_window::f_cursor>())
	(L"cursor__"sv, t_member<void(t_window::*)(t_cursor), &t_window::f_cursor__>())
	.f_derive<t_window, t_drawable>();
}

void t_type_of<xraft::t_widget>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
#define T_WINDOW t_widget
#include "window_define.h"
#undef T_WINDOW
	(L"parent"sv, t_member<xraft::t_pointer<t_window>(t_widget::*)() const, &t_widget::f_parent>())
	.f_derive<t_widget, t_window>();
}

t_pvalue t_type_of<xraft::t_widget>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_pvalue(*)(t_type*), xemmaix::xraft::t_widget::f_construct>::t_bind<xraft::t_widget>::f_do(this, a_stack, 0);
}

void t_type_of<xraft::t_shell>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
#define T_WINDOW t_shell
#include "window_define.h"
#undef T_WINDOW
	(a_library->v_symbol_on_activate, t_member<void(*)(t_shell&), xemmaix::xraft::t_shell::f_super__on_activate>())
	(a_library->v_symbol_on_deactivate, t_member<void(*)(t_shell&), xemmaix::xraft::t_shell::f_super__on_deactivate>())
	.f_derive<t_shell, t_window>();
}

t_pvalue t_type_of<xraft::t_shell>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_pvalue(*)(t_type*), xemmaix::xraft::t_shell::f_construct>::t_bind<xraft::t_shell>::f_do(this, a_stack, 0);
}

void t_type_of<xraft::t_frame>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
#define T_WINDOW t_frame
#include "window_define.h"
#undef T_WINDOW
	(a_library->v_symbol_on_activate, t_member<void(*)(t_frame&), xemmaix::xraft::t_frame::f_super__on_activate>())
	(a_library->v_symbol_on_deactivate, t_member<void(*)(t_frame&), xemmaix::xraft::t_frame::f_super__on_deactivate>())
	(a_library->v_symbol_on_close, t_member<void(*)(t_frame&), xemmaix::xraft::t_frame::f_super__on_close>())
	(L"caption"sv, t_member<std::wstring_view(t_frame::*)() const, &t_frame::f_caption>())
	(L"caption__"sv, t_member<void(t_frame::*)(std::wstring_view), &t_frame::f_caption__>())
	.f_derive<t_frame, t_shell>();
}

t_pvalue t_type_of<xraft::t_frame>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_pvalue(*)(t_type*), xemmaix::xraft::t_frame::f_construct>::t_bind<xraft::t_frame>::f_do(this, a_stack, 0);
}

}
