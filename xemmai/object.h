#ifndef XEMMAIX__XRAFT__OBJECT_H
#define XEMMAIX__XRAFT__OBJECT_H

#include "xraft.h"

namespace xemmai
{

template<typename T>
struct t_fundamental<xraft::t_pointer<T>>
{
	using t_type = T;
};

template<>
struct t_type_of<xraft::t_object> : xemmaix::xraft::t_bears_pointer<xraft::t_object>
{
	template<typename T0>
	struct t_cast
	{
		template<typename T1>
		static T0* f_call(T1&& a_object)
		{
			xraft::f_application();
			auto p = f_object(std::forward<T1>(a_object))->template f_as<T0*>();
			if (!p) f_throw(L"already destroyed."sv);
			return p;
		}
	};
	template<typename T0>
	struct t_as
	{
		template<typename T1>
		static T0 f_call(T1&& a_object)
		{
			return *t_cast<typename t_fundamental<T0>::t_type>::f_call(std::forward<T1>(a_object));
		}
	};
	template<typename T0>
	struct t_as<T0*>
	{
		template<typename T1>
		static T0* f_call(T1&& a_object)
		{
			return f_object(std::forward<T1>(a_object)) ? t_cast<T0>::f_call(std::forward<T1>(a_object)) : nullptr;
		}
	};
	template<typename T0>
	struct t_as<const xraft::t_pointer<T0>&>
	{
		template<typename T1>
		static xraft::t_pointer<T0> f_call(T1&& a_object)
		{
			return t_as<T0*>::f_call(std::forward<T1>(a_object));
		}
	};
	template<typename T0>
	struct t_is
	{
		template<typename T1>
		static bool f_call(T1&& a_object)
		{
			auto p = f_object(std::forward<T1>(a_object));
			return reinterpret_cast<uintptr_t>(p) >= e_tag__OBJECT && p->f_type()->template f_derives<typename t_fundamental<T0>::t_type>();
		}
	};
	template<typename T0>
	struct t_is<T0*>
	{
		template<typename T1>
		static bool f_call(T1&& a_object)
		{
			auto p = f_object(std::forward<T1>(a_object));
			switch (reinterpret_cast<uintptr_t>(p)) {
			case e_tag__NULL:
				return true;
			case e_tag__BOOLEAN:
			case e_tag__INTEGER:
			case e_tag__FLOAT:
				return false;
			default:
				return p->f_type()->template f_derives<typename t_fundamental<T0>::t_type>();
			}
		}
	};
	template<typename T0>
	struct t_is<const xraft::t_pointer<T0>&>
	{
		template<typename T1>
		static bool f_call(T1&& a_object)
		{
			return t_is<T0*>::f_call(std::forward<T1>(a_object));
		}
	};
	using t_library = xemmaix::xraft::t_library;

	template<typename T_library, typename T>
	static t_pvalue f_transfer(T_library* a_library, const xraft::t_pointer<T>& a_value)
	{
		return xemmaix::xraft::t_proxy::f_wrap(a_library->template f_type<typename t_fundamental<T>::t_type>(), static_cast<T*>(a_value));
	}
	template<typename T_library, typename T>
	static t_pvalue f_transfer(T_library* a_library, T* a_value)
	{
		return xemmaix::xraft::t_proxy::f_wrap(a_library->template f_type<typename t_fundamental<T>::t_type>(), a_value);
	}
	static void f_acquire(xraft::t_object& a_self)
	{
		static_cast<xemmaix::xraft::t_proxy*>(a_self.f_user())->f_acquire();
	}
	static void f_release(xraft::t_object& a_self)
	{
		static_cast<xemmaix::xraft::t_proxy*>(a_self.f_user())->f_release();
	}
	static void f_define(t_library* a_library);

	using t_base::t_base;
	static void f_do_finalize(xemmai::t_object* a_this);
	t_pvalue f_do_construct(t_pvalue* a_stack, size_t a_n);
	void f_do_instantiate(t_pvalue* a_stack, size_t a_n);
};

}

namespace xemmaix::xraft
{

inline void t_proxy::f_acquire()
{
	if (v_n <= 0) f_as<::xraft::t_object*>(v_object)->f_acquire();
	++v_n;
}

inline void t_proxy::f_release()
{
	if (v_n > 0 && --v_n <= 0) f_as<::xraft::t_object*>(v_object)->f_release();
}

}

#endif
