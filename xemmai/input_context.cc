#include "input_context.h"

namespace xemmai
{

t_object* t_type_of<xraft::t_input_attribute>::f_define(t_library* a_library)
{
	using namespace xraft;
	return t_base::f_define(a_library, [](auto a_fields)
	{
		a_fields
		(L"NONE"sv, e_input_attribute__NONE)
		(L"REVERSE"sv, e_input_attribute__REVERSE)
		(L"UNDERLINE"sv, e_input_attribute__UNDERLINE)
		;
	});
}

t_pvalue t_type_of<xraft::t_input_context>::f_composition(t_library* a_library, xraft::t_input_context& a_self)
{
	std::vector<wchar_t> cs;
	std::vector<xraft::t_input_attribute> as;
	a_self.f_composition(cs, as);
	return f_tuple(t_string::f_instantiate(cs.data(), cs.size()), t_tuple::f_instantiate(as.size(), [&](auto& tuple)
	{
		for (size_t i = 0; i < as.size(); ++i) new(&tuple[i]) t_svalue(a_library->f_as(as[i]));
	}));
}

void t_type_of<xraft::t_input_context>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
		(L"composition"sv, t_member<t_pvalue(*)(t_library*, t_input_context&), f_composition>())
	.f_derive<t_input_context, xraft::t_object>();
}

t_pvalue t_type_of<xraft::t_input_context>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_pvalue(*)(t_type*), xemmaix::xraft::t_wrapper<xraft::t_input_context>::f_construct>::t_bind<xraft::t_input_context>::f_do(this, a_stack, a_n);
}

}
