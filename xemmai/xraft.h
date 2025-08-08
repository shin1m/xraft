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
	static t_pvalue f_wrap(t_type* a_class, auto* a_value)
	{
		if (!a_value) return {};
		auto proxy = static_cast<t_proxy*>(a_value->f_user());
		if (!proxy) proxy = new t_proxy(a_class, a_value);
		return static_cast<xemmai::t_object*>(proxy->v_object);
	}
	static t_pvalue f_construct(t_type* a_class, auto* a_p)
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
#define XEMMAIX__XRAFT__SYMBOLS(_)\
	_(on_move)\
	_(on_show)\
	_(on_hide)\
	_(on_paint)\
	_(on_focus_enter)\
	_(on_focus_leave)\
	_(on_key_press)\
	_(on_key_release)\
	_(on_input_compose)\
	_(on_input_commit)\
	_(on_input_spot)\
	_(on_button_press)\
	_(on_button_release)\
	_(on_pointer_enter)\
	_(on_pointer_leave)\
	_(on_pointer_move)\
	_(add)\
	_(remove)\
	_(on_activate)\
	_(on_deactivate)\
	_(on_close)\
	_(on_create)\
	_(on_destroy)
	XEMMAIX__XRAFT__SYMBOLS(XEMMAI__SYMBOL__DECLARE)

private:
#define XEMMAIX__XRAFT__TYPES(_)\
	_(application)\
	_(point)\
	_(extent)\
	_(rectangle)\
	_##_AS(::xraft::t_object, object)\
	_(font)\
	_##_AS(::xraft::t_color, color)\
	_(graphics)\
	_##_AS(t_graphics::t_function, graphics__function)\
	_(input_attribute)\
	_(input_context)\
	_(drawable)\
	_(bitmap)\
	_(pixmap)\
	_(region)\
	_(key)\
	_(modifier)\
	_(button)\
	_(timer)\
	_(cross_mode)\
	_(cross_detail)\
	_(window)\
	_(widget)\
	_(shell)\
	_(frame)\
	_(opengl_format)\
	_(opengl_widget)\
	_(opengl_context)
	XEMMAIX__XRAFT__TYPES(XEMMAI__TYPE__DECLARE)
	xemmai::t_object* v_application;

	static void f_main(t_library* a_library, const t_pvalue& a_arguments, const t_pvalue& a_callable);
	static t_pvalue f_application(t_library* a_library)
	{
		return a_library->v_application;
	}

public:
	using xemmai::t_library::t_library;
	XEMMAI__LIBRARY__MEMBERS
};

XEMMAI__LIBRARY__BASE(t_library, t_global, f_global())
#define XEMMAI__TYPE__LIBRARY t_library
XEMMAIX__XRAFT__TYPES(XEMMAI__TYPE__DEFINE)
#undef XEMMAI__TYPE__LIBRARY

template<typename T, typename T_base = t_type>
struct t_bears_pointer : t_bears<T, T_base>
{
	using t_base = t_bears_pointer;

	static constexpr size_t c_NATIVE = sizeof(T*);

	using t_bears<T, T_base>::t_bears;
};

}

#endif
