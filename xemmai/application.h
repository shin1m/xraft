#ifndef XRAFT__XEMMAI__APPLICATION_H
#define XRAFT__XEMMAI__APPLICATION_H

#include "window.h"
#include <xemmai/thread.h>

namespace xraft
{

namespace xemmai
{

using namespace ::xemmai;

class t_application : public xraft::t_application, public t_entry
{
	friend class t_proxy;

	t_extension* v_extension;

protected:
	virtual void f_wait();

public:
	class t_post : public t_runnable
	{
		t_scoped v_callable;

	public:
		t_post(const t_transfer& a_callable) : v_callable(a_callable)
		{
		}
		virtual void operator()();
	};

	t_application(t_extension* a_extension, ::xemmai::t_object* a_arguments);
	~t_application()
	{
		while (v_next != this) v_next->f_dispose();
	}
	t_extension* f_extension() const
	{
		return v_extension;
	}
};

}

}

namespace xemmai
{

using namespace xraft;

template<>
struct t_type_of<t_application> : t_type
{
	template<typename T0, typename T1>
	struct t_as
	{
		typedef T0 t_type;

		static T0 f_call(T1 a_object)
		{
			T0* p = static_cast<T0*>(f_object(a_object)->f_pointer());
			if (!p) t_throwable::f_throw(L"already destroyed.");
			return *p;
		}
	};
	template<typename T0, typename T1>
	struct t_as<T0*, T1>
	{
		typedef T0* t_type;

		static T0* f_call(T1 a_object)
		{
			T0* p = static_cast<T0*>(f_object(a_object)->f_pointer());
			if (!p) t_throwable::f_throw(L"already destroyed.");
			return p;
		}
	};
	template<typename T0, typename T1>
	struct t_as<T0&, T1>
	{
		typedef T0& t_type;

		static T0& f_call(T1 a_object)
		{
			T0* p = static_cast<T0*>(f_object(a_object)->f_pointer());
			if (!p) t_throwable::f_throw(L"already destroyed.");
			return *p;
		}
	};
	typedef xraft::xemmai::t_extension t_extension;

	static void f_post(t_application& a_self, const t_transfer& a_callable)
	{
		t_thread::f_cache_release();
		a_self.f_post(new xraft::xemmai::t_application::t_post(a_callable));
	}
	static void f_add(t_application& a_self, const xraft::t_pointer<t_shell>& a_shell)
	{
		if (!a_shell) t_throwable::f_throw(L"shell must not be null.");
		a_self.f_add(a_shell);
	}
	static void f_add(t_application& a_self, const xraft::t_pointer<t_shell>& a_shell, size_t a_i)
	{
		if (!a_shell) t_throwable::f_throw(L"shell must not be null.");
		a_self.f_add(a_shell, a_i);
	}
	static void f_define(t_extension* a_extension);

	t_type_of(const t_transfer& a_module, const t_transfer& a_super) : t_type(a_module, a_super)
	{
		v_shared = true;
	}
	virtual t_type* f_derive(::xemmai::t_object* a_this);
	virtual t_transfer f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n);
};

}

#endif
