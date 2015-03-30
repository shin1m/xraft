#include "graphics.h"
#include "pixmap.h"
#include "region.h"

namespace xemmai
{

using xraft::xemmai::t_with_application_thread;

void t_type_of<t_font>::f_define(t_extension* a_extension)
{
	t_define<t_font, xraft::t_object>(a_extension, L"Font")
		(L"ascent", t_member<unsigned (t_font::*)() const, &t_font::f_ascent, t_with_application_thread>())
		(L"descent", t_member<unsigned (t_font::*)() const, &t_font::f_descent, t_with_application_thread>())
		(L"height", t_member<unsigned (t_font::*)() const, &t_font::f_height, t_with_application_thread>())
		(L"width",
			t_member<unsigned (t_font::*)() const, &t_font::f_width, t_with_application_thread>(),
			t_member<unsigned (*)(t_font&, size_t), f_width, t_with_application_thread>(),
			t_member<unsigned (*)(t_font&, const std::wstring&), f_width, t_with_application_thread>(),
			t_member<unsigned (*)(t_font&, const std::wstring&, unsigned), f_width, t_with_application_thread>()
		)
	;
}

t_scoped t_type_of<t_font>::f_construct(::xemmai::t_object* a_class, t_scoped* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped (*)(::xemmai::t_object*, const std::wstring&), f_construct>::t_bind<t_font>::f_do(a_class, a_stack, a_n);
}

void t_type_of<t_color>::f_define(t_extension* a_extension)
{
	t_define<t_color, ::xemmai::t_object>(a_extension, L"Color")
		(L"red", t_member<size_t (*)(t_color&), f_red, t_with_application_thread>())
		(L"green", t_member<size_t (*)(t_color&), f_green, t_with_application_thread>())
		(L"blue", t_member<size_t (*)(t_color&), f_blue, t_with_application_thread>())
		(L"pixel", t_member<t_pixel (t_color::*)() const, &t_color::f_pixel, t_with_application_thread>())
	;
}

t_type* t_type_of<t_color>::f_derive(t_object* a_this)
{
	return new t_type_of(v_module, a_this);
}

void t_type_of<t_color>::f_finalize(t_object* a_this)
{
	delete f_as<t_color*>(a_this);
}

t_scoped t_type_of<t_color>::f_construct(::xemmai::t_object* a_class, t_scoped* a_stack, size_t a_n)
{
#ifdef XRAFT_X11
	return t_overload<
		t_construct_with<t_scoped (*)(::xemmai::t_object*, const std::wstring&), f_construct>,
		t_construct_with<t_scoped (*)(::xemmai::t_object*, size_t, size_t, size_t), f_construct>
	>::t_bind<t_color>::f_do(a_class, a_stack, a_n);
#else
	return t_construct_with<t_scoped (*)(::xemmai::t_object*, size_t, size_t, size_t), f_construct>::t_bind<t_color>::f_do(a_class, a_stack, a_n);
#endif
}

void t_type_of<t_graphics>::f_extract(const t_value& a_points0, std::vector<t_point>& a_points1)
{
	t_scoped m = a_points0.f_get(f_global()->f_symbol_size())();
	f_check<size_t>(m, L"size");
	size_t n = f_as<size_t>(m);
	a_points1.resize(n);
	for (size_t i = 0; i < n; ++i) {
		t_scoped x = a_points0.f_get_at(f_global()->f_as(i));
		f_check<t_point>(x, L"path");
		a_points1[i] = f_as<const t_point&>(x);
	}
}

void t_type_of<t_graphics>::f_define(t_extension* a_extension)
{
	t_define<t_graphics, ::xemmai::t_object>(a_extension, L"Graphics")
		(L"region", t_member<const t_region& (t_graphics::*)() const, &t_graphics::f_region, t_with_application_thread>())
		(L"clip", t_member<void (t_graphics::*)(t_region&), &t_graphics::f_clip, t_with_application_thread>())
		(L"font", t_member<void (*)(t_graphics&, const xraft::t_pointer<t_font>&), f_font, t_with_application_thread>())
		(L"color", t_member<void (t_graphics::*)(t_pixel), &t_graphics::f_color, t_with_application_thread>())
		(L"function", t_member<void (t_graphics::*)(t_graphics::t_function), &t_graphics::f_function, t_with_application_thread>())
		(L"translate", t_member<void (t_graphics::*)(int, int), &t_graphics::f_translate, t_with_application_thread>())
		(L"bounds", t_member<t_rectangle (t_graphics::*)() const, &t_graphics::f_bounds, t_with_application_thread>())
		(L"invalid", t_member<bool (t_graphics::*)(int, int, unsigned, unsigned) const, &t_graphics::f_invalid, t_with_application_thread>())
		(L"draw",
			t_member<void (t_graphics::*)(int, int, unsigned, unsigned), &t_graphics::f_draw, t_with_application_thread>(),
			t_member<void (*)(t_graphics&, const t_value&), f_draw, t_with_application_thread>(),
			t_member<void (*)(t_graphics&, int, int, const std::wstring&), f_draw, t_with_application_thread>(),
			t_member<void (*)(t_graphics&, int, int, const xraft::t_pointer<t_pixmap>&, int, int, unsigned, unsigned), f_draw, t_with_application_thread>(),
			t_member<void (*)(t_graphics&, int, int, const xraft::t_pointer<t_pixmap>&, int, int, unsigned, unsigned, const xraft::t_pointer<t_bitmap>&), f_draw, t_with_application_thread>()
		)
		(L"fill",
			t_member<void (t_graphics::*)(int, int, unsigned, unsigned), &t_graphics::f_fill, t_with_application_thread>(),
			t_member<void (*)(t_graphics&, const t_value&), f_fill, t_with_application_thread>()
		)
	;
}

t_type* t_type_of<t_graphics>::f_derive(t_object* a_this)
{
	return nullptr;
}

void t_type_of<t_graphics::t_function>::f_define(t_extension* a_extension)
{
	t_define<t_graphics::t_function, intptr_t>(a_extension, L"Function")
		(L"CLEAR", t_graphics::e_function__CLEAR)
		(L"AND", t_graphics::e_function__AND)
		(L"AND_REVERSE", t_graphics::e_function__AND_REVERSE)
		(L"COPY", t_graphics::e_function__COPY)
		(L"AND_INVERTED", t_graphics::e_function__AND_INVERTED)
		(L"NOOP", t_graphics::e_function__NOOP)
		(L"XOR", t_graphics::e_function__XOR)
		(L"OR", t_graphics::e_function__OR)
		(L"NOR", t_graphics::e_function__NOR)
		(L"EQUIV", t_graphics::e_function__EQUIV)
		(L"INVERT", t_graphics::e_function__INVERT)
		(L"OR_REVERSE", t_graphics::e_function__OR_REVERSE)
		(L"COPY_INVERTED", t_graphics::e_function__COPY_INVERTED)
		(L"OR_INVERTED", t_graphics::e_function__OR_INVERTED)
		(L"NAND", t_graphics::e_function__NAND)
		(L"SET", t_graphics::e_function__SET)
	;
}

}
