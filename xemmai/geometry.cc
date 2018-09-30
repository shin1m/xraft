#include "geometry.h"

namespace xemmai
{

void t_type_of<xraft::t_point>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	using xemmaix::xraft::t_with_application_thread;
	t_define<t_point, xemmai::t_object>(a_extension, L"Point"sv)
		(L"x"sv, t_member<int(*)(t_point&), f_x, t_with_application_thread>())
		(L"x__"sv, t_member<void(*)(t_point&, int), f_x__, t_with_application_thread>())
		(L"y"sv, t_member<int(*)(t_point&), f_y, t_with_application_thread>())
		(L"y__"sv, t_member<void(*)(t_point&, int), f_y__, t_with_application_thread>())
	;
}

t_scoped t_type_of<xraft::t_point>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct<false, int, int>::t_bind<xraft::t_point>::f_do(this, a_stack, a_n);
}

void t_type_of<xraft::t_extent>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	using xemmaix::xraft::t_with_application_thread;
	t_define<t_extent, xemmai::t_object>(a_extension, L"Extent"sv)
		(L"width"sv, t_member<unsigned(*)(t_extent&), f_width, t_with_application_thread>())
		(L"width__"sv, t_member<void(*)(t_extent&, unsigned), f_width__, t_with_application_thread>())
		(L"height"sv, t_member<unsigned(*)(t_extent&), f_height, t_with_application_thread>())
		(L"height__"sv, t_member<void(*)(t_extent&, unsigned), f_height__, t_with_application_thread>())
	;
}

t_scoped t_type_of<xraft::t_extent>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct<false, unsigned, unsigned>::t_bind<xraft::t_extent>::f_do(this, a_stack, a_n);
}

void t_type_of<xraft::t_rectangle>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	using xemmaix::xraft::t_with_application_thread;
	t_define<t_rectangle, xemmai::t_object>(a_extension, L"Rectangle"sv)
		(L"x"sv, t_member<int(*)(t_rectangle&), f_x, t_with_application_thread>())
		(L"x__"sv, t_member<void(*)(t_rectangle&, int), f_x__, t_with_application_thread>())
		(L"y"sv, t_member<int(*)(t_rectangle&), f_y, t_with_application_thread>())
		(L"y__"sv, t_member<void(*)(t_rectangle&, int), f_y__, t_with_application_thread>())
		(L"width"sv, t_member<unsigned(*)(t_rectangle&), f_width, t_with_application_thread>())
		(L"width__"sv, t_member<void(*)(t_rectangle&, unsigned), f_width__, t_with_application_thread>())
		(L"height"sv, t_member<unsigned(*)(t_rectangle&), f_height, t_with_application_thread>())
		(L"height__"sv, t_member<void(*)(t_rectangle&, unsigned), f_height__, t_with_application_thread>())
	;
}

t_scoped t_type_of<xraft::t_rectangle>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_overload<
		t_construct<false, const xraft::t_point&, const xraft::t_extent&>,
		t_construct<false, int, int, unsigned, unsigned>
	>::t_bind<xraft::t_rectangle>::f_do(this, a_stack, a_n);
}

}
