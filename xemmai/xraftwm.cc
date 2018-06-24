#include "pixmap.h"
#include "window.h"

#include <xraftwm/common.h>
#include <xemmai/array.h>

namespace xemmaix::xraft
{

using namespace xemmai;

struct t_xraftwm : xemmai::t_extension
{
	t_slot v_module_xraft;
	xemmaix::xraft::t_extension* v_xraft;
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

	t_xraftwm(xemmai::t_object* a_module, t_scoped&& a_xraft);
	virtual void f_scan(t_scan a_scan)
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
	template<typename T>
	const T* f_extension() const
	{
		return v_xraft->f_extension<T>();
	}
	template<typename T>
	t_slot_of<t_type>& f_type_slot()
	{
		return v_xraft->f_type_slot<T>();
	}
	template<typename T>
	t_type* f_type() const
	{
		return const_cast<t_xraftwm*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_scoped f_as(T&& a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(f_extension<typename t::t_extension>(), std::forward<T>(a_value));
	}
};

template<>
inline const t_xraftwm* t_xraftwm::f_extension<t_xraftwm>() const
{
	return this;
}

template<>
inline t_slot_of<t_type>& t_xraftwm::f_type_slot<t_side>()
{
	return v_type_side;
}

template<>
inline t_slot_of<t_type>& t_xraftwm::f_type_slot<t_client>()
{
	return v_type_client;
}

template<>
inline t_slot_of<t_type>& t_xraftwm::f_type_slot<t_root>()
{
	return v_type_root;
}

}

namespace xemmai
{

template<>
struct t_type_of<xraft::t_side> : t_enum_of<xraft::t_side, xemmaix::xraft::t_xraftwm>
{
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

template<>
struct t_type_of<xraft::t_client> : t_type_of<xraft::t_widget>
{
	typedef xemmaix::xraft::t_xraftwm t_extension;

	static t_scoped f_borders(xraft::t_client& a_self);
	static void f_borders__(xraft::t_client& a_self, t_scoped&& a_borders);
	static void f_define(t_extension* a_extension);

	using t_type_of<xraft::t_widget>::t_type_of;
	virtual t_type* f_derive();
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
};

template<>
struct t_type_of<xraft::t_root> : t_type_of<xraft::t_window>
{
	typedef xemmaix::xraft::t_xraftwm t_extension;

	static void f_define(t_extension* a_extension);

