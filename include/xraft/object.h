#ifndef XRAFT__OBJECT_H
#define XRAFT__OBJECT_H

#ifdef XRAFT_WIN32
#ifndef XRAFT__EXPORT
#define XRAFT__EXPORT __declspec(dllimport)
#endif
#else
#define XRAFT__EXPORT
#endif

#include <cstdlib>

namespace xraft
{

struct t_user
{
	virtual void f_destroy() = 0;
};

class t_object
{
	size_t v_n = 0;
	t_user* v_user = nullptr;

protected:
	XRAFT__EXPORT virtual ~t_object() = default;

public:
	void f_acquire()
	{
		++v_n;
	}
	void f_release()
	{
		if (--v_n > 0) return;
		if (v_user) v_user->f_destroy();
		delete this;
	}
	t_user* f_user() const
	{
		return v_user;
	}
	void f_user__(t_user* a_user)
	{
		t_user* user = v_user;
		v_user = a_user;
		if (user) user->f_destroy();
	}
};

}

#endif
