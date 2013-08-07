#include "pixmap.h"

namespace xemmai
{

using xraft::xemmai::t_with_application_thread;

void t_type_of<t_bitmap>::f_define(t_extension* a_extension)
{
	t_define<t_bitmap, t_drawable>(a_extension, L"Bitmap")
		(L"width", t_member<unsigned (t_bitmap::*)() const, &t_bitmap::f_width, t_with_application_thread>())
		(L"height", t_member<unsigned (t_bitmap::*)() const, &t_bitmap::f_height, t_with_application_thread>())
	;
}

t_scoped t_type_of<t_bitmap>::f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n)
{
	return t_overload<
		t_construct_with<t_scoped (*)(::xemmai::t_object*, unsigned, unsigned), f_construct>,
		t_construct_with<t_scoped (*)(::xemmai::t_object*, unsigned, unsigned, const t_bytes&), f_construct>,
		t_construct_with<t_scoped (*)(::xemmai::t_object*, unsigned, unsigned, const t_bytes&, int), f_construct>
	>::t_bind<t_bitmap>::f_do(a_class, a_stack, a_n);
}

void t_type_of<t_pixmap>::f_define(t_extension* a_extension)
{
	t_define<t_pixmap, t_drawable>(a_extension, L"Pixmap")
		(L"width", t_member<unsigned (t_pixmap::*)() const, &t_pixmap::f_width, t_with_application_thread>())
		(L"height", t_member<unsigned (t_pixmap::*)() const, &t_pixmap::f_height, t_with_application_thread>())
		(L"alpha", t_member<bool (t_pixmap::*)() const, &t_pixmap::f_alpha, t_with_application_thread>())
	;
}

t_scoped t_type_of<t_pixmap>::f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n)
{
	return t_overload<
		t_construct_with<t_scoped (*)(::xemmai::t_object*, unsigned, unsigned), f_construct>,
		t_construct_with<t_scoped (*)(::xemmai::t_object*, unsigned, unsigned, bool), f_construct>,
		t_construct_with<t_scoped (*)(::xemmai::t_object*, unsigned, unsigned, bool, const t_bytes&), f_construct>,
		t_construct_with<t_scoped (*)(::xemmai::t_object*, unsigned, unsigned, bool, const t_bytes&, int), f_construct>
	>::t_bind<t_pixmap>::f_do(a_class, a_stack, a_n);
}

}