	using t_type_of<xraft::t_window>::t_type_of;
	virtual t_type* f_derive();
	virtual t_scoped f_construct(t_stacked* a_stack, size_t a_n);
};

}

namespace xemmaix
{

namespace xraft
{

t_xraftwm::t_xraftwm(xemmai::t_object* a_module, t_scoped&& a_xraft) : xemmai::t_extension(a_module), v_module_xraft(std::move(a_xraft))
{
	v_xraft = xemmai::f_extension<xemmaix::xraft::t_extension>(v_module_xraft);
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
	v_symbol_on_name = t_symbol::f_instantiate(L"on_name");
	v_symbol_on_protocols = t_symbol::f_instantiate(L"on_protocols");
	v_symbol_on_client = t_symbol::f_instantiate(L"on_client");
	t_type_of<t_side>::f_define(this);
	t_type_of<xraft::t_client>::f_define(this);
	t_type_of<xraft::t_root>::f_define(this);
	f_define<xraft::t_root*(*)(), f_root>(this, L"root");
}

struct t_client : ::xraft::t_client, t_wrapper<t_client>
{
	typedef t_xraftwm t_extension;

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
		auto extension = f_extension<t_extension>(f_self(this)->f_type()->v_module);
		f_self(this)->f_invoke(extension->v_symbol_on_activate);
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
		auto extension = f_extension<t_extension>(f_self(this)->f_type()->v_module);
		f_self(this)->f_invoke(extension->v_symbol_on_deactivate);
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
		auto extension = f_extension<t_extension>(f_self(this)->f_type()->v_module);
		f_self(this)->f_invoke(extension->v_symbol_on_name);
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
		auto extension = f_extension<t_extension>(f_self(this)->f_type()->v_module);
		f_self(this)->f_invoke(extension->v_symbol_on_protocols);
	}
};

struct t_root : ::xraft::t_root, t_wrapper<t_root>
{
	typedef t_xraftwm t_extension;

#define T_WINDOW t_root
#include "window_override.h"
#undef T_WINDOW
	virtual t_pointer<::xraft::t_client> f_on_client()
	{
		auto extension = f_extension<t_extension>(f_self(this)->f_type()->v_module);
		t_scoped p = f_self(this)->f_invoke(extension->v_symbol_on_client);
		f_check<::xraft::t_client>(p, L"client");
		t_pointer<::xraft::t_client> q = f_as<::xraft::t_client*>(p);
		q->f_release();
		return q;
	}
};

}

}

namespace xemmai
{

void t_type_of<xraft::t_side>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	t_define<t_side, intptr_t>(a_extension, L"Side")
		(L"NONE", e_side__NONE)
		(L"NEAR", e_side__NEAR)
		(L"FAR", e_side__FAR)
		(L"BOTH", e_side__BOTH)
	;
}

t_scoped t_type_of<xraft::t_client>::f_borders(xraft::t_client& a_self)
{
	const unsigned* borders = a_self.f_borders();
	t_scoped tuple = t_array::f_instantiate();
	for (size_t i = 0; i < 4; ++i) f_as<t_array&>(tuple).f_push(f_global()->f_as(borders[i]));
	return tuple;
}

void t_type_of<xraft::t_client>::f_borders__(xraft::t_client& a_self, t_scoped&& a_borders)
{
	unsigned borders[4];
	for (size_t i = 0; i < 4; ++i) {
		t_scoped x = a_borders.f_get_at(f_global()->f_as(i));
		f_check<unsigned>(x, L"border");
		borders[i] = f_as<unsigned>(x);
	}
	a_self.f_borders__(borders);
}

void t_type_of<xraft::t_client>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	using xemmaix::xraft::t_with_application_thread;
	t_define<xraft::t_client, xraft::t_widget>(a_extension, L"Client")
#define T_WINDOW t_client
#include "window_define.h"
#undef T_WINDOW
		(a_extension->v_symbol_on_activate, t_member<void(*)(t_client&), xemmaix::xraft::t_client::f_super__on_activate, t_with_application_thread>())
		(a_extension->v_symbol_on_deactivate, t_member<void(*)(t_client&), xemmaix::xraft::t_client::f_super__on_deactivate, t_with_application_thread>())
		(a_extension->v_symbol_on_name, t_member<void(*)(t_client&), xemmaix::xraft::t_client::f_super__on_name, t_with_application_thread>())
		(a_extension->v_symbol_on_protocols, t_member<void(*)(t_client&), xemmaix::xraft::t_client::f_super__on_protocols, t_with_application_thread>())
		(L"borders", t_member<t_scoped(*)(t_client&), f_borders, t_with_application_thread>())
		(L"borders__", t_member<void(*)(t_client&, t_scoped&&), f_borders__, t_with_application_thread>())
		(L"move", t_member<void(t_client::*)(t_side, int, t_side, int), &t_client::f_move, t_with_application_thread>())
		(L"show", t_member<void(t_client::*)(), &t_client::f_show, t_with_application_thread>())
		(L"hide", t_member<void(t_client::*)(), &t_client::f_hide, t_with_application_thread>())
		(L"name", t_member<const std::wstring&(t_client::*)() const, &t_client::f_name, t_with_application_thread>())
		(L"delta", t_member<t_extent(t_client::*)() const, &t_client::f_delta, t_with_application_thread>())
		(L"base", t_member<t_extent(t_client::*)() const, &t_client::f_base, t_with_application_thread>())
		(L"extent", t_member<t_extent(t_client::*)() const, &t_client::f_extent, t_with_application_thread>())
		(L"closable", t_member<bool(t_client::*)() const, &t_client::f_closable, t_with_application_thread>())
		(L"close", t_member<void(t_client::*)(), &t_client::f_close, t_with_application_thread>())
		(L"shaded", t_member<bool(t_client::*)() const, &t_client::f_shaded, t_with_application_thread>())
		(L"shaded__", t_member<void(t_client::*)(bool), &t_client::f_shaded__, t_with_application_thread>())
	;
}

t_type* t_type_of<xraft::t_client>::f_derive()
{
	return new t_type_of(v_module, this);
}

t_scoped t_type_of<xraft::t_client>::f_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_type*), xemmaix::xraft::t_client::f_construct>::t_bind<xraft::t_client>::f_do(this, a_stack, a_n);
}

void t_type_of<xraft::t_root>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	using xemmaix::xraft::t_with_application_thread;
	t_define<t_root, t_window>(a_extension, L"Root")
#define T_WINDOW t_root
#include "window_define.h"
#undef T_WINDOW
		(L"run", t_member<void(t_root::*)(), &t_root::f_run, t_with_application_thread>())
		(L"active", t_member<t_pointer<t_client>(t_root::*)() const, &t_root::f_active, t_with_application_thread>())
		(L"active__", t_member<void(t_root::*)(const t_pointer<t_client>&), &t_root::f_active__, t_with_application_thread>())
		(L"background",
			t_member<void(t_root::*)(t_pixel), &t_root::f_background, t_with_application_thread>(),
			t_member<void(t_root::*)(const t_pointer<t_pixmap>&), &t_root::f_background, t_with_application_thread>()
		)
		(L"share_background", t_member<void(t_root::*)(const t_pointer<t_pixmap>&), &t_root::f_share_background, t_with_application_thread>())
	;
}

t_type* t_type_of<xraft::t_root>::f_derive()
{
	return new t_type_of(v_module, this);
}

t_scoped t_type_of<xraft::t_root>::f_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_type*), xemmaix::xraft::t_root::f_construct>::t_bind<xraft::t_root>::f_do(this, a_stack, a_n);
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	return new xemmaix::xraft::t_xraftwm(a_module, xemmai::t_module::f_instantiate(L"xraft"));
}
