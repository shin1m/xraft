		(a_library->v_symbol_on_move, t_member<void(*)(xraft::T_WINDOW&), xemmaix::xraft::T_WINDOW::f_super__on_move>())
		(a_library->v_symbol_on_show, t_member<void(*)(xraft::T_WINDOW&), xemmaix::xraft::T_WINDOW::f_super__on_show>())
		(a_library->v_symbol_on_hide, t_member<void(*)(xraft::T_WINDOW&), xemmaix::xraft::T_WINDOW::f_super__on_hide>())
		(a_library->v_symbol_on_paint, t_member<void(*)(xraft::T_WINDOW&, t_graphics&), xemmaix::xraft::T_WINDOW::f_super__on_paint>())
		(a_library->v_symbol_on_focus_enter, t_member<void(*)(xraft::T_WINDOW&), xemmaix::xraft::T_WINDOW::f_super__on_focus_enter>())
		(a_library->v_symbol_on_focus_leave, t_member<void(*)(xraft::T_WINDOW&), xemmaix::xraft::T_WINDOW::f_super__on_focus_leave>())
		(a_library->v_symbol_on_key_press, t_member<void(*)(xraft::T_WINDOW&, t_modifier, t_key, int), xemmaix::xraft::T_WINDOW::f_super__on_key_press>())
		(a_library->v_symbol_on_key_release, t_member<void(*)(xraft::T_WINDOW&, t_modifier, t_key, int), xemmaix::xraft::T_WINDOW::f_super__on_key_release>())
		(a_library->v_symbol_on_input_compose, t_member<void(*)(xraft::T_WINDOW&), xemmaix::xraft::T_WINDOW::f_super__on_input_compose>())
		(a_library->v_symbol_on_input_commit, t_member<void(*)(xraft::T_WINDOW&, std::wstring_view), xemmaix::xraft::T_WINDOW::f_super__on_input_commit>())
		(a_library->v_symbol_on_input_spot, t_member<t_rectangle(*)(xraft::T_WINDOW&), xemmaix::xraft::T_WINDOW::f_super__on_input_spot>())
		(a_library->v_symbol_on_button_press, t_member<void(*)(xraft::T_WINDOW&, t_modifier, t_button, int, int), xemmaix::xraft::T_WINDOW::f_super__on_button_press>())
		(a_library->v_symbol_on_button_release, t_member<void(*)(xraft::T_WINDOW&, t_modifier, t_button, int, int), xemmaix::xraft::T_WINDOW::f_super__on_button_release>())
		(a_library->v_symbol_on_pointer_enter, t_member<void(*)(xraft::T_WINDOW&, t_modifier, int, int, t_cross_mode, t_cross_detail), xemmaix::xraft::T_WINDOW::f_super__on_pointer_enter>())
		(a_library->v_symbol_on_pointer_leave, t_member<void(*)(xraft::T_WINDOW&, t_modifier, int, int, t_cross_mode, t_cross_detail), xemmaix::xraft::T_WINDOW::f_super__on_pointer_leave>())
		(a_library->v_symbol_on_pointer_move, t_member<void(*)(xraft::T_WINDOW&, t_modifier, int, int), xemmaix::xraft::T_WINDOW::f_super__on_pointer_move>())
		(a_library->v_symbol_add,
			t_member<void(*)(xraft::T_WINDOW&, const xraft::t_pointer<t_widget>&), xemmaix::xraft::T_WINDOW::f_super__add>(),
			t_member<void(*)(xraft::T_WINDOW&, const xraft::t_pointer<t_widget>&, size_t), xemmaix::xraft::T_WINDOW::f_super__add>()
		)
		(a_library->v_symbol_remove, t_member<void(*)(xraft::T_WINDOW&, size_t), xemmaix::xraft::T_WINDOW::f_super__remove>())
