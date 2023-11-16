#include "pixmap.h"
#include "window.h"
#include <xraftwm/common.h>

namespace xemmaix::xraft
{

using namespace xemmai;

struct t_xraftwm : xemmai::t_library
{
	t_slot v_module_xraft;
	xemmaix::xraft::t_library* v_xraft;
	t_slot v_symbol_on_move;
	t_slot v_symbol_on_show;
	t_slot v_symbol_on_hide;
	t_slot v_symbol_on_paint;
	t_slot v_symbol_on_focus_enter;
	t_slot v_symbol_on_focus_leave;
	t_slot v_symbol_on_key_press;
	t_slot v_symbol_on_key_release;
	t_slot v_symbol_on_input_compose;
	t_slot v_symbol_on_input_commit;
	t_slot v_symbol_on_input_spot;
	t_slot v_symbol_on_button_press;
	t_slot v_symbol_on_button_release;
	t_slot v_symbol_on_pointer_enter;
	t_slot v_symbol_on_pointer_leave;
	t_slot v_symbol_on_pointer_move;
	t_slot v_symbol_add;
	t_slot v_symbol_remove;
	t_slot v_symbol_on_activate;
	t_slot v_symbol_on_deactivate;
	t_slot v_symbol_on_name;
	t_slot v_symbol_on_protocols;
	t_slot v_symbol_on_client;
	t_slot_of<t_type> v_type_side;
	t_slot_of<t_type> v_type_client;
	t_slot_of<t_type> v_type_root;

	t_xraftwm(xemmai::t_library::t_handle* a_handle, const t_pvalue& a_xraft) : xemmai::t_library(a_handle), v_module_xraft(a_xraft), v_xraft(&v_module_xraft->f_as<t_module>().v_body->f_as<xemmaix::xraft::t_library>())
	{
	}
	using t_root = xemmai::t_root;
	XEMMAI__LIBRARY__MEMBERS
};

XEMMAI__LIBRARY__BASE(t_xraftwm, t_global, f_global())
XEMMAI__LIBRARY__BASE(t_xraftwm, xemmaix::xraft::t_library, v_xraft)
XEMMAI__LIBRARY__TYPE(t_xraftwm, side)
XEMMAI__LIBRARY__TYPE(t_xraftwm, client)
XEMMAI__LIBRARY__TYPE_AS(t_xraftwm, ::xraft::t_root, root)

}

namespace xemmai
{

template<>
struct t_type_of<xraft::t_side> : t_enum_of<xraft::t_side, xemmaix::xraft::t_xraftwm>
{
	static t_object* f_define(t_library* a_library);

	using t_base::t_base;
};

template<>
struct t_type_of<xraft::t_client> : xemmaix::xraft::t_derivable<xemmaix::xraft::t_bears_pointer<xraft::t_client, t_type_of<xraft::t_widget>>>
{
	using t_library = xemmaix::xraft::t_xraftwm;

	static t_pvalue f_borders(xraft::t_client& a_self);
	static void f_borders__(xraft::t_client& a_self, const t_pvalue& a_borders);
	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

template<>
struct t_type_of<xraft::t_root> : xemmaix::xraft::t_derivable<xemmaix::xraft::t_bears_pointer<xraft::t_root, t_type_of<xraft::t_window>>>
{
	using t_library = xemmaix::xraft::t_xraftwm;

	static void f_define(t_library* a_library);

	using t_base::t_base;
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
};

}

namespace xemmaix::xraft
{

void t_xraftwm::f_scan(t_scan a_scan)
{
	a_scan(v_module_xraft);
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
	a_scan(v_symbol_on_name);
	a_scan(v_symbol_on_protocols);
	a_scan(v_symbol_on_client);
	a_scan(v_type_side);
	a_scan(v_type_client);
	a_scan(v_type_root);
}

std::vector<std::pair<xemmai::t_root, t_rvalue>> t_xraftwm::f_define()
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
	v_symbol_on_name = t_symbol::f_instantiate(L"on_name"sv);
	v_symbol_on_protocols = t_symbol::f_instantiate(L"on_protocols"sv);
	v_symbol_on_client = t_symbol::f_instantiate(L"on_client"sv);
	t_type_of<::xraft::t_client>::f_define(this);
	t_type_of<::xraft::t_root>::f_define(this);
	return t_define(this)
		(L"Side"sv, t_type_of<t_side>::f_define(this))
		(L"Client"sv, static_cast<xemmai::t_object*>(v_type_client))
		(L"Root"sv, static_cast<xemmai::t_object*>(v_type_root))
		(L"root"sv, t_static<::xraft::t_root*(*)(), f_root>())
	;
}

struct t_client : ::xraft::t_client, t_wrapper<t_client>
{
	using t_library = t_xraftwm;

#define T_WINDOW t_client
#include "window_override.h"
#undef T_WINDOW
	static void f_super__on_activate(::xraft::t_client& a_self)
	{
		if (dynamic_cast<t_client*>(&a_self))
			a_self.::xraft::t_client::f_on_activate();
		else
			a_self.f_on_activate();
	}
	virtual void f_on_activate()
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_activate, index);
	}
	static void f_super__on_deactivate(::xraft::t_client& a_self)
	{
		if (dynamic_cast<t_client*>(&a_self))
			a_self.::xraft::t_client::f_on_deactivate();
		else
			a_self.f_on_deactivate();
	}
	virtual void f_on_deactivate()
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_deactivate, index);
	}
	static void f_super__on_name(::xraft::t_client& a_self)
	{
		if (dynamic_cast<t_client*>(&a_self))
			a_self.::xraft::t_client::f_on_name();
		else
			a_self.f_on_name();
	}
	virtual void f_on_name()
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_name, index);
	}
	static void f_super__on_protocols(::xraft::t_client& a_self)
	{
		if (dynamic_cast<t_client*>(&a_self))
			a_self.::xraft::t_client::f_on_protocols();
		else
			a_self.f_on_protocols();
	}
	virtual void f_on_protocols()
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_protocols, index);
	}
};

