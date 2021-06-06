#ifndef XEMMAIX__XRAFT__XRAFT_H
#define XEMMAIX__XRAFT__XRAFT_H

#ifdef XRAFT_WIN32
#ifndef XRAFT__XEMMAI__EXPORT
#define XRAFT__XEMMAI__EXPORT __declspec(dllimport)
#endif
#else
#define XRAFT__XEMMAI__EXPORT
#endif

#include <xraft/graphics.h>
#include <xraft/opengl.h>
#include <xemmai/convert.h>

namespace xemmaix::xraft
{

using namespace ::xraft;
using namespace xemmai;

class t_entry
{
protected:
	t_entry* v_previous;
	t_entry* v_next;

	t_entry(bool) : v_previous(this), v_next(this)
	{
	}
	XRAFT__XEMMAI__EXPORT t_entry();
	void f_unlink()
	{
		v_previous->v_next = v_next;
		v_next->v_previous = v_previous;
		v_previous = v_next = nullptr;
	}

public:
	virtual void f_dispose();
};

class t_proxy : public t_user, public t_entry
{
	t_application* v_application;
	t_root v_object;
	size_t v_n = 0;

	template<typename T>
	t_proxy(t_type* a_class, T* a_p) : v_application(f_application()), v_object(f_engine()->f_allocate(xemmai::t_object::f_align_for_fields(sizeof(T*)) + sizeof(t_svalue) * a_class->v_instance_fields))
	{
		a_p->f_user__(this);
		v_object->f_as<T*>() = a_p;
		std::uninitialized_default_construct_n(v_object->f_fields(sizeof(T*)), a_class->v_instance_fields);
		v_object->f_be(a_class);
	}
	XRAFT__XEMMAI__EXPORT virtual void f_destroy();

public:
	template<typename T>
	static t_pvalue f_wrap(t_type* a_class, T* a_value)
	{
		if (!a_value) return {};
		auto proxy = static_cast<t_proxy*>(a_value->f_user());
		if (!proxy) proxy = new t_proxy(a_class, a_value);
		return static_cast<xemmai::t_object*>(proxy->v_object);
	}
	template<typename T>
	static t_pvalue f_construct(t_type* a_class, T* a_p)
	{
		auto proxy = new t_proxy(a_class, a_p);
		proxy->f_acquire();
		return static_cast<xemmai::t_object*>(proxy->v_object);
	}

	XRAFT__XEMMAI__EXPORT virtual void f_dispose();
	xemmai::t_object* f_object() const
	{
		return v_object;
	}
	void f_acquire();
	void f_release();
};

inline xemmai::t_object* f_self(const ::xraft::t_object* a_this)
{
	return static_cast<t_proxy*>(a_this->f_user())->f_object();
}

template<typename T>
struct t_wrapper
{
	static t_pvalue f_construct(t_type* a_class)
	{
		return t_proxy::f_construct(a_class, new T());
	}
};

struct t_library : xemmai::t_library
{
	std::mutex v_mutex;
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
	t_slot v_symbol_on_close;
	t_slot v_symbol_on_create;
	t_slot v_symbol_on_destroy;

private:
	t_slot_of<t_type> v_type_application;
	t_slot_of<t_type> v_type_point;
	t_slot_of<t_type> v_type_extent;
	t_slot_of<t_type> v_type_rectangle;
	t_slot_of<t_type> v_type_object;
	t_slot_of<t_type> v_type_font;
	t_slot_of<t_type> v_type_color;
	t_slot_of<t_type> v_type_graphics;
	t_slot_of<t_type> v_type_graphics__function;
	t_slot_of<t_type> v_type_input_attribute;
	t_slot_of<t_type> v_type_input_context;
	t_slot_of<t_type> v_type_drawable;
	t_slot_of<t_type> v_type_bitmap;
	t_slot_of<t_type> v_type_pixmap;
	t_slot_of<t_type> v_type_region;
	t_slot_of<t_type> v_type_key;
	t_slot_of<t_type> v_type_modifier;
	t_slot_of<t_type> v_type_button;
	t_slot_of<t_type> v_type_timer;
	t_slot_of<t_type> v_type_cross_mode;
	t_slot_of<t_type> v_type_cross_detail;
	t_slot_of<t_type> v_type_window;
	t_slot_of<t_type> v_type_widget;
	t_slot_of<t_type> v_type_shell;
	t_slot_of<t_type> v_type_frame;
	t_slot_of<t_type> v_type_opengl_format;
	t_slot_of<t_type> v_type_opengl_widget;
	t_slot_of<t_type> v_type_opengl_context;
	xemmai::t_object* v_application;

