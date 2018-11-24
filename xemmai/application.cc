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
		while (f_as<size_t>(v_arguments->f_invoke(size)) > 0) {
			t_scoped p = v_arguments->f_invoke(shift);
			f_check<t_string>(p, L"argument");
			v_as.push_back(portable::f_convert(f_as<std::wstring_view>(p)));
		}
	}
	~t_arguments()
	{
		auto push = t_symbol::f_instantiate(L"push"sv);
		size_t n = v_as.size();
		for (size_t i = 0; i < n; ++i) v_arguments->f_invoke(push, f_global()->f_as(portable::f_convert(v_as[i])));
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
	t_thread::f_cache_acquire();
	v_callable();
	delete this;
}

t_application::t_application(t_extension* a_extension, xemmai::t_object* a_arguments) : ::xraft::t_application(t_arguments(a_arguments)), t_entry(false), v_extension(a_extension)
{
}

}

namespace xemmai
{

void t_type_of<xraft::t_application>::f_define(t_extension* a_extension)
{
	using namespace xraft;
	using xemmaix::xraft::t_with_application_thread;
	t_define<t_application, xemmai::t_object>(a_extension, L"Application"sv)
		(L"run"sv, t_member<void(t_application::*)(), &t_application::f_run, t_with_application_thread>())
		(L"exit"sv, t_member<void(t_application::*)(), &t_application::f_exit, t_with_application_thread>())
		(L"post"sv, t_member<void(*)(t_application&, t_scoped&&), f_post, t_with_lock_for_write>())
		(L"active"sv, t_member<t_pointer<t_shell>(t_application::*)() const, &t_application::f_active, t_with_application_thread>())
		(L"focus"sv, t_member<t_pointer<t_window>(t_application::*)() const, &t_application::f_focus, t_with_application_thread>())
		(L"focus__"sv, t_member<void(t_application::*)(const t_pointer<t_window>&), &t_application::f_focus__, t_with_application_thread>())
		(L"pointer_grabber"sv, t_member<t_pointer<t_window>(t_application::*)() const, &t_application::f_pointer_grabber, t_with_application_thread>())
		(L"pointer_grabber__"sv, t_member<void(t_application::*)(const t_pointer<t_window>&), &t_application::f_pointer_grabber__, t_with_application_thread>())
		(L"pointer"sv, t_member<t_point(t_application::*)() const, &t_application::f_pointer, t_with_application_thread>())
		(L"input_context"sv, t_member<const t_pointer<t_input_context>&(t_application::*)() const, &t_application::f_input_context, t_with_application_thread>())
#ifdef XRAFT_X11
		(L"cursor_x"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_x, t_with_application_thread>())
#endif
		(L"cursor_arrow"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_arrow, t_with_application_thread>())
		(L"cursor_ibeam"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_ibeam, t_with_application_thread>())
		(L"cursor_vslide"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_vslide, t_with_application_thread>())
		(L"cursor_hslide"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_hslide, t_with_application_thread>())
#ifdef XRAFT_X11
		(L"cursor_top"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_top, t_with_application_thread>())
		(L"cursor_bottom"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_bottom, t_with_application_thread>())
		(L"cursor_left"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_left, t_with_application_thread>())
		(L"cursor_right"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_right, t_with_application_thread>())
		(L"cursor_top_left"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_top_left, t_with_application_thread>())
		(L"cursor_top_right"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_top_right, t_with_application_thread>())
		(L"cursor_bottom_left"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_bottom_left, t_with_application_thread>())
		(L"cursor_bottom_right"sv, t_member<t_cursor(t_application::*)(), &t_application::f_cursor_bottom_right, t_with_application_thread>())
#endif
		(L"font"sv, t_member<const t_pointer<t_font>&(t_application::*)() const, &t_application::f_font, t_with_application_thread>())
		(L"count"sv, t_member<size_t(t_application::*)() const, &t_application::f_count, t_with_application_thread>())
		(L"at"sv, t_member<const t_pointer<t_shell>&(t_application::*)(size_t) const, &t_application::f_at, t_with_application_thread>())
		(L"add"sv,
			t_member<void(*)(t_application&, const t_pointer<t_shell>&), f_add, t_with_application_thread>(),
			t_member<void(*)(t_application&, const t_pointer<t_shell>&, size_t), f_add, t_with_application_thread>()
		)
		(L"remove"sv, t_member<void(t_application::*)(size_t), &t_application::f_remove, t_with_application_thread>())
		(L"screen"sv, t_member<t_extent(t_application::*)() const, &t_application::f_screen, t_with_application_thread>())
//		(L"input_negotiate"sv, t_member<void(t_application::*)(), &t_application::f_input_negotiate, t_with_application_thread>())
	;
}

}
