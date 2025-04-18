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
	struct t_post : t_runnable
	{
		t_rvalue v_callable;

		t_post(const t_pvalue& a_callable) : v_callable(a_callable)
		{
		}
		virtual void operator()();
	};

protected:
	virtual void f_wait();

public:
	t_application(xemmai::t_object* a_arguments);
	~t_application()
	{
		while (v_next != this) v_next->f_dispose();
	}
	void f_post(const t_pvalue& a_callable)
	{
		::xraft::t_application::f_post(new t_post(a_callable));
	}
};

}

namespace xemmai
{

template<>
struct t_type_of<xraft::t_application> : t_uninstantiatable<xemmaix::xraft::t_bears_pointer<xraft::t_application>>
{
	template<typename T>
	struct t_cast : t_type::t_cast<T>
	{
		static T f_as(auto&& a_object)
		{
			return *t_cast<typename t_fundamental<T>::t_type*>::f_as(std::forward<decltype(a_object)>(a_object));
		}
	};
	template<typename T>
	struct t_cast<T*> : t_type::t_cast<T*>
	{
		static T* f_as(auto&& a_object)
		{
			xraft::f_application();
			auto p = static_cast<t_object*>(a_object)->f_as<T*>();
			if (!p) f_throw(L"already destroyed."sv);
			return p;
		}
	};
	using t_library = xemmaix::xraft::t_library;

	static void f_post(t_library* a_library, const t_pvalue& a_self, const t_pvalue& a_callable)
	{
		if (!a_self) f_throw(L"must be not null."sv);
		std::lock_guard lock(a_library->v_mutex);
		auto p = a_self->f_as<xemmaix::xraft::t_application*>();
		if (!p) f_throw(L"already destroyed."sv);
		p->f_post(a_callable);
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
	static void f_define(t_library* a_library);

	using t_base::t_base;
};

}

#endif
