#ifndef XEMMAIX__XRAFT__APPLICATION_H
#define XEMMAIX__XRAFT__APPLICATION_H

#include "window.h"
#include <xemmai/thread.h>

namespace xemmaix::xraft
{

using namespace xemmai;

class t_application : public ::xraft::t_application, public t_entry
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
		t_post(t_scoped&& a_callable) : v_callable(std::move(a_callable))
		{
		}
		virtual void operator()();
	};

	t_application(t_extension* a_extension, xemmai::t_object* a_arguments);
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

namespace xemmai
{

template<>
struct t_type_of<xraft::t_application> : t_uninstantiatable<t_underivable<t_bears<xraft::t_application>>>
{
	template<typename T0>
	struct t_as
	{
		template<typename T1>
		static T0 f_call(T1&& a_object)
		{
			return *t_as<typename t_fundamental<T0>::t_type*>::f_call(std::forward<T1>(a_object));
		}
	};
	template<typename T0>
	struct t_as<T0*>
	{
		template<typename T1>
		static T0* f_call(T1&& a_object)
		{
			auto p = static_cast<T0*>(f_object(std::forward<T1>(a_object))->f_pointer());
			if (!p) f_throw(L"already destroyed."sv);
			return p;
		}
	};
	typedef xemmaix::xraft::t_extension t_extension;

	static void f_post(xraft::t_application& a_self, t_scoped&& a_callable)
	{
		t_thread::f_cache_release();
		a_self.f_post(new xemmaix::xraft::t_application::t_post(std::move(a_callable)));
	}
	static void f_add(xraft::t_application& a_self, const xraft::t_pointer<xraft::t_shell>& a_shell)
	{
		if (!a_shell) f_throw(L"shell must not be null."sv);
		a_self.f_add(a_shell);
	}
	static void f_add(xraft::t_application& a_self, const xraft::t_pointer<xraft::t_shell>& a_shell, size_t a_i)
	{
		if (!a_shell) f_throw(L"shell must not be null."sv);
		a_self.f_add(a_shell, a_i);
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
};

}

#endif
