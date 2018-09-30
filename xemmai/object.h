#ifndef XEMMAIX__XRAFT__OBJECT_H
#define XEMMAIX__XRAFT__OBJECT_H

#include "xraft.h"

namespace xemmai
{

template<typename T>
struct t_fundamental<xraft::t_pointer<T>>
{
	typedef T t_type;
};

template<>
struct t_type_of<xraft::t_object> : t_underivable<t_bears<xraft::t_object>>
{
	template<typename T0>
	struct t_cast
	{
		template<typename T1>
		static T0* f_call(T1&& a_object)
		{
			xraft::f_application();
			auto p = static_cast<T0*>(f_object(std::forward<T1>(a_object))->f_pointer());
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
			return reinterpret_cast<size_t>(f_object(std::forward<T1>(a_object))) == t_value::e_tag__NULL ? nullptr : t_cast<T0>::f_call(std::forward<T1>(a_object));
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
			return reinterpret_cast<size_t>(p) >= t_value::e_tag__OBJECT && p->f_type()->template f_derives<typename t_fundamental<T0>::t_type>();
		}
	};
	template<typename T0>
	struct t_is<T0*>
	{
		template<typename T1>
		static bool f_call(T1&& a_object)
		{
			auto p = f_object(std::forward<T1>(a_object));
			switch (reinterpret_cast<size_t>(p)) {
			case t_value::e_tag__NULL:
				return true;
			case t_value::e_tag__BOOLEAN:
			case t_value::e_tag__INTEGER:
			case t_value::e_tag__FLOAT:
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
	typedef xemmaix::xraft::t_extension t_extension;

	template<typename T_extension, typename T>
	static t_scoped f_transfer(T_extension* a_extension, const xraft::t_pointer<T>& a_value)
	{
		return xemmaix::xraft::t_proxy::f_wrap(a_extension->template f_type<typename t_fundamental<T>::t_type>(), static_cast<T*>(a_value));
	}
	template<typename T_extension, typename T>
	static t_scoped f_transfer(T_extension* a_extension, T* a_value)
	{
		return xemmaix::xraft::t_proxy::f_wrap(a_extension->template f_type<typename t_fundamental<T>::t_type>(), a_value);
	}
	static void f_acquire(xraft::t_object& a_self)
	{
		static_cast<xemmaix::xraft::t_proxy*>(a_self.f_user())->f_acquire();
	}
	static void f_release(xraft::t_object& a_self)
	{
		static_cast<xemmaix::xraft::t_proxy*>(a_self.f_user())->f_release();
	}
	static void f_define(t_extension* a_extension);

	using t_base::t_base;
	static void f_do_finalize(xemmai::t_object* a_this);
	t_scoped f_do_construct(t_stacked* a_stack, size_t a_n);
	void f_do_instantiate(t_stacked* a_stack, size_t a_n);
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
