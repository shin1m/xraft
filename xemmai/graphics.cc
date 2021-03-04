#include "graphics.h"
#include "pixmap.h"
#include "region.h"

namespace xemmai
{

void t_type_of<xraft::t_font>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	using xemmaix::xraft::t_with_application_thread;
	t_define<t_font, xraft::t_object>(a_extension, L"Font"sv)
		(L"ascent"sv, t_member<unsigned(t_font::*)() const, &t_font::f_ascent, t_with_application_thread>())
		(L"descent"sv, t_member<unsigned(t_font::*)() const, &t_font::f_descent, t_with_application_thread>())
		(L"height"sv, t_member<unsigned(t_font::*)() const, &t_font::f_height, t_with_application_thread>())
		(L"width"sv,
			t_member<unsigned(t_font::*)() const, &t_font::f_width, t_with_application_thread>(),
			t_member<unsigned(*)(t_font&, size_t), f_width, t_with_application_thread>(),
			t_member<unsigned(*)(t_font&, std::wstring_view), f_width, t_with_application_thread>(),
			t_member<unsigned(*)(t_font&, std::wstring_view, unsigned), f_width, t_with_application_thread>()
		)
	;
}

t_pvalue t_type_of<xraft::t_font>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_pvalue(*)(t_type*, std::wstring_view), f_construct>::t_bind<xraft::t_font>::f_do(this, a_stack, a_n);
}

void t_type_of<xraft::t_color>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	using xemmaix::xraft::t_with_application_thread;
	t_define<xraft::t_color, xemmai::t_object>(a_extension, L"Color"sv)
		(L"red"sv, t_member<size_t(*)(xraft::t_color&), f_red, t_with_application_thread>())
		(L"green"sv, t_member<size_t(*)(xraft::t_color&), f_green, t_with_application_thread>())
		(L"blue"sv, t_member<size_t(*)(xraft::t_color&), f_blue, t_with_application_thread>())
		(L"pixel"sv, t_member<t_pixel(xraft::t_color::*)() const, &xraft::t_color::f_pixel, t_with_application_thread>())
	;
}

t_pvalue t_type_of<xraft::t_color>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
#ifdef XRAFT_X11
	return t_overload<
		t_construct_with<t_pvalue(*)(t_type*, std::wstring_view), f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, size_t, size_t, size_t), f_construct>
	>::t_bind<xraft::t_color>::f_do(this, a_stack, a_n);
#else
	return t_construct_with<t_pvalue(*)(t_type*, size_t, size_t, size_t), f_construct>::t_bind<xraft::t_color>::f_do(this, a_stack, a_n);
#endif
}

void t_type_of<xraft::t_graphics>::f_extract(const t_pvalue& a_points0, std::vector<xraft::t_point>& a_points1)
{
	auto m = a_points0.f_invoke(f_global()->f_symbol_size());
	f_check<size_t>(m, L"size");
	auto n = f_as<size_t>(m);
	a_points1.resize(n);
	for (size_t i = 0; i < n; ++i) {
		auto x = a_points0.f_get_at(f_global()->f_as(i));
		f_check<xraft::t_point>(x, L"path");
		a_points1[i] = f_as<const xraft::t_point&>(x);
	}
}

void t_type_of<xraft::t_graphics>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	using xemmaix::xraft::t_with_application_thread;
	t_define<t_graphics, xemmai::t_object>(a_extension, L"Graphics"sv)
		(L"region"sv, t_member<const t_region&(t_graphics::*)() const, &t_graphics::f_region, t_with_application_thread>())
		(L"clip"sv, t_member<void(t_graphics::*)(t_region&), &t_graphics::f_clip, t_with_application_thread>())
		(L"font"sv, t_member<void(*)(t_graphics&, const xraft::t_pointer<t_font>&), f_font, t_with_application_thread>())
		(L"color"sv, t_member<void(t_graphics::*)(t_pixel), &t_graphics::f_color, t_with_application_thread>())
		(L"function"sv, t_member<void(t_graphics::*)(t_graphics::t_function), &t_graphics::f_function, t_with_application_thread>())
		(L"translate"sv, t_member<void(t_graphics::*)(int, int), &t_graphics::f_translate, t_with_application_thread>())
		(L"bounds"sv, t_member<t_rectangle(t_graphics::*)() const, &t_graphics::f_bounds, t_with_application_thread>())
		(L"invalid"sv, t_member<bool(t_graphics::*)(int, int, unsigned, unsigned) const, &t_graphics::f_invalid, t_with_application_thread>())
		(L"draw"sv,
			t_member<void(t_graphics::*)(int, int, unsigned, unsigned), &t_graphics::f_draw, t_with_application_thread>(),
			t_member<void(*)(t_graphics&, const t_pvalue&), f_draw, t_with_application_thread>(),
			t_member<void(*)(t_graphics&, int, int, std::wstring_view), f_draw, t_with_application_thread>(),
			t_member<void(*)(t_graphics&, int, int, const xraft::t_pointer<t_pixmap>&, int, int, unsigned, unsigned), f_draw, t_with_application_thread>(),
			t_member<void(*)(t_graphics&, int, int, const xraft::t_pointer<t_pixmap>&, int, int, unsigned, unsigned, const xraft::t_pointer<t_bitmap>&), f_draw, t_with_application_thread>()
		)
		(L"fill"sv,
			t_member<void(t_graphics::*)(int, int, unsigned, unsigned), &t_graphics::f_fill, t_with_application_thread>(),
			t_member<void(*)(t_graphics&, const t_pvalue&), f_fill, t_with_application_thread>()
		)
	;
}

void t_type_of<xraft::t_graphics::t_function>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	t_define<t_graphics::t_function, intptr_t>(a_extension, L"Function"sv)
		(L"CLEAR"sv, t_graphics::e_function__CLEAR)
		(L"AND"sv, t_graphics::e_function__AND)
		(L"AND_REVERSE"sv, t_graphics::e_function__AND_REVERSE)
		(L"COPY"sv, t_graphics::e_function__COPY)
		(L"AND_INVERTED"sv, t_graphics::e_function__AND_INVERTED)
		(L"NOOP"sv, t_graphics::e_function__NOOP)
		(L"XOR"sv, t_graphics::e_function__XOR)
		(L"OR"sv, t_graphics::e_function__OR)
		(L"NOR"sv, t_graphics::e_function__NOR)
		(L"EQUIV"sv, t_graphics::e_function__EQUIV)
		(L"INVERT"sv, t_graphics::e_function__INVERT)
		(L"OR_REVERSE"sv, t_graphics::e_function__OR_REVERSE)
		(L"COPY_INVERTED"sv, t_graphics::e_function__COPY_INVERTED)
		(L"OR_INVERTED"sv, t_graphics::e_function__OR_INVERTED)
		(L"NAND"sv, t_graphics::e_function__NAND)
		(L"SET"sv, t_graphics::e_function__SET)
	;
}

}
