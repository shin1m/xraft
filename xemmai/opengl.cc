#include "opengl.h"

namespace xemmaix::xraft
{

struct t_opengl_widget : ::xraft::t_opengl_widget, t_wrapper<t_opengl_widget>
{
	static t_pvalue f_construct(t_type* a_class, const ::xraft::t_pointer<t_opengl_format>& a_format)
	{
		return t_proxy::f_construct(a_class, new t_opengl_widget(a_format));
	}

	explicit t_opengl_widget(const ::xraft::t_pointer<t_opengl_format>& a_format) : ::xraft::t_opengl_widget(a_format)
	{
	}
#define T_WINDOW t_opengl_widget
#include "window_override.h"
#undef T_WINDOW
	static void f_super__on_create(::xraft::t_opengl_widget& a_self)
	{
		if (dynamic_cast<t_opengl_widget*>(&a_self))
			a_self.::xraft::t_opengl_widget::f_on_create();
		else
			a_self.f_on_create();
	}
	virtual void f_on_create()
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_create, index);
	}
	static void f_super__on_destroy(::xraft::t_opengl_widget& a_self)
	{
		if (dynamic_cast<t_opengl_widget*>(&a_self))
			a_self.::xraft::t_opengl_widget::f_on_destroy();
		else
			a_self.f_on_destroy();
	}
	virtual void f_on_destroy()
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_destroy, index);
	}
};

}

namespace xemmai
{

void t_type_of<xraft::t_opengl_format>::f_define(t_library* a_library)
{
	t_define{a_library}.f_derive<xraft::t_opengl_format, xraft::t_object>();
}

t_pvalue t_type_of<xraft::t_opengl_format>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_overload<
		t_construct_with<t_pvalue(*)(t_type*), xemmaix::xraft::t_wrapper<xraft::t_opengl_format>::f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, bool), f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, bool, bool), f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, bool, bool, bool), f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, bool, bool, bool, bool), f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, bool, bool, bool, bool, bool), f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, bool, bool, bool, bool, bool, bool), f_construct>
	>::t_bind<xraft::t_opengl_format>::f_do(this, a_stack, a_n);
}

void t_type_of<xraft::t_opengl_widget>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
#define T_WINDOW t_opengl_widget
#include "window_define.h"
#undef T_WINDOW
		(a_library->v_symbol_on_create, t_member<void(*)(t_opengl_widget&), xemmaix::xraft::t_opengl_widget::f_super__on_create>())
		(a_library->v_symbol_on_destroy, t_member<void(*)(t_opengl_widget&), xemmaix::xraft::t_opengl_widget::f_super__on_destroy>())
		(L"format"sv, t_member<const xraft::t_pointer<t_opengl_format>&(t_opengl_widget::*)() const, &t_opengl_widget::f_format>())
		(L"created"sv, t_member<bool(t_opengl_widget::*)() const, &t_opengl_widget::f_created>())
	.f_derive<t_opengl_widget, t_widget>();
}

t_pvalue t_type_of<xraft::t_opengl_widget>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	if (a_n < 1) f_throw(L"must be called with at least an argument."sv);
	return t_construct_with<t_pvalue(*)(t_type*, const xraft::t_pointer<xraft::t_opengl_format>&), xemmaix::xraft::t_opengl_widget::f_construct>::t_bind<xraft::t_opengl_widget>::f_do(this, a_stack, 1);
}

void t_type_of<xraft::t_opengl_context>::f_define(t_library* a_library)
{
	using namespace xraft;
	t_define{a_library}
		(L"make_current"sv, t_member<bool(t_opengl_context::*)(const xraft::t_pointer<t_opengl_widget>&), &t_opengl_context::f_make_current>())
		(L"flush"sv, t_member<void(t_opengl_context::*)(), &t_opengl_context::f_flush>())
	.f_derive<t_opengl_context, xraft::t_object>();
}

t_pvalue t_type_of<xraft::t_opengl_context>::f_do_construct(t_pvalue* a_stack, size_t a_n)
{
	return t_overload<
		t_construct_with<t_pvalue(*)(t_type*, const xraft::t_pointer<xraft::t_opengl_format>&), f_construct>,
		t_construct_with<t_pvalue(*)(t_type*, const xraft::t_pointer<xraft::t_opengl_format>&, const xraft::t_pointer<xraft::t_opengl_context>&), f_construct>
	>::t_bind<xraft::t_opengl_context>::f_do(this, a_stack, a_n);
}

}