struct t_root : ::xraft::t_root, t_wrapper<t_root>
{
	using t_library = t_xraftwm;

#define T_WINDOW t_root
#include "window_override.h"
#undef T_WINDOW
	virtual ::xraft::t_pointer<::xraft::t_client> f_on_client()
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		auto p = f_self(this)->f_invoke(library.v_symbol_on_client, index);
		f_check<::xraft::t_client>(p, L"client");
		::xraft::t_pointer<::xraft::t_client> q = f_as<::xraft::t_client*>(p);
		q->f_release();
		return q;
	}
};

}

namespace xemmai
{

t_object* t_type_of<xraft::t_side>::f_define(t_library* a_library)
{
	using namespace xraft;
	return t_base::f_define(a_library, [](auto a_fields)
	{
		a_fields
		(L"NONE"sv, e_side__NONE)
		(L"NEAR"sv, e_side__NEAR)
		(L"FAR"sv, e_side__FAR)
		(L"BOTH"sv, e_side__BOTH)
		;
	});
}

t_pvalue t_type_of<xraft::t_client>::f_borders(xraft::t_client& a_self)
{
	auto borders = a_self.f_borders();
	return f_tuple(borders[0], borders[1], borders[2], borders[3]);
}

void t_type_of<xraft::t_client>::f_borders__(xraft::t_client& a_self, const t_pvalue& a_borders)
{
	unsigned borders[4];
	for (size_t i = 0; i < 4; ++i) {
		auto x = a_borders.f_get_at(i);
		f_check<unsigned>(x, L"border");
		borders[i] = f_as<unsigned>(x);
	}
	a_self.f_borders__(borders);
}

void t_type_of<xraft::t_client>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
#define T_WINDOW t_client
#include "window_define.h"
#undef T_WINDOW
		(a_library->v_symbol_on_activate, t_member<void(*)(t_client&), xemmaix::xraft::t_client::f_super__on_activate>())
		(a_library->v_symbol_on_deactivate, t_member<void(*)(t_client&), xemmaix::xraft::t_client::f_super__on_deactivate>())
		(a_library->v_symbol_on_name, t_member<void(*)(t_client&), xemmaix::xraft::t_client::f_super__on_name>())
		(a_library->v_symbol_on_protocols, t_member<void(*)(t_client&), xemmaix::xraft::t_client::f_super__on_protocols>())
		(L"borders"sv, t_member<t_pvalue(*)(t_client&), f_borders>())
		(L"borders__"sv, t_member<void(*)(t_client&, const t_pvalue&), f_borders__>())
		(L"move"sv, t_member<void(t_client::*)(t_side, int, t_side, int), &t_client::f_move>())
		(L"show"sv, t_member<void(t_client::*)(), &t_client::f_show>())
		(L"hide"sv, t_member<void(t_client::*)(), &t_client::f_hide>())
		(L"name"sv, t_member<std::wstring_view(t_client::*)() const, &t_client::f_name>())
		(L"delta"sv, t_member<t_extent(t_client::*)() const, &t_client::f_delta>())
		(L"base"sv, t_member<t_extent(t_client::*)() const, &t_client::f_base>())
		(L"extent"sv, t_member<t_extent(t_client::*)() const, &t_client::f_extent>())
		(L"closable"sv, t_member<bool(t_client::*)() const, &t_client::f_closable>())
		(L"close"sv, t_member<void(t_client::*)(), &t_client::f_close>())
		(L"shaded"sv, t_member<bool(t_client::*)() const, &t_client::f_shaded>())
		(L"shaded__"sv, t_member<void(t_client::*)(bool), &t_client::f_shaded__>())
	.f_derive<xraft::t_client, xraft::t_widget>();
}

t_pvalue t_type_of<xraft::t_client>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_pvalue(*)(t_type*), xemmaix::xraft::t_client::f_construct>::t_bind<xraft::t_client>::f_do(this, a_stack, a_n);
}

void t_type_of<xraft::t_root>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
#define T_WINDOW t_root
#include "window_define.h"
#undef T_WINDOW
		(L"run"sv, t_member<void(xraft::t_root::*)(), &xraft::t_root::f_run>())
		(L"active"sv, t_member<xraft::t_pointer<t_client>(xraft::t_root::*)() const, &xraft::t_root::f_active>())
		(L"active__"sv, t_member<void(xraft::t_root::*)(const xraft::t_pointer<t_client>&), &xraft::t_root::f_active__>())
		(L"background"sv,
			t_member<void(xraft::t_root::*)(t_pixel), &xraft::t_root::f_background>(),
			t_member<void(xraft::t_root::*)(const xraft::t_pointer<t_pixmap>&), &xraft::t_root::f_background>()
		)
		(L"share_background"sv, t_member<void(xraft::t_root::*)(const xraft::t_pointer<t_pixmap>&), &xraft::t_root::f_share_background>())
	.f_derive<xraft::t_root, t_window>();
}

t_pvalue t_type_of<xraft::t_root>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_pvalue(*)(t_type*), xemmaix::xraft::t_root::f_construct>::t_bind<xraft::t_root>::f_do(this, a_stack, a_n);
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_library::t_handle* a_handle)
{
	using namespace std::literals;
	return xemmai::f_new<xemmaix::xraft::t_xraftwm>(a_handle, xemmai::t_module::f_instantiate(L"xraft"sv));
}
