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
	t_scoped v_object;
	size_t v_n = 0;

	template<typename T>
	t_proxy(t_type* a_class, T* a_p) : v_application(f_application()), v_object(xemmai::t_object::f_allocate(a_class))
	{
		a_p->f_user__(this);
		v_object.f_pointer__(a_p);
	}
	XRAFT__XEMMAI__EXPORT virtual void f_destroy();

public:
	template<typename T>
	static t_scoped f_wrap(t_type* a_class, T* a_value)
	{
		if (!a_value) return t_value();
		t_proxy* proxy = static_cast<t_proxy*>(a_value->f_user());
		if (!proxy) proxy = new t_proxy(a_class, a_value);
		return proxy->v_object;
	}
	template<typename T>
	static t_scoped f_construct(t_type* a_class, T* a_p)
	{
		t_proxy* proxy = new t_proxy(a_class, a_p);
		proxy->f_acquire();
		return proxy->v_object;
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
	static t_scoped f_construct(t_type* a_class)
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

struct t_extension : xemmai::t_extension
{
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

	static void f_main(t_extension* a_extension, const t_value& a_arguments, const t_value& a_callable);
	static t_scoped f_application(t_extension* a_extension)
	{
		return a_extension->v_application;
	}

public:
	t_extension(xemmai::t_object* a_module);
	virtual void f_scan(t_scan a_scan);
	template<typename T>
	const T* f_extension() const
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
		return const_cast<t_extension*>(this)->f_type_slot<T>();
	}
	template<typename T>
	t_scoped f_as(T&& a_value) const
	{
		typedef t_type_of<typename t_fundamental<T>::t_type> t;
		return t::f_transfer(f_extension<typename t::t_extension>(), std::forward<T>(a_value));
	}
};

template<>
inline const t_extension* t_extension::f_extension<t_extension>() const
{
	return this;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_application>()
{
	return v_type_application;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_point>()
{
	return v_type_point;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_extent>()
{
	return v_type_extent;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_rectangle>()
{
	return v_type_rectangle;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<::xraft::t_object>()
{
	return v_type_object;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_font>()
{
	return v_type_font;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_color>()
{
	return v_type_color;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_graphics>()
{
	return v_type_graphics;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_graphics::t_function>()
{
	return v_type_graphics__function;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_input_attribute>()
{
	return v_type_input_attribute;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_input_context>()
{
	return v_type_input_context;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_drawable>()
{
	return v_type_drawable;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_bitmap>()
{
	return v_type_bitmap;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_pixmap>()
{
	return v_type_pixmap;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_region>()
{
	return v_type_region;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_key>()
{
	return v_type_key;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_modifier>()
{
	return v_type_modifier;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_button>()
{
	return v_type_button;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_timer>()
{
	return v_type_timer;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_cross_mode>()
{
	return v_type_cross_mode;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_cross_detail>()
{
	return v_type_cross_detail;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_window>()
{
	return v_type_window;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_widget>()
{
	return v_type_widget;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_shell>()
{
	return v_type_shell;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_frame>()
{
	return v_type_frame;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_opengl_format>()
{
	return v_type_opengl_format;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_opengl_widget>()
{
	return v_type_opengl_widget;
}

template<>
inline t_slot_of<t_type>& t_extension::f_type_slot<t_opengl_context>()
{
	return v_type_opengl_context;
}

template<typename T_base>
struct t_derivable : T_base
{
	typedef t_derivable t_base;

	using T_base::T_base;
	t_type* f_do_derive()
	{
		return new t_type_of<typename T_base::t_what>(t_type_of<typename T_base::t_what>::V_ids, this, this->v_module);
	}
};

}

#endif
