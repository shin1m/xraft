#include "graphics.h"
#include "pixmap.h"
#include "region.h"

namespace xemmai
{

void t_type_of<xraft::t_font>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
		(L"ascent"sv, t_member<unsigned(t_font::*)() const, &t_font::f_ascent>())
		(L"descent"sv, t_member<unsigned(t_font::*)() const, &t_font::f_descent>())
		(L"height"sv, t_member<unsigned(t_font::*)() const, &t_font::f_height>())
		(L"width"sv,
			t_member<unsigned(t_font::*)() const, &t_font::f_width>(),
			t_member<unsigned(*)(t_font&, size_t), f_width>(),
			t_member<unsigned(*)(t_font&, std::wstring_view), f_width>(),
			t_member<unsigned(*)(t_font&, std::wstring_view, unsigned), f_width>()
		)
	.f_derive<t_font, xraft::t_object>();
}

t_pvalue t_type_of<xraft::t_font>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_construct_with<t_pvalue(*)(t_type*, std::wstring_view), f_construct>::t_bind<xraft::t_font>::f_do(this, a_stack, a_n);
}

void t_type_of<xraft::t_color>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
		(L"red"sv, t_member<size_t(*)(xraft::t_color&), f_red>())
		(L"green"sv, t_member<size_t(*)(xraft::t_color&), f_green>())
		(L"blue"sv, t_member<size_t(*)(xraft::t_color&), f_blue>())
		(L"pixel"sv, t_member<t_pixel(xraft::t_color::*)() const, &xraft::t_color::f_pixel>())
	.f_derive<xraft::t_color, xemmai::t_object>();
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
	static size_t index;
	auto m = a_points0.f_invoke(f_global()->f_symbol_size(), index);
	f_check<size_t>(m, L"size");
	auto n = f_as<size_t>(m);
	a_points1.resize(n);
	for (size_t i = 0; i < n; ++i) {
		auto x = a_points0.f_get_at(f_global()->f_as(i));
		f_check<xraft::t_point>(x, L"path");
		a_points1[i] = f_as<const xraft::t_point&>(x);
	}
}

void t_type_of<xraft::t_graphics>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
		(L"region"sv, t_member<const t_region&(t_graphics::*)() const, &t_graphics::f_region>())
		(L"clip"sv, t_member<void(t_graphics::*)(t_region&), &t_graphics::f_clip>())
		(L"font"sv, t_member<void(*)(t_graphics&, const xraft::t_pointer<t_font>&), f_font>())
		(L"color"sv, t_member<void(t_graphics::*)(t_pixel), &t_graphics::f_color>())
		(L"function"sv, t_member<void(t_graphics::*)(t_graphics::t_function), &t_graphics::f_function>())
		(L"translate"sv, t_member<void(t_graphics::*)(int, int), &t_graphics::f_translate>())
		(L"bounds"sv, t_member<t_rectangle(t_graphics::*)() const, &t_graphics::f_bounds>())
		(L"invalid"sv, t_member<bool(t_graphics::*)(int, int, unsigned, unsigned) const, &t_graphics::f_invalid>())
		(L"draw"sv,
			t_member<void(t_graphics::*)(int, int, unsigned, unsigned), &t_graphics::f_draw>(),
			t_member<void(*)(t_graphics&, const t_pvalue&), f_draw>(),
			t_member<void(*)(t_graphics&, int, int, std::wstring_view), f_draw>(),
			t_member<void(*)(t_graphics&, int, int, const xraft::t_pointer<t_pixmap>&, int, int, unsigned, unsigned), f_draw>(),
			t_member<void(*)(t_graphics&, int, int, const xraft::t_pointer<t_pixmap>&, int, int, unsigned, unsigned, const xraft::t_pointer<t_bitmap>&), f_draw>()
		)
		(L"fill"sv,
			t_member<void(t_graphics::*)(int, int, unsigned, unsigned), &t_graphics::f_fill>(),
			t_member<void(*)(t_graphics&, const t_pvalue&), f_fill>()
		)
	.f_derive<t_graphics, xemmai::t_object>();
}

t_object* t_type_of<xraft::t_graphics::t_function>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}.f_derive<t_graphics::t_function, intptr_t>();
	return a_library->f_type<t_graphics::t_function>()->f_do_derive({{}, t_define(a_library)
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
	});
}

}
