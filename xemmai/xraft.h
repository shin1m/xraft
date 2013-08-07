#ifndef XRAFT__XEMMAI__XRAFT_H
#define XRAFT__XEMMAI__XRAFT_H

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

namespace xraft
{

namespace xemmai
{

using namespace ::xemmai;

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
	t_scoped v_object;
	size_t v_n = 0;

	template<typename T>
	t_proxy(::xemmai::t_object* a_class, T* a_p) : v_application(f_application()), v_object(::xemmai::t_object::f_allocate(a_class))
	{
		a_p->f_user__(this);
		v_object.f_pointer__(a_p);
	}
	XRAFT__XEMMAI__EXPORT virtual void f_destroy();

public:
	template<typename T>
	static t_scoped f_wrap(::xemmai::t_object* a_class, T* a_value)
	{
		if (!a_value) return t_value();
		t_proxy* proxy = static_cast<t_proxy*>(a_value->f_user());
		if (!proxy) proxy = new t_proxy(a_class, a_value);
		return proxy->v_object;
	}
	template<typename T>
	static t_scoped f_construct(::xemmai::t_object* a_class, T* a_p)
	{
		t_proxy* proxy = new t_proxy(a_class, a_p);
		proxy->f_acquire();
		return proxy->v_object;
	}

	XRAFT__XEMMAI__EXPORT virtual void f_dispose();
	::xemmai::t_object* f_object() const
	{
		return v_object;
	}
	void f_acquire();
	void f_release();
};

inline ::xemmai::t_object* f_self(const xraft::t_object* a_this)
{
	return static_cast<t_proxy*>(a_this->f_user())->f_object();
}

template<typename T>
struct t_wrapper
{
	static t_scoped f_construct(::xemmai::t_object* a_class)
	{
		return t_proxy::f_construct(a_class, new T());
	}
};

struct t_with_application_thread
{
	t_with_application_thread(const t_value& a_self)
	{
		f_application();
	}
};

class t_extension : public ::xemmai::t_extension
{
	template<typename T, typename T_super> friend class ::xemmai::t_define;

public:
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
	t_slot v_type_application;
	t_slot v_type_point;
	t_slot v_type_extent;
	t_slot v_type_rectangle;
	t_slot v_type_object;
	t_slot v_type_font;
	t_slot v_type_color;
	t_slot v_type_graphics;
	t_slot v_type_graphics__function;
	t_slot v_type_input_attribute;
	t_slot v_type_input_context;
	t_slot v_type_drawable;
	t_slot v_type_bitmap;
	t_slot v_type_pixmap;
	t_slot v_type_region;
	t_slot v_type_key;
	t_slot v_type_modifier;
	t_slot v_type_button;
	t_slot v_type_timer;
	t_slot v_type_cross_mode;
	t_slot v_type_cross_detail;
	t_slot v_type_window;
	t_slot v_type_widget;
	t_slot v_type_shell;
	t_slot v_type_frame;
	t_slot v_type_opengl_format;
	t_slot v_type_opengl_widget;
	t_slot v_type_opengl_context;
	::xemmai::t_object* v_application;

	static void f_main(t_extension* a_extension, const t_value& a_arguments, const t_value& a_callable);
	static t_scoped f_application(t_extension* a_extension)
	{
		return a_extension->v_application;
	}

