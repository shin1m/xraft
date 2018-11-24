#include "xrafttv.h"

#include "graphics.h"

namespace xemmaix::xrafttv
{

void t_text_model::f_loaded()
{
	v_loaded();
}

void t_text_model::f_replacing(size_t a_p, size_t a_n)
{
	v_replacing(t_value(a_p), t_value(a_n));
}

void t_text_model::f_replaced(size_t a_n)
{
	v_replaced(t_value(a_n));
}

t_scoped t_text_model::f_slice(size_t a_p, size_t a_n) const
{
	size_t n = f_text_size();
	if (a_p > n) f_throw(L"out of range."sv);
	if (a_n > n) a_n = n;
	if (a_p + a_n > n) a_n = n - a_p;
	auto extension = f_extension<t_extension>(f_self(this)->f_type()->v_module);
	auto segments = t_array::f_instantiate();
	if (a_n > 0) {
		size_t text = a_p;
		size_t i = t_base::f_text_to_segment(text);
		while (true) {
			size_t next = std::min(++i < f_segments_size() ? t_base::f_segment_to_text(i) : n, a_p + a_n);
			f_as<t_array&>(segments).f_push(f_tuple(t_value(next - text), extension->f_as(t_base::f_segment_to_attribute(i))));
			if (next >= a_p + a_n) break;
			text = next;
		}
	}
	std::wstring text(a_n, L'\0');
	std::copy_n(f_begin() + a_p, a_n, text.begin());
	return f_tuple(segments, extension->f_as(text));
}

void t_text_model::f_replace(size_t a_p, size_t a_n, t_scoped&& a_segments, std::wstring_view a_text)
{
	{
		size_t n = f_text_size();
		if (a_p > n || a_n > n || a_p + a_n > n) f_throw(L"out of range."sv);
	}
	auto extension = f_extension<t_extension>(f_self(this)->f_type()->v_module);
	t_scoped p = a_segments.f_invoke(extension->v_symbol_size);
	f_check<size_t>(p, L"size");
	size_t n = f_as<size_t>(p);
	std::vector<t_segment> ss(n);
	size_t m = 0;
	for (size_t i = 0; i < n; ++i) {
		t_scoped p = a_segments.f_get_at(t_value(i));
		t_scoped text = p.f_get_at(t_value(0));
		f_check<size_t>(text, L"text");
		ss[i].v_text = f_as<size_t>(text);
		if (ss[i].v_text <= 0) f_throw(L"empty segment."sv);
		m += ss[i].v_text;
		t_scoped attribute = p.f_get_at(t_value(1));
		f_check<t_attribute*>(attribute, L"attribute");
		ss[i].v_attribute = f_as<t_attribute*>(attribute);
	}
	if (a_text.size() != m) f_throw(L"invalid text size."sv);
	t_base::f_replace(a_p, a_n, ss.begin(), ss.end(), a_text.begin());
}

void t_wrapped_view::f_invalidated(size_t a_y, size_t a_height)
{
	v_invalidated(t_value(a_y), t_value(a_height));
}

void t_wrapped_view::f_resized()
{
	v_resized();
}

void t_wrapped_view::f_moved(size_t a_y, size_t a_height, size_t a_delta)
{
	v_moved(t_value(a_y), t_value(a_height), t_value(a_delta));
}

t_extension::t_extension(xemmai::t_object* a_module, t_scoped&& a_xraft) : xemmai::t_extension(a_module), v_module_xraft(std::move(a_xraft))
{
	v_xraft = xemmai::f_extension<xemmaix::xraft::t_extension>(v_module_xraft);
	v_symbol_size = t_symbol::f_instantiate(L"size"sv);
	t_type_of<t_attribute>::f_define(this);
	t_type_of<t_text_model>::f_define(this);
	t_type_of<t_wrapped_view>::f_define(this);
}

void t_extension::f_scan(t_scan a_scan)
{
	a_scan(v_module_xraft);
	a_scan(v_symbol_size);
	a_scan(v_type_attribute);
	a_scan(v_type_text_model);
	a_scan(v_type_wrapped_view);
}

}

namespace xemmai
{

void t_type_of<xemmaix::xrafttv::t_attribute>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	using namespace xemmaix::xraft;
	using namespace xemmaix::xrafttv;
	t_define<t_attribute, xraft::t_object>(a_extension, L"Attribute"sv)
		(L"foreground"sv, t_member<t_pixel(t_attribute::*)() const, &t_attribute::f_foreground, t_with_application_thread>())
		(L"background"sv, t_member<t_pixel(t_attribute::*)() const, &t_attribute::f_background, t_with_application_thread>())
	;
}

t_scoped t_type_of<xemmaix::xrafttv::t_attribute>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_type*, xraft::t_pixel, xraft::t_pixel), f_construct>::t_bind<xemmaix::xrafttv::t_attribute>::f_do(this, a_stack, a_n);
}

