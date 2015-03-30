#ifndef XRAFT__XEMMAI__XRAFTWM_H
#define XRAFT__XEMMAI__XRAFTWM_H

#include "pixmap.h"
#include "window.h"

#include <xraftwm/common.h>

namespace xraft
{

namespace xemmai
{

using namespace ::xemmai;

struct t_xraftwm : ::xemmai::t_extension
{
	template<typename T, typename T_super> friend class t_define;

	t_slot v_module_xraft;
	xraft::xemmai::t_extension* v_xraft;
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
	t_slot v_type_side;
	t_slot v_type_client;
	t_slot v_type_root;

	template<typename T>
	void f_type__(t_scoped&& a_type);

	t_xraftwm(xemmai::t_object* a_module, t_scoped&& a_xraft);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	const T* f_extension() const
	{
		return v_xraft->f_extension<T>();
	}
	template<typename T>
	xemmai::t_object* f_type() const
	{
		return v_xraft->f_type<T>();
	}
	template<typename T>
	t_scoped f_as(const T& a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(f_extension<typename t::t_extension>(), a_value);
	}
};

template<>
inline void t_xraftwm::f_type__<t_side>(t_scoped&& a_type)
{
	v_type_side = std::move(a_type);
}

template<>
inline void t_xraftwm::f_type__<t_client>(t_scoped&& a_type)
{
	v_type_client = std::move(a_type);
}

template<>
inline void t_xraftwm::f_type__<t_root>(t_scoped&& a_type)
{
	v_type_root = std::move(a_type);
}

template<>
inline const t_xraftwm* t_xraftwm::f_extension<t_xraftwm>() const
{
	return this;
}

template<>
inline xemmai::t_object* t_xraftwm::f_type<t_side>() const
{
	return v_type_side;
}

template<>
inline xemmai::t_object* t_xraftwm::f_type<t_client>() const
{
	return v_type_client;
}

template<>
inline xemmai::t_object* t_xraftwm::f_type<t_root>() const
{
	return v_type_root;
}

}

}

namespace xemmai
{

using namespace xraft;

template<>
struct t_type_of<t_side> : t_enum_of<t_side, xraft::xemmai::t_xraftwm>
{
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

template<>
struct t_type_of<t_client> : t_type_of<t_widget>
{
	typedef xraft::xemmai::t_xraftwm t_extension;

	static t_scoped f_borders(t_client& a_self);
	static void f_borders__(t_client& a_self, t_scoped&& a_borders);
	static void f_define(t_extension* a_extension);

	using t_type_of<t_widget>::t_type_of;
	virtual t_type* f_derive(::xemmai::t_object* a_this);
	virtual t_scoped f_construct(::xemmai::t_object* a_class, t_scoped* a_stack, size_t a_n);
};

template<>
struct t_type_of<t_root> : t_type_of<t_window>
{
	typedef xraft::xemmai::t_xraftwm t_extension;

	static void f_define(t_extension* a_extension);

	using t_type_of<t_window>::t_type_of;
	virtual t_type* f_derive(::xemmai::t_object* a_this);
	virtual t_scoped f_construct(::xemmai::t_object* a_class, t_scoped* a_stack, size_t a_n);
};

}

#endif