	template<typename T>
	void f_type__(t_scoped&& a_type);

public:
	t_extension(xemmai::t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	const T* f_extension() const
	{
		return f_global();
	}
	template<typename T>
	xemmai::t_object* f_type() const
	{
		return f_global()->f_type<T>();
	}
	template<typename T>
	t_scoped f_as(const T& a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(f_extension<typename t::t_extension>(), a_value);
	}
};

template<>
inline void t_extension::f_type__<t_application>(t_scoped&& a_type)
{
	v_type_application = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_point>(t_scoped&& a_type)
{
	v_type_point = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_extent>(t_scoped&& a_type)
{
	v_type_extent = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_rectangle>(t_scoped&& a_type)
{
	v_type_rectangle = std::move(a_type);
}

template<>
inline void t_extension::f_type__<xraft::t_object>(t_scoped&& a_type)
{
	v_type_object = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_font>(t_scoped&& a_type)
{
	v_type_font = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_color>(t_scoped&& a_type)
{
	v_type_color = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_graphics>(t_scoped&& a_type)
{
	v_type_graphics = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_graphics::t_function>(t_scoped&& a_type)
{
	v_type_graphics__function = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_input_attribute>(t_scoped&& a_type)
{
	v_type_input_attribute = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_input_context>(t_scoped&& a_type)
{
	v_type_input_context = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_drawable>(t_scoped&& a_type)
{
	v_type_drawable = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_bitmap>(t_scoped&& a_type)
{
	v_type_bitmap = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_pixmap>(t_scoped&& a_type)
{
	v_type_pixmap = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_region>(t_scoped&& a_type)
{
	v_type_region = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_key>(t_scoped&& a_type)
{
	v_type_key = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_modifier>(t_scoped&& a_type)
{
	v_type_modifier = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_button>(t_scoped&& a_type)
{
	v_type_button = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_timer>(t_scoped&& a_type)
{
	v_type_timer = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_cross_mode>(t_scoped&& a_type)
{
	v_type_cross_mode = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_cross_detail>(t_scoped&& a_type)
{
	v_type_cross_detail = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_window>(t_scoped&& a_type)
{
	v_type_window = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_widget>(t_scoped&& a_type)
{
	v_type_widget = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_shell>(t_scoped&& a_type)
{
	v_type_shell = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_frame>(t_scoped&& a_type)
{
	v_type_frame = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_opengl_format>(t_scoped&& a_type)
{
	v_type_opengl_format = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_opengl_widget>(t_scoped&& a_type)
{
	v_type_opengl_widget = std::move(a_type);
}

template<>
inline void t_extension::f_type__<t_opengl_context>(t_scoped&& a_type)
{
	v_type_opengl_context = std::move(a_type);
}

template<>
inline const t_extension* t_extension::f_extension<t_extension>() const
{
	return this;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_application>() const
{
	return v_type_application;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_point>() const
{
	return v_type_point;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_extent>() const
{
	return v_type_extent;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_rectangle>() const
{
	return v_type_rectangle;
}

template<>
inline xemmai::t_object* t_extension::f_type<xraft::t_object>() const
{
	return v_type_object;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_font>() const
{
	return v_type_font;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_color>() const
{
	return v_type_color;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_graphics>() const
{
	return v_type_graphics;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_graphics::t_function>() const
{
	return v_type_graphics__function;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_input_attribute>() const
{
	return v_type_input_attribute;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_input_context>() const
{
	return v_type_input_context;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_drawable>() const
{
	return v_type_drawable;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_bitmap>() const
{
	return v_type_bitmap;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_pixmap>() const
{
	return v_type_pixmap;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_region>() const
{
	return v_type_region;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_key>() const
{
	return v_type_key;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_modifier>() const
{
	return v_type_modifier;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_button>() const
{
	return v_type_button;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_timer>() const
{
	return v_type_timer;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_cross_mode>() const
{
	return v_type_cross_mode;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_cross_detail>() const
{
	return v_type_cross_detail;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_window>() const
{
	return v_type_window;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_widget>() const
{
	return v_type_widget;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_shell>() const
{
	return v_type_shell;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_frame>() const
{
	return v_type_frame;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_opengl_format>() const
{
	return v_type_opengl_format;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_opengl_widget>() const
{
	return v_type_opengl_widget;
}

template<>
inline xemmai::t_object* t_extension::f_type<t_opengl_context>() const
{
	return v_type_opengl_context;
}

}

}

#endif
