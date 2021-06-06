#include "geometry.h"

namespace xemmai
{

void t_type_of<xraft::t_point>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
		(L"x"sv, t_member<int(*)(t_point&), f_x>())
		(L"x__"sv, t_member<void(*)(t_point&, int), f_x__>())
		(L"y"sv, t_member<int(*)(t_point&), f_y>())
		(L"y__"sv, t_member<void(*)(t_point&, int), f_y__>())
	.f_derive<t_point, xemmai::t_object>();
}

t_pvalue t_type_of<xraft::t_point>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct<int, int>::t_bind<xraft::t_point>::f_do(this, a_stack, a_n);
}

void t_type_of<xraft::t_extent>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
		(L"width"sv, t_member<unsigned(*)(t_extent&), f_width>())
		(L"width__"sv, t_member<void(*)(t_extent&, unsigned), f_width__>())
		(L"height"sv, t_member<unsigned(*)(t_extent&), f_height>())
		(L"height__"sv, t_member<void(*)(t_extent&, unsigned), f_height__>())
	.f_derive<t_extent, xemmai::t_object>();
}

t_pvalue t_type_of<xraft::t_extent>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct<unsigned, unsigned>::t_bind<xraft::t_extent>::f_do(this, a_stack, a_n);
}

void t_type_of<xraft::t_rectangle>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
		(L"x"sv, t_member<int(*)(t_rectangle&), f_x>())
		(L"x__"sv, t_member<void(*)(t_rectangle&, int), f_x__>())
		(L"y"sv, t_member<int(*)(t_rectangle&), f_y>())
		(L"y__"sv, t_member<void(*)(t_rectangle&, int), f_y__>())
		(L"width"sv, t_member<unsigned(*)(t_rectangle&), f_width>())
		(L"width__"sv, t_member<void(*)(t_rectangle&, unsigned), f_width__>())
		(L"height"sv, t_member<unsigned(*)(t_rectangle&), f_height>())
		(L"height__"sv, t_member<void(*)(t_rectangle&, unsigned), f_height__>())
	.f_derive<t_rectangle, xemmai::t_object>();
}

t_pvalue t_type_of<xraft::t_rectangle>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_overload<
		t_construct<const xraft::t_point&, const xraft::t_extent&>,
		t_construct<int, int, unsigned, unsigned>
	>::t_bind<xraft::t_rectangle>::f_do(this, a_stack, a_n);
}

}
