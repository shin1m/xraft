#include "opengl.h"

namespace xraft
{

namespace xemmai
{

struct t_opengl_widget : xraft::t_opengl_widget, t_wrapper<t_opengl_widget>
{
	static t_transfer f_construct(::xemmai::t_object* a_class, const t_pointer<t_opengl_format>& a_format)
	{
		return t_proxy::f_construct(a_class, new t_opengl_widget(a_format));
	}

	explicit t_opengl_widget(const t_pointer<t_opengl_format>& a_format) : xraft::t_opengl_widget(a_format)
	{
	}
#define T_WINDOW t_opengl_widget
#include "window_override.h"
#undef T_WINDOW
	static void f_super__on_create(xraft::t_opengl_widget& a_self)
	{
		if (dynamic_cast<t_opengl_widget*>(&a_self))
			a_self.xraft::t_opengl_widget::f_on_create();
		else
			a_self.f_on_create();
	}
	virtual void f_on_create()
	{
		t_extension* extension = f_extension<t_extension>(f_as<t_type*>(f_self(this)->f_type())->v_module);
		f_self(this)->f_get(extension->v_symbol_on_create)();
	}
	static void f_super__on_destroy(xraft::t_opengl_widget& a_self)
	{
		if (dynamic_cast<t_opengl_widget*>(&a_self))
			a_self.xraft::t_opengl_widget::f_on_destroy();
		else
			a_self.f_on_destroy();
	}
	virtual void f_on_destroy()
	{
		t_extension* extension = f_extension<t_extension>(f_as<t_type*>(f_self(this)->f_type())->v_module);
		f_self(this)->f_get(extension->v_symbol_on_destroy)();
	}
};

}

}

namespace xemmai
{

using xraft::xemmai::t_with_application_thread;

void t_type_of<t_opengl_format>::f_define(t_extension* a_extension)
{
	t_define<t_opengl_format, xraft::t_object>(a_extension, L"GLFormat");
}

t_transfer t_type_of<t_opengl_format>::f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n)
{
	return
		t_overload<t_construct_with<t_transfer (*)(::xemmai::t_object*), xraft::xemmai::t_wrapper<t_opengl_format>::f_construct>,
		t_overload<t_construct_with<t_transfer (*)(::xemmai::t_object*, bool), f_construct>,
		t_overload<t_construct_with<t_transfer (*)(::xemmai::t_object*, bool, bool), f_construct>,
		t_overload<t_construct_with<t_transfer (*)(::xemmai::t_object*, bool, bool, bool), f_construct>,
		t_overload<t_construct_with<t_transfer (*)(::xemmai::t_object*, bool, bool, bool, bool), f_construct>,
		t_overload<t_construct_with<t_transfer (*)(::xemmai::t_object*, bool, bool, bool, bool, bool), f_construct>,
		t_overload<t_construct_with<t_transfer (*)(::xemmai::t_object*, bool, bool, bool, bool, bool, bool), f_construct>
	> > > > > > >::t_bind<t_opengl_format>::f_do(a_class, a_stack, a_n);
}

void t_type_of<t_opengl_widget>::f_define(t_extension* a_extension)
{
	t_define<t_opengl_widget, t_widget>(a_extension, L"GLWidget")
#define T_WINDOW t_opengl_widget
#include "window_define.h"
#undef T_WINDOW
		(a_extension->v_symbol_on_create, t_member<void (*)(t_opengl_widget&), xraft::xemmai::t_opengl_widget::f_super__on_create, t_with_application_thread>())
		(a_extension->v_symbol_on_destroy, t_member<void (*)(t_opengl_widget&), xraft::xemmai::t_opengl_widget::f_super__on_destroy, t_with_application_thread>())
		(L"format", t_member<const xraft::t_pointer<t_opengl_format>& (t_opengl_widget::*)() const, &t_opengl_widget::f_format, t_with_application_thread>())
		(L"created", t_member<bool (t_opengl_widget::*)() const, &t_opengl_widget::f_created, t_with_application_thread>())
	;
}

t_type* t_type_of<t_opengl_widget>::f_derive(::xemmai::t_object* a_this)
{
	return new t_type_of(v_module, a_this);
}

t_transfer t_type_of<t_opengl_widget>::f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n)
{
	if (a_n < 1) t_throwable::f_throw(L"must be called with at least an argument.");
	return t_construct_with<t_transfer (*)(::xemmai::t_object*, const xraft::t_pointer<t_opengl_format>&), xraft::xemmai::t_opengl_widget::f_construct>::t_bind<t_opengl_widget>::f_do(a_class, a_stack, 1);
}

void t_type_of<t_opengl_context>::f_define(t_extension* a_extension)
{
	t_define<t_opengl_context, xraft::t_object>(a_extension, L"GLContext")
		(L"make_current", t_member<bool (t_opengl_context::*)(const xraft::t_pointer<t_opengl_widget>&), &t_opengl_context::f_make_current, t_with_application_thread>())
		(L"flush", t_member<void (t_opengl_context::*)(), &t_opengl_context::f_flush, t_with_application_thread>())
	;
}

t_transfer t_type_of<t_opengl_context>::f_construct(::xemmai::t_object* a_class, t_slot* a_stack, size_t a_n)
{
	return
		t_overload<t_construct_with<t_transfer (*)(::xemmai::t_object*, const xraft::t_pointer<t_opengl_format>&), f_construct>,
		t_overload<t_construct_with<t_transfer (*)(::xemmai::t_object*, const xraft::t_pointer<t_opengl_format>&, const xraft::t_pointer<t_opengl_context>&), f_construct>
	> >::t_bind<t_opengl_context>::f_do(a_class, a_stack, a_n);
}

}