	static void f_main(t_library* a_library, const t_pvalue& a_arguments, const t_pvalue& a_callable);
	static t_pvalue f_application(t_library* a_library)
	{
		return a_library->v_application;
	}

public:
	using xemmai::t_library::t_library;
	virtual void f_scan(t_scan a_scan);
	virtual std::vector<std::pair<t_root, t_rvalue>> f_define();
	template<typename T>
	const T* f_library() const
	{
		return f_global();
	}
	template<typename T>
	t_slot_of<t_type>& f_type_slot()
	{
		return f_global()->f_type_slot<T>();
	}
	template<typename T>
	t_type* f_type() const
	{
		return const_cast<t_library*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_pvalue f_as(T&& a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(f_library<typename t::t_library>(), std::forward<T>(a_value));
	}
};

template<>
inline const t_library* t_library::f_library<t_library>() const
{
	return this;
}

XEMMAI__LIBRARY__TYPE(t_library, application)
XEMMAI__LIBRARY__TYPE(t_library, point)
XEMMAI__LIBRARY__TYPE(t_library, extent)
XEMMAI__LIBRARY__TYPE(t_library, rectangle)
XEMMAI__LIBRARY__TYPE_AS(t_library, ::xraft::t_object, object)
XEMMAI__LIBRARY__TYPE(t_library, font)
XEMMAI__LIBRARY__TYPE_AS(t_library, ::xraft::t_color, color)
XEMMAI__LIBRARY__TYPE(t_library, graphics)
XEMMAI__LIBRARY__TYPE_AS(t_library, t_graphics::t_function, graphics__function)
XEMMAI__LIBRARY__TYPE(t_library, input_attribute)
XEMMAI__LIBRARY__TYPE(t_library, input_context)
XEMMAI__LIBRARY__TYPE(t_library, drawable)
XEMMAI__LIBRARY__TYPE(t_library, bitmap)
XEMMAI__LIBRARY__TYPE(t_library, pixmap)
XEMMAI__LIBRARY__TYPE(t_library, region)
XEMMAI__LIBRARY__TYPE(t_library, key)
XEMMAI__LIBRARY__TYPE(t_library, modifier)
XEMMAI__LIBRARY__TYPE(t_library, button)
XEMMAI__LIBRARY__TYPE(t_library, timer)
XEMMAI__LIBRARY__TYPE(t_library, cross_mode)
XEMMAI__LIBRARY__TYPE(t_library, cross_detail)
XEMMAI__LIBRARY__TYPE(t_library, window)
XEMMAI__LIBRARY__TYPE(t_library, widget)
XEMMAI__LIBRARY__TYPE(t_library, shell)
XEMMAI__LIBRARY__TYPE(t_library, frame)
XEMMAI__LIBRARY__TYPE(t_library, opengl_format)
XEMMAI__LIBRARY__TYPE(t_library, opengl_widget)
XEMMAI__LIBRARY__TYPE(t_library, opengl_context)

template<typename T, typename T_base = t_type>
struct t_bears_pointer : t_bears<T, T_base>
{
	using t_base = t_bears_pointer;

	static constexpr size_t V_native = sizeof(T*);

	using t_bears<T, T_base>::t_bears;
};

}

#endif
