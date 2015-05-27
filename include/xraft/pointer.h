#ifndef XRAFT__POINTER_H
#define XRAFT__POINTER_H

namespace xraft
{

template<typename T>
class t_pointer
{
	T* v_p = nullptr;

public:
	t_pointer() = default;
	t_pointer(T* a_p) : v_p(a_p)
	{
		if (v_p) v_p->f_acquire();
	}
	t_pointer(const t_pointer& a_p) : v_p(a_p)
	{
		if (v_p) v_p->f_acquire();
	}
	template<typename U>
	t_pointer(const t_pointer<U>& a_p) : v_p(static_cast<U*>(a_p))
	{
		if (v_p) v_p->f_acquire();
	}
	~t_pointer()
	{
		if (v_p) v_p->f_release();
	}
	operator T*() const
	{
		return v_p;
	}
	T& operator*() const
	{
		return *v_p;
	}
	T* operator->() const
	{
		return v_p;
	}
	T* operator=(T* a_p)
	{
		if (a_p) a_p->f_acquire();
		if (v_p) v_p->f_release();
		return v_p = a_p;
	}
	T* operator=(const t_pointer& a_p)
	{
		return operator=(static_cast<T*>(a_p));
	}
	template<typename U>
	T* operator=(const t_pointer<U>& a_p)
	{
		return operator=(static_cast<U*>(a_p));
	}
	template<typename U>
	bool operator==(U* a_p) const
	{
		return v_p == a_p;
	}
};

template<typename T, typename U>
inline t_pointer<T> f_static_cast(const t_pointer<U>& a_p)
{
	return t_pointer<T>(static_cast<T*>(static_cast<U*>(a_p)));
}

template<typename T, typename U>
inline t_pointer<T> f_dynamic_cast(const t_pointer<U>& a_p)
{
	return t_pointer<T>(dynamic_cast<T*>(static_cast<U*>(a_p)));
}

}

#endif
