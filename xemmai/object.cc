#include "object.h"

namespace xemmai
{

void t_type_of<xraft::t_object>::f_define(t_library* a_library)
{
	t_define{a_library}
	(L"acquire"sv, t_member<void (*)(xraft::t_object&), f_acquire>())
	(L"release"sv, t_member<void (*)(xraft::t_object&), f_release>())
	.f_derive<xraft::t_object, xemmai::t_object>();
}

void t_type_of<xraft::t_object>::f_do_finalize(xemmai::t_object* a_this)
{
	assert(a_this->f_as<xraft::t_object*>() == nullptr);
}

t_pvalue t_type_of<xraft::t_object>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	f_throw(L"uninstantiatable."sv);
}

void t_type_of<xraft::t_object>::f_do_instantiate(t_pvalue* a_stack, size_t a_n)
{
	auto object = f_construct(a_stack, a_n);
	try {
		static size_t index;
		object.f_call(f_global()->f_symbol_initialize(), index, a_stack, a_n);
	} catch (...) {
		static_cast<xemmaix::xraft::t_proxy*>(f_as<xraft::t_object*>(object)->f_user())->f_release();
		throw;
	}
	a_stack[0] = object;
}

}
