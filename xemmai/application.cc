#include "application.h"

namespace xemmaix::xraft
{

namespace
{

struct t_arguments
{
	xemmai::t_object* v_arguments;
	std::vector<std::string> v_as;

	t_arguments(xemmai::t_object* a_arguments) : v_arguments(a_arguments)
	{
		auto size = t_symbol::f_instantiate(L"size"sv);
		auto shift = t_symbol::f_instantiate(L"shift"sv);
		static size_t index0;
		while (f_as<size_t>(v_arguments->f_invoke(size, index0)) > 0) {
			static size_t index1;
			auto p = v_arguments->f_invoke(shift, index1);
			f_check<t_string>(p, L"argument");
			v_as.push_back(portable::f_convert(f_as<std::wstring_view>(p)));
		}
	}
	~t_arguments()
	{
		auto push = t_symbol::f_instantiate(L"push"sv);
		size_t n = v_as.size();
		for (size_t i = 0; i < n; ++i) {
			static size_t index;
			v_arguments->f_invoke(push, index, f_global()->f_as(portable::f_convert(v_as[i])));
		}
	}
	operator std::vector<std::string>&()
	{
		return v_as;
	}
};

}

void t_application::f_wait()
{
	t_safe_region region;
	::xraft::t_application::f_wait();
}

void t_application::t_post::operator()()
{
	v_callable();
	delete this;
}

t_application::t_application(xemmai::t_object* a_arguments) : ::xraft::t_application(t_arguments(a_arguments)), t_entry(false)
{
}

}

namespace xemmai
{

void t_type_of<xraft::t_application>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
		(L"run"sv, t_member<void(t_application::*)(), &t_application::f_run>())
		(L"exit"sv, t_member<void(t_application::*)(), &t_application::f_exit>())
		(L"post"sv, t_member<void(*)(t_library*, const t_pvalue&, const t_pvalue&), f_post>())
		(L"active"sv, t_member<xraft::t_pointer<t_shell>(t_application::*)() const, &t_application::f_active>())
		(L"focus"sv, t_member<xraft::t_pointer<t_window>(t_application::*)() const, &t_application::f_focus>())
		(L"focus__"sv, t_member<void(t_application::*)(const xraft::t_pointer<t_window>&), &t_application::f_focus__>())
		(L"pointer_grabber"sv, t_member<xraft::t_pointer<t_window>(t_application::*)() const, &t_application::f_pointer_grabber>())
		(L"pointer_grabber__"sv, t_member<void(t_application::*)(const xraft::t_pointer<t_window>&), &t_application::f_pointer_grabber__>())
		(L"pointer"sv, t_member<t_point(t_application::*)() const, &t_application::f_pointer>())
		(L"input_context"sv, t_member<const xraft::t_pointer<t_input_context>&(t_application::*)() const, &t_application::f_input_context>())
#ifdef XRAFT_X11
		(L"cursor_x"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_x>())
#endif
		(L"cursor_arrow"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_arrow>())
		(L"cursor_ibeam"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_ibeam>())
		(L"cursor_vslide"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_vslide>())
		(L"cursor_hslide"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_hslide>())
#ifdef XRAFT_X11
		(L"cursor_top"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_top>())
		(L"cursor_bottom"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_bottom>())
		(L"cursor_left"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_left>())
		(L"cursor_right"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_right>())
		(L"cursor_top_left"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_top_left>())
		(L"cursor_top_right"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_top_right>())
		(L"cursor_bottom_left"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_bottom_left>())
		(L"cursor_bottom_right"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_bottom_right>())
#endif
		(L"font"sv, t_member<const xraft::t_pointer<t_font>&(t_application::*)() const, &t_application::f_font>())
		(L"count"sv, t_member<size_t(t_application::*)() const, &t_application::f_count>())
		(L"at"sv, t_member<const xraft::t_pointer<t_shell>&(t_application::*)(size_t) const, &t_application::f_at>())
		(L"add"sv,
			t_member<void(*)(t_application&, const xraft::t_pointer<t_shell>&), f_add>(),
			t_member<void(*)(t_application&, const xraft::t_pointer<t_shell>&, size_t), f_add>()
		)
		(L"remove"sv, t_member<void(t_application::*)(size_t), &t_application::f_remove>())
		(L"screen"sv, t_member<t_extent(t_application::*)() const, &t_application::f_screen>())
//		(L"input_negotiate"sv, t_member<void(t_application::*)(), &t_application::f_input_negotiate>())
	.f_derive<t_application, xemmai::t_object>();
}

}
