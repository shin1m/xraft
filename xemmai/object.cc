#include "object.h"

#include <cassert>

namespace xemmai
{

using xraft::xemmai::t_with_application_thread;

void t_type_of<xraft::t_object>::f_define(t_extension* a_extension)
{
	t_define<xraft::t_object, ::xemmai::t_object>(a_extension, L"Object")
		(L"acquire", t_member<void (*)(xraft::t_object&), f_acquire, t_with_application_thread>())
		(L"release", t_member<void (*)(xraft::t_object&), f_release, t_with_application_thread>())
	;
}

t_type* t_type_of<xraft::t_object>::f_derive(::xemmai::t_object* a_this)
{
	return nullptr;
}

void t_type_of<xraft::t_object>::f_finalize(::xemmai::t_object* a_this)
{
//	assert(!f_as<xraft::t_object*>(a_this));
	assert(!a_this->f_pointer());
}

t_scoped t_type_of<xraft::t_object>::f_construct(::xemmai::t_object* a_class, t_scoped* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

void t_type_of<xraft::t_object>::f_instantiate(::xemmai::t_object* a_class, t_scoped* a_stack, size_t a_n)
{
	t_scoped object = f_as<t_type&>(a_class).f_construct(a_class, a_stack, a_n);
	try {
		object.f_get(f_global()->f_symbol_initialize()).f_call(a_stack, a_n);
	} catch (...) {
		static_cast<xraft::xemmai::t_proxy*>(f_as<xraft::t_object*>(object)->f_user())->f_release();
		throw;
	}
	a_stack[0] = std::move(object);
}

}
