	static void f_super__on_move(::xraft::T_WINDOW& a_self)
	{
		if (dynamic_cast<T_WINDOW*>(&a_self))
			a_self.::xraft::T_WINDOW::f_on_move();
		else
			a_self.f_on_move();
	}
	virtual void f_on_move()
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_move, index);
	}
	static void f_super__on_show(::xraft::T_WINDOW& a_self)
	{
		if (dynamic_cast<T_WINDOW*>(&a_self))
			a_self.::xraft::T_WINDOW::f_on_show();
		else
			a_self.f_on_show();
	}
	virtual void f_on_show()
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_show, index);
	}
	static void f_super__on_hide(::xraft::T_WINDOW& a_self)
	{
		if (dynamic_cast<T_WINDOW*>(&a_self))
			a_self.::xraft::T_WINDOW::f_on_hide();
		else
			a_self.f_on_hide();
	}
	virtual void f_on_hide()
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_hide, index);
	}
	static void f_super__on_paint(::xraft::T_WINDOW& a_self, t_graphics& a_g)
	{
		if (dynamic_cast<T_WINDOW*>(&a_self))
			a_self.::xraft::T_WINDOW::f_on_paint(a_g);
		else
			a_self.f_on_paint(a_g);
	}
	virtual void f_on_paint(t_graphics& a_g)
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		auto object = f_engine()->f_allocate(sizeof(t_graphics*));
		object->f_as<t_graphics*>() = &a_g;
		object->f_be(library.f_type<t_graphics>());
		try {
			static size_t index;
			f_self(this)->f_invoke(library.v_symbol_on_paint, index, object);
			object->f_as<t_graphics*>() = nullptr;
		} catch (...) {
			object->f_as<t_graphics*>() = nullptr;
			throw;
		}
	}
	static void f_super__on_focus_enter(::xraft::T_WINDOW& a_self)
	{
		if (dynamic_cast<T_WINDOW*>(&a_self))
			a_self.::xraft::T_WINDOW::f_on_focus_enter();
		else
			a_self.f_on_focus_enter();
	}
	virtual void f_on_focus_enter()
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_focus_enter, index);
	}
	static void f_super__on_focus_leave(::xraft::T_WINDOW& a_self)
	{
		if (dynamic_cast<T_WINDOW*>(&a_self))
			a_self.::xraft::T_WINDOW::f_on_focus_leave();
		else
			a_self.f_on_focus_leave();
	}
	virtual void f_on_focus_leave()
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_focus_leave, index);
	}
	static void f_super__on_key_press(::xraft::T_WINDOW& a_self, t_modifier a_modifier, t_key a_key, int a_ascii)
	{
		if (dynamic_cast<T_WINDOW*>(&a_self))
			a_self.::xraft::T_WINDOW::f_on_key_press(a_modifier, a_key, a_ascii);
		else
			a_self.f_on_key_press(a_modifier, a_key, a_ascii);
	}
	virtual void f_on_key_press(t_modifier a_modifier, t_key a_key, char a_ascii)
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_key_press, index, library.f_as(a_modifier), library.f_as(a_key), library.f_as(static_cast<int>(a_ascii)));
	}
	static void f_super__on_key_release(::xraft::T_WINDOW& a_self, t_modifier a_modifier, t_key a_key, int a_ascii)
	{
		if (dynamic_cast<T_WINDOW*>(&a_self))
			a_self.::xraft::T_WINDOW::f_on_key_release(a_modifier, a_key, a_ascii);
		else
			a_self.f_on_key_release(a_modifier, a_key, a_ascii);
	}
	virtual void f_on_key_release(t_modifier a_modifier, t_key a_key, char a_ascii)
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_key_release, index, library.f_as(a_modifier), library.f_as(a_key), library.f_as(static_cast<int>(a_ascii)));
	}
	static void f_super__on_input_compose(::xraft::T_WINDOW& a_self)
	{
		if (dynamic_cast<T_WINDOW*>(&a_self))
			a_self.::xraft::T_WINDOW::f_on_input_compose();
		else
			a_self.f_on_input_compose();
	}
	virtual void f_on_input_compose()
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_input_compose, index);
	}
	static void f_super__on_input_commit(::xraft::T_WINDOW& a_self, std::wstring_view a_s)
	{
		if (dynamic_cast<T_WINDOW*>(&a_self))
			a_self.::xraft::T_WINDOW::f_on_input_commit(a_s.data(), a_s.size());
		else
			a_self.f_on_input_commit(a_s.data(), a_s.size());
	}
	virtual void f_on_input_commit(const wchar_t* a_cs, size_t a_n)
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_input_commit, index, t_string::f_instantiate(a_cs, a_n));
	}
	static t_rectangle f_super__on_input_spot(::xraft::T_WINDOW& a_self)
	{
		if (dynamic_cast<T_WINDOW*>(&a_self))
			return a_self.::xraft::T_WINDOW::f_on_input_spot();
		else
			return a_self.f_on_input_spot();
	}
	virtual t_rectangle f_on_input_spot()
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		return f_as<t_rectangle>(f_self(this)->f_invoke(library.v_symbol_on_input_spot, index));
	}
	static void f_super__on_button_press(::xraft::T_WINDOW& a_self, t_modifier a_modifier, t_button a_button, int a_x, int a_y)
	{
		if (dynamic_cast<T_WINDOW*>(&a_self))
			a_self.::xraft::T_WINDOW::f_on_button_press(a_modifier, a_button, a_x, a_y);
		else
			a_self.f_on_button_press(a_modifier, a_button, a_x, a_y);
	}
	virtual void f_on_button_press(t_modifier a_modifier, t_button a_button, int a_x, int a_y)
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_button_press, index, library.f_as(a_modifier), library.f_as(a_button), library.f_as(a_x), library.f_as(a_y));
	}
	static void f_super__on_button_release(::xraft::T_WINDOW& a_self, t_modifier a_modifier, t_button a_button, int a_x, int a_y)
	{
		if (dynamic_cast<T_WINDOW*>(&a_self))
			a_self.::xraft::T_WINDOW::f_on_button_release(a_modifier, a_button, a_x, a_y);
		else
			a_self.f_on_button_release(a_modifier, a_button, a_x, a_y);
	}
	virtual void f_on_button_release(t_modifier a_modifier, t_button a_button, int a_x, int a_y)
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_button_release, index, library.f_as(a_modifier), library.f_as(a_button), library.f_as(a_x), library.f_as(a_y));
	}
	static void f_super__on_pointer_enter(::xraft::T_WINDOW& a_self, t_modifier a_modifier, int a_x, int a_y, t_cross_mode a_mode, t_cross_detail a_detail)
	{
		if (dynamic_cast<T_WINDOW*>(&a_self))
			a_self.::xraft::T_WINDOW::f_on_pointer_enter(a_modifier, a_x, a_y, a_mode, a_detail);
		else
			a_self.f_on_pointer_enter(a_modifier, a_x, a_y, a_mode, a_detail);
	}
	virtual void f_on_pointer_enter(t_modifier a_modifier, int a_x, int a_y, t_cross_mode a_mode, t_cross_detail a_detail)
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_pointer_enter, index, library.f_as(a_modifier), library.f_as(a_x), library.f_as(a_y), library.f_as(a_mode), library.f_as(a_detail));
	}
	static void f_super__on_pointer_leave(::xraft::T_WINDOW& a_self, t_modifier a_modifier, int a_x, int a_y, t_cross_mode a_mode, t_cross_detail a_detail)
	{
		if (dynamic_cast<T_WINDOW*>(&a_self))
			a_self.::xraft::T_WINDOW::f_on_pointer_leave(a_modifier, a_x, a_y, a_mode, a_detail);
		else
			a_self.f_on_pointer_leave(a_modifier, a_x, a_y, a_mode, a_detail);
	}
	virtual void f_on_pointer_leave(t_modifier a_modifier, int a_x, int a_y, t_cross_mode a_mode, t_cross_detail a_detail)
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_pointer_leave, index, library.f_as(a_modifier), library.f_as(a_x), library.f_as(a_y), library.f_as(a_mode), library.f_as(a_detail));
	}
	static void f_super__on_pointer_move(::xraft::T_WINDOW& a_self, t_modifier a_modifier, int a_x, int a_y)
	{
		if (dynamic_cast<T_WINDOW*>(&a_self))
			a_self.::xraft::T_WINDOW::f_on_pointer_move(a_modifier, a_x, a_y);
		else
			a_self.f_on_pointer_move(a_modifier, a_x, a_y);
	}
	virtual void f_on_pointer_move(t_modifier a_modifier, int a_x, int a_y)
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_on_pointer_move, index, library.f_as(a_modifier), library.f_as(a_x), library.f_as(a_y));
	}
	static void f_super__add(::xraft::T_WINDOW& a_self, const ::xraft::t_pointer<::xraft::t_widget>& a_widget)
	{
		if (dynamic_cast<T_WINDOW*>(&a_self))
			a_self.::xraft::T_WINDOW::f_add(a_widget);
		else
			a_self.f_add(a_widget);
	}
	static void f_super__add(::xraft::T_WINDOW& a_self, const ::xraft::t_pointer<::xraft::t_widget>& a_widget, size_t a_i)
	{
		if (dynamic_cast<T_WINDOW*>(&a_self))
			a_self.::xraft::T_WINDOW::f_add(a_widget, a_i);
		else
			a_self.f_add(a_widget, a_i);
	}
	virtual void f_add(const ::xraft::t_pointer<::xraft::t_widget>& a_widget, size_t a_i = size_t(-1))
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_add, index, library.f_as(a_widget), library.f_as(a_i));
	}
	static void f_super__remove(::xraft::T_WINDOW& a_self, size_t a_i)
	{
		if (dynamic_cast<T_WINDOW*>(&a_self))
			a_self.::xraft::T_WINDOW::f_remove(a_i);
		else
			a_self.f_remove(a_i);
	}
	virtual void f_remove(size_t a_i)
	{
		auto& library = f_self(this)->f_type()->v_module->f_as<t_library>();
		static size_t index;
		f_self(this)->f_invoke(library.v_symbol_remove, index, library.f_as(a_i));
	}
