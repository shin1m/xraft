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
		t_scoped size = t_symbol::f_instantiate(L"size");
		t_scoped shift = t_symbol::f_instantiate(L"shift");
		while (f_as<size_t>(v_arguments->f_invoke(size)) > 0) {
			t_scoped p = v_arguments->f_invoke(shift);
			f_check<std::wstring>(p, L"argument");
			v_as.push_back(portable::f_convert(f_as<const std::wstring&>(p)));
		}
	}
	~t_arguments()
	{
		t_scoped push = t_symbol::f_instantiate(L"push");
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
	t_define<t_application, xemmai::t_object>(a_extension, L"Application")
		(L"run", t_member<void(t_application::*)(), &t_application::f_run, t_with_application_thread>())
		(L"exit", t_member<void(t_application::*)(), &t_application::f_exit, t_with_application_thread>())
		(L"post", t_member<void(*)(t_application&, t_scoped&&), f_post, t_with_lock_for_write>())
		(L"active", t_member<t_pointer<t_shell>(t_application::*)() const, &t_application::f_active, t_with_application_thread>())
		(L"focus", t_member<t_pointer<t_window>(t_application::*)() const, &t_application::f_focus, t_with_application_thread>())
		(L"focus__", t_member<void(t_application::*)(const t_pointer<t_window>&), &t_application::f_focus__, t_with_application_thread>())
		(L"pointer_grabber", t_member<t_pointer<t_window>(t_application::*)() const, &t_application::f_pointer_grabber, t_with_application_thread>())
		(L"pointer_grabber__", t_member<void(t_application::*)(const t_pointer<t_window>&), &t_application::f_pointer_grabber__, t_with_application_thread>())
		(L"pointer", t_member<t_point(t_application::*)() const, &t_application::f_pointer, t_with_application_thread>())
		(L"input_context", t_member<const t_pointer<t_input_context>&(t_application::*)() const, &t_application::f_input_context, t_with_application_thread>())
#ifdef XRAFT_X11
		(L"cursor_x", t_member<t_cursor(t_application::*)(), &t_application::f_cursor_x, t_with_application_thread>())
#endif
		(L"cursor_arrow", t_member<t_cursor(t_application::*)(), &t_application::f_cursor_arrow, t_with_application_thread>())
		(L"cursor_ibeam", t_member<t_cursor(t_application::*)(), &t_application::f_cursor_ibeam, t_with_application_thread>())
		(L"cursor_vslide", t_member<t_cursor(t_application::*)(), &t_application::f_cursor_vslide, t_with_application_thread>())
		(L"cursor_hslide", t_member<t_cursor(t_application::*)(), &t_application::f_cursor_hslide, t_with_application_thread>())
#ifdef XRAFT_X11
		(L"cursor_top", t_member<t_cursor(t_application::*)(), &t_application::f_cursor_top, t_with_application_thread>())
		(L"cursor_bottom", t_member<t_cursor(t_application::*)(), &t_application::f_cursor_bottom, t_with_application_thread>())
		(L"cursor_left", t_member<t_cursor(t_application::*)(), &t_application::f_cursor_left, t_with_application_thread>())
		(L"cursor_right", t_member<t_cursor(t_application::*)(), &t_application::f_cursor_right, t_with_application_thread>())
		(L"cursor_top_left", t_member<t_cursor(t_application::*)(), &t_application::f_cursor_top_left, t_with_application_thread>())
		(L"cursor_top_right", t_member<t_cursor(t_application::*)(), &t_application::f_cursor_top_right, t_with_application_thread>())
		(L"cursor_bottom_left", t_member<t_cursor(t_application::*)(), &t_application::f_cursor_bottom_left, t_with_application_thread>())
		(L"cursor_bottom_right", t_member<t_cursor(t_application::*)(), &t_application::f_cursor_bottom_right, t_with_application_thread>())
#endif
		(L"font", t_member<const t_pointer<t_font>&(t_application::*)() const, &t_application::f_font, t_with_application_thread>())
		(L"count", t_member<size_t(t_application::*)() const, &t_application::f_count, t_with_application_thread>())
		(L"at", t_member<const t_pointer<t_shell>&(t_application::*)(size_t) const, &t_application::f_at, t_with_application_thread>())
		(L"add",
			t_member<void(*)(t_application&, const t_pointer<t_shell>&), f_add, t_with_application_thread>(),
			t_member<void(*)(t_application&, const t_pointer<t_shell>&, size_t), f_add, t_with_application_thread>()
		)
		(L"remove", t_member<void(t_application::*)(size_t), &t_application::f_remove, t_with_application_thread>())
		(L"screen", t_member<t_extent(t_application::*)() const, &t_application::f_screen, t_with_application_thread>())
//		(L"input_negotiate", t_member<void(t_application::*)(), &t_application::f_input_negotiate, t_with_application_thread>())
	;
}

t_type* t_type_of<xraft::t_application>::f_derive()
{
	return nullptr;
}

t_scoped t_type_of<xraft::t_application>::f_construct(t_stacked* a_stack, size_t a_n)
{
	t_throwable::f_throw(L"uninstantiatable.");
}

}
