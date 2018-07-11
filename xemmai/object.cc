#include "object.h"

namespace xemmai
{

void t_type_of<xraft::t_object>::f_define(t_extension* a_extension)
{
	using xemmaix::xraft::t_with_application_thread;
	t_define<xraft::t_object, xemmai::t_object>(a_extension, L"Object")
		(L"acquire", t_member<void (*)(xraft::t_object&), f_acquire, t_with_application_thread>())
		(L"release", t_member<void (*)(xraft::t_object&), f_release, t_with_application_thread>())
	;
}

void t_type_of<xraft::t_object>::f_finalize(xemmai::t_object* a_this)
{
	assert(a_this->f_pointer() == nullptr);
}

t_scoped t_type_of<xraft::t_object>::f_construct(t_stacked* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

void t_type_of<xraft::t_object>::f_instantiate(t_stacked* a_stack, size_t a_n)
{
	t_scoped object;
	try {
		object = f_construct(a_stack, a_n);
	} catch (...) {
		t_destruct_n(a_stack, a_n);
		throw;
	}
	try {
		object.f_call(f_global()->f_symbol_initialize(), a_stack, a_n);
	} catch (...) {
		static_cast<xemmaix::xraft::t_proxy*>(f_as<xraft::t_object*>(object)->f_user())->f_release();
		throw;
	}
	a_stack[0] = std::move(object);
}

}
