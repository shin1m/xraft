#include "input_context.h"

#include <xemmai/array.h>

namespace xemmai
{

using xraft::xemmai::t_with_application_thread;

void t_type_of<t_input_attribute>::f_define(t_extension* a_extension)
{
	t_define<t_input_attribute, intptr_t>(a_extension, L"InputAttribute")
		(L"NONE", e_input_attribute__NONE)
		(L"REVERSE", e_input_attribute__REVERSE)
		(L"UNDERLINE", e_input_attribute__UNDERLINE)
	;
}

t_scoped t_type_of<t_input_context>::f_composition(t_extension* a_extension, t_input_context& a_self)
{
	std::vector<wchar_t> cs;
	std::vector<t_input_attribute> as0;
	a_self.f_composition(cs, as0);
	t_scoped as1 = t_tuple::f_instantiate(as0.size());
	for (size_t i = 0; i < as0.size(); ++i) f_as<t_tuple&>(as1)[i] = a_extension->f_as(as0[i]);
	t_scoped tuple = t_tuple::f_instantiate(2);
	f_as<t_tuple&>(tuple)[0] = a_extension->f_as(std::wstring(cs.begin(), cs.end()));
	f_as<t_tuple&>(tuple)[1] = std::move(as1);
	return tuple;
}

void t_type_of<t_input_context>::f_define(t_extension* a_extension)
{
	t_define<t_input_context, xraft::t_object>(a_extension, L"InputContext")
		(L"composition", t_member<t_scoped (*)(t_extension*, t_input_context&), f_composition, t_with_application_thread>())
	;
}

t_scoped t_type_of<t_input_context>::f_construct(::xemmai::t_object* a_class, t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped (*)(::xemmai::t_object*), xraft::xemmai::t_wrapper<t_input_context>::f_construct>::t_bind<t_input_context>::f_do(a_class, a_stack, a_n);
}

}