void t_type_of<xemmaix::xrafttv::t_text_model>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	using namespace xemmaix::xraft;
	using namespace xemmaix::xrafttv;
	using xemmaix::xrafttv::t_text_model;
	t_define<t_text_model, xraft::t_object>(a_extension, L"TextModel"sv)
		(L"add_loaded"sv, t_member<void(t_text_model::*)(t_scoped&&), &t_text_model::f_add_loaded, t_with_application_thread>())
		(L"remove_loaded"sv, t_member<void(t_text_model::*)(t_scoped&&), &t_text_model::f_remove_loaded, t_with_application_thread>())
		(L"add_replacing"sv, t_member<void(t_text_model::*)(t_scoped&&), &t_text_model::f_add_replacing, t_with_application_thread>())
		(L"remove_replacing"sv, t_member<void(t_text_model::*)(t_scoped&&), &t_text_model::f_remove_replacing, t_with_application_thread>())
		(L"add_replaced"sv, t_member<void(t_text_model::*)(t_scoped&&), &t_text_model::f_add_replaced, t_with_application_thread>())
		(L"remove_replaced"sv, t_member<void(t_text_model::*)(t_scoped&&), &t_text_model::f_remove_replaced, t_with_application_thread>())
		(L"enable_notification"sv, t_member<void(t_text_model::*)(), &t_text_model::f_enable_notification, t_with_application_thread>())
		(L"disable_notification"sv, t_member<void(t_text_model::*)(), &t_text_model::f_disable_notification, t_with_application_thread>())
		(L"text_size"sv, t_member<size_t(t_text_model::*)() const, &t_text_model::f_text_size, t_with_application_thread>())
		(L"segments_size"sv, t_member<size_t(t_text_model::*)() const, &t_text_model::f_segments_size, t_with_application_thread>())
		(L"segment_to_text"sv, t_member<size_t(t_text_model::*)(size_t) const, &t_text_model::f_segment_to_text, t_with_application_thread>())
		(L"segment_to_attribute"sv, t_member<const t_pointer<t_attribute>&(t_text_model::*)(size_t) const, &t_text_model::f_segment_to_attribute, t_with_application_thread>())
		(L"text_to_segment"sv, t_member<size_t(t_text_model::*)(size_t) const, &t_text_model::f_text_to_segment, t_with_application_thread>())
		(L"slice"sv, t_member<t_scoped(t_text_model::*)(size_t, size_t) const, &t_text_model::f_slice, t_with_application_thread>())
		(L"replace"sv, t_member<void(t_text_model::*)(size_t, size_t, t_scoped&&, std::wstring_view), &t_text_model::f_replace, t_with_application_thread>())
		(L"attribute"sv, t_member<void(t_text_model::*)(size_t, size_t, const t_pointer<t_attribute>&), &t_text_model::f_attribute, t_with_application_thread>())
	;
}

t_scoped t_type_of<xemmaix::xrafttv::t_text_model>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_type*), f_construct>::t_bind<xemmaix::xrafttv::t_text_model>::f_do(this, a_stack, a_n);
}

