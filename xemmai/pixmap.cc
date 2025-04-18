#include "pixmap.h"

namespace xemmai
{

void t_type_of<xraft::t_bitmap>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
	(L"width"sv, t_member<unsigned(t_bitmap::*)() const, &t_bitmap::f_width>())
	(L"height"sv, t_member<unsigned(t_bitmap::*)() const, &t_bitmap::f_height>())
	.f_derive<t_bitmap, t_drawable>();
}

t_pvalue t_type_of<xraft::t_bitmap>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_overload<
		t_construct_with<t_pvalue(*)(t_type*, unsigned, unsigned), f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, unsigned, unsigned, const t_bytes&), f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, unsigned, unsigned, const t_bytes&, int), f_construct>
	>::t_bind<xraft::t_bitmap>::f_do(this, a_stack, a_n);
}

void t_type_of<xraft::t_pixmap>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
	(L"width"sv, t_member<unsigned(t_pixmap::*)() const, &t_pixmap::f_width>())
	(L"height"sv, t_member<unsigned(t_pixmap::*)() const, &t_pixmap::f_height>())
	(L"alpha"sv, t_member<bool(t_pixmap::*)() const, &t_pixmap::f_alpha>())
	.f_derive<t_pixmap, t_drawable>();
}

t_pvalue t_type_of<xraft::t_pixmap>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_overload<
		t_construct_with<t_pvalue(*)(t_type*, unsigned, unsigned), f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, unsigned, unsigned, bool), f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, unsigned, unsigned, bool, const t_bytes&), f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, unsigned, unsigned, bool, const t_bytes&, int), f_construct>
	>::t_bind<xraft::t_pixmap>::f_do(this, a_stack, a_n);
}

}
