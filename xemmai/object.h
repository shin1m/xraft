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
struct t_type_of<xraft::t_object> : t_type
{
	template<typename T0, typename T1>
	struct t_cast
	{
		static T0* f_call(T1 a_object)
		{
			xraft::f_application();
			T0* p = static_cast<T0*>(f_object(a_object)->f_pointer());
			if (!p) t_throwable::f_throw(L"already destroyed.");
			return p;
		}
	};
	template<typename T0, typename T1>
	struct t_as
	{
		typedef T0 t_type;

		static T0 f_call(T1 a_object)
		{
			return *t_cast<T0, T1>::f_call(a_object);
		}
	};
	template<typename T0, typename T1>
	struct t_as<T0*, T1>
	{
		typedef T0* t_type;

		static T0* f_call(T1 a_object)
		{
			return reinterpret_cast<size_t>(f_object(a_object)) == t_value::e_tag__NULL ? nullptr : t_cast<T0, T1>::f_call(a_object);
		}
	};
	template<typename T0, typename T1>
	struct t_as<T0&, T1>
	{
		typedef T0& t_type;

		static T0& f_call(T1 a_object)
		{
			return *t_cast<T0, T1>::f_call(a_object);
		}
	};
	template<typename T0, typename T1>
	struct t_as<const xraft::t_pointer<T0>&, T1>
	{
		typedef xraft::t_pointer<T0> t_type;

		static T0* f_call(T1 a_object)
		{
			return reinterpret_cast<size_t>(f_object(a_object)) == t_value::e_tag__NULL ? nullptr : t_cast<T0, T1>::f_call(a_object);
		}
	};
	template<typename T0, typename T1>
	struct t_is
	{
		static bool f_call(T1 a_object)
		{
			return reinterpret_cast<size_t>(f_object(a_object)) >= t_value::e_tag__OBJECT && dynamic_cast<t_type_of<typename t_fundamental<T0>::t_type>*>(f_as<t_type*>(f_object(a_object)->f_type())) != nullptr;
		}
	};
	template<typename T0, typename T1>
	struct t_is<T0*, T1>
	{
		static bool f_call(T1 a_object)
		{
			switch (reinterpret_cast<size_t>(f_object(a_object))) {
			case t_value::e_tag__NULL:
				return true;
			case t_value::e_tag__BOOLEAN:
			case t_value::e_tag__INTEGER:
			case t_value::e_tag__FLOAT:
				return false;
			default:
				return dynamic_cast<t_type_of<typename t_fundamental<T0>::t_type>*>(f_as<t_type*>(f_object(a_object)->f_type())) != nullptr;
			}
		}
	};
	template<typename T0, typename T1>
	struct t_is<const xraft::t_pointer<T0>&, T1>
	{
		static bool f_call(T1 a_object)
		{
			switch (reinterpret_cast<size_t>(f_object(a_object))) {
			case t_value::e_tag__NULL:
				return true;
			case t_value::e_tag__BOOLEAN:
			case t_value::e_tag__INTEGER:
			case t_value::e_tag__FLOAT:
				return false;
			default:
				return dynamic_cast<t_type_of<typename t_fundamental<T0>::t_type>*>(f_as<t_type*>(f_object(a_object)->f_type())) != nullptr;
			}
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

	using t_type::t_type;
	XRAFT__XEMMAI__EXPORT virtual t_type* f_derive(xemmai::t_object* a_this);
	XRAFT__XEMMAI__EXPORT virtual void f_finalize(xemmai::t_object* a_this);
	XRAFT__XEMMAI__EXPORT virtual t_scoped f_construct(xemmai::t_object* a_class, t_stacked* a_stack, size_t a_n);
	XRAFT__XEMMAI__EXPORT virtual void f_instantiate(xemmai::t_object* a_class, t_stacked* a_stack, size_t a_n);
};

}

namespace xemmaix
{

namespace xraft
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

}

#endif