void t_type_of<xemmaix::xrafttv::t_wrapped_view>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	using namespace xemmaix::xraft;
	using xemmaix::xrafttv::t_text_model;
	using xemmaix::xrafttv::t_wrapped_view;
	t_define<t_wrapped_view, xraft::t_object>(a_extension, L"WrappedView"sv)
		(L"add_invalidated"sv, t_member<void(t_wrapped_view::*)(t_scoped&&), &t_wrapped_view::f_add_invalidated, t_with_application_thread>())
		(L"remove_invalidated"sv, t_member<void(t_wrapped_view::*)(t_scoped&&), &t_wrapped_view::f_remove_invalidated, t_with_application_thread>())
		(L"add_resized"sv, t_member<void(t_wrapped_view::*)(t_scoped&&), &t_wrapped_view::f_add_resized, t_with_application_thread>())
		(L"remove_resized"sv, t_member<void(t_wrapped_view::*)(t_scoped&&), &t_wrapped_view::f_remove_resized, t_with_application_thread>())
		(L"add_moved"sv, t_member<void(t_wrapped_view::*)(t_scoped&&), &t_wrapped_view::f_add_moved, t_with_application_thread>())
		(L"remove_moved"sv, t_member<void(t_wrapped_view::*)(t_scoped&&), &t_wrapped_view::f_remove_moved, t_with_application_thread>())
		(L"draw"sv, t_member<void(t_wrapped_view::*)(t_graphics&, t_pixel, size_t, size_t), &t_wrapped_view::f_draw, t_with_application_thread>())
		(L"font"sv, t_member<const t_pointer<t_font>&(t_wrapped_view::*)() const, &t_wrapped_view::f_font, t_with_application_thread>())
		(L"font__"sv, t_member<void(t_wrapped_view::*)(const t_pointer<t_font>&), &t_wrapped_view::f_font__, t_with_application_thread>())
		(L"model"sv, t_member<const t_pointer<t_text_model>&(t_wrapped_view::*)() const, &t_wrapped_view::f_model, t_with_application_thread>())
		(L"model__"sv, t_member<void(t_wrapped_view::*)(const t_pointer<t_text_model>&), &t_wrapped_view::f_model__, t_with_application_thread>())
		(L"width"sv, t_member<size_t(t_wrapped_view::*)() const, &t_wrapped_view::f_width, t_with_application_thread>())
		(L"width__"sv, t_member<void(t_wrapped_view::*)(size_t), &t_wrapped_view::f_width__, t_with_application_thread>())
		(L"height"sv, t_member<size_t(t_wrapped_view::*)() const, &t_wrapped_view::f_height, t_with_application_thread>())
		(L"caret_visible"sv, t_member<bool(t_wrapped_view::*)() const, &t_wrapped_view::f_caret_visible, t_with_application_thread>())
		(L"caret_visible__"sv, t_member<void(t_wrapped_view::*)(bool), &t_wrapped_view::f_caret_visible__, t_with_application_thread>())
		(L"caret_text"sv, t_member<size_t(t_wrapped_view::*)() const, &t_wrapped_view::f_caret_text, t_with_application_thread>())
		(L"caret_row"sv, t_member<size_t(t_wrapped_view::*)() const, &t_wrapped_view::f_caret_row, t_with_application_thread>())
		(L"caret_line"sv, t_member<size_t(t_wrapped_view::*)() const, &t_wrapped_view::f_caret_line, t_with_application_thread>())
		(L"caret_row_x"sv, t_member<size_t(t_wrapped_view::*)() const, &t_wrapped_view::f_caret_row_x, t_with_application_thread>())
		(L"caret_line_x"sv, t_member<size_t(t_wrapped_view::*)() const, &t_wrapped_view::f_caret_line_x, t_with_application_thread>())
		(L"rows_size"sv, t_member<size_t(t_wrapped_view::*)() const, &t_wrapped_view::f_rows_size, t_with_application_thread>())
		(L"lines_size"sv, t_member<size_t(t_wrapped_view::*)() const, &t_wrapped_view::f_lines_size, t_with_application_thread>())
		(L"line_to_row"sv, t_member<size_t(t_wrapped_view::*)(size_t) const, &t_wrapped_view::f_line_to_row, t_with_application_thread>())
		(L"row_to_text"sv, t_member<size_t(t_wrapped_view::*)(size_t) const, &t_wrapped_view::f_row_to_text, t_with_application_thread>())
		(L"line_to_text"sv, t_member<size_t(t_wrapped_view::*)(size_t) const, &t_wrapped_view::f_line_to_text, t_with_application_thread>())
		(L"caret_move_text"sv, t_member<void(t_wrapped_view::*)(size_t), &t_wrapped_view::f_caret_move_text, t_with_application_thread>())
		(L"caret_move_text_forward"sv, t_member<void(t_wrapped_view::*)(size_t), &t_wrapped_view::f_caret_move_text_forward, t_with_application_thread>())
		(L"caret_move_text_backward"sv, t_member<void(t_wrapped_view::*)(size_t), &t_wrapped_view::f_caret_move_text_backward, t_with_application_thread>())
		(L"caret_move_line_forward"sv, t_member<void(t_wrapped_view::*)(size_t), &t_wrapped_view::f_caret_move_line_forward, t_with_application_thread>())
		(L"caret_move_line_backward"sv, t_member<void(t_wrapped_view::*)(size_t), &t_wrapped_view::f_caret_move_line_backward, t_with_application_thread>())
		(L"caret_move_line_x"sv, t_member<void(t_wrapped_view::*)(size_t), &t_wrapped_view::f_caret_move_line_x, t_with_application_thread>())
		(L"caret_move_point"sv, t_member<void(t_wrapped_view::*)(size_t, size_t), &t_wrapped_view::f_caret_move_point, t_with_application_thread>())
	;
}

t_scoped t_type_of<xemmaix::xrafttv::t_wrapped_view>::f_do_construct(t_stacked* a_stack, size_t a_n)
{
	return t_construct_with<t_scoped(*)(t_type*), f_construct>::t_bind<xemmaix::xrafttv::t_wrapped_view>::f_do(this, a_stack, a_n);
}

}

XEMMAI__MODULE__FACTORY(xemmai::t_object* a_module)
{
	using namespace std::literals;
	return new xemmaix::xrafttv::t_extension(a_module, xemmai::t_module::f_instantiate(L"xraft"sv));
}
