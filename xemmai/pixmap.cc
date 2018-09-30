#include "pixmap.h"

namespace xemmai
{

void t_type_of<xraft::t_bitmap>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	using xemmaix::xraft::t_with_application_thread;
	t_define<t_bitmap, t_drawable>(a_extension, L"Bitmap"sv)
		(L"width"sv, t_member<unsigned(t_bitmap::*)() const, &t_bitmap::f_width, t_with_application_thread>())
		(L"height"sv, t_member<unsigned(t_bitmap::*)() const, &t_bitmap::f_height, t_with_application_thread>())
	;
}

t_scoped t_type_of<xraft::t_bitmap>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_overload<
		t_construct_with<t_scoped(*)(t_type*, unsigned, unsigned), f_construct>,
		t_construct_with<t_scoped(*)(t_type*, unsigned, unsigned, const t_bytes&), f_construct>,
		t_construct_with<t_scoped(*)(t_type*, unsigned, unsigned, const t_bytes&, int), f_construct>
	>::t_bind<xraft::t_bitmap>::f_do(this, a_stack, a_n);
}

void t_type_of<xraft::t_pixmap>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	using xemmaix::xraft::t_with_application_thread;
	t_define<t_pixmap, t_drawable>(a_extension, L"Pixmap"sv)
		(L"width"sv, t_member<unsigned(t_pixmap::*)() const, &t_pixmap::f_width, t_with_application_thread>())
		(L"height"sv, t_member<unsigned(t_pixmap::*)() const, &t_pixmap::f_height, t_with_application_thread>())
		(L"alpha"sv, t_member<bool(t_pixmap::*)() const, &t_pixmap::f_alpha, t_with_application_thread>())
	;
}

t_scoped t_type_of<xraft::t_pixmap>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_overload<
		t_construct_with<t_scoped(*)(t_type*, unsigned, unsigned), f_construct>,
		t_construct_with<t_scoped(*)(t_type*, unsigned, unsigned, bool), f_construct>,
		t_construct_with<t_scoped(*)(t_type*, unsigned, unsigned, bool, const t_bytes&), f_construct>,
		t_construct_with<t_scoped(*)(t_type*, unsigned, unsigned, bool, const t_bytes&, int), f_construct>
	>::t_bind<xraft::t_pixmap>::f_do(this, a_stack, a_n);
}

}
