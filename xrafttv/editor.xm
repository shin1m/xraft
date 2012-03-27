system = Module("system");
xraft = Module("xraft");
xrafttv = Module("xrafttv");

Attribute = Class(xrafttv.Attribute) :: @{
	$__string = @{
		"(" + $foreground() + ", " + $background() + ")";
	};
};

Editor = Class(xraft.Frame) :: @{
	$insert = @(s) $_model.replace($_view.caret_text(), 0, '('(s.size(), $_attribute_default)), s);
	$backspace = @{
		model = $_view.caret_text();
		if (model > 0) $_model.replace(model - 1, 1, '(), "");
	};
	$delete = @{
		model = $_view.caret_text();
		if (model < $_model.text_size()) $_model.replace(model, 1, '(), "");
	};
	$on_move = @{
		extent = $geometry();
		$_view.width__(extent.width());
	};
	$on_paint = @(g) {
		g.translate(0, -$_position);
		bounds = g.bounds();
		$_view.draw(g, $_end.pixel(), bounds.y(), bounds.y() + bounds.height());
	};
	key_binds = {
		xraft.Key.LEFT: @{
			$_view.caret_move_text_backward(1);
		},
		xraft.Key.RIGHT: @{
			$_view.caret_move_text_forward(1);
		},
		xraft.Key.UP: @{
			$_view.caret_move_line_backward(1);
		},
		xraft.Key.DOWN: @{
			$_view.caret_move_line_forward(1);
		},
		xraft.Key.RETURN: @{
			$insert("\n");
		},
		xraft.Key.BACK_SPACE: @{
			$backspace();
		},
		xraft.Key.DELETE: @{
			$delete();
		},
		xraft.Key.F1: @{
			for (c = 0x20; c <= 0x7e; c = c + 1) $insert(String.from_code(c));
		},
		xraft.Key.F2: @{
			segments = '(3, 5, 7, 9, 11, 13, 17, 19);
			attributes = '(
				Attribute(xraft.Color(0, 0, 0).pixel(), xraft.Color(255, 255, 255).pixel()),
				Attribute(xraft.Color(255, 0, 0).pixel(), xraft.Color(255, 255, 255).pixel()),
				Attribute(xraft.Color(0, 255, 0).pixel(), xraft.Color(255, 255, 255).pixel()),
				Attribute(xraft.Color(0, 0, 255).pixel(), xraft.Color(255, 255, 255).pixel())
			);
			n = $_model.text_size();
			i = 0;
			while (i < n) {
				m = segments[i % segments.size()];
				if (i + m > n) m = n - i;
				$_model.attribute(i, m, attributes[i % attributes.size()]);
				i = i + m;
			}
		},
		xraft.Key.F3: @{
			n = $_model.text_size();
			for (i = 0; i < n; i = i + 10) system.out.write_line($_model.slice(i, 10));
		}
	};
	$on_key_press = @(modifier, key, ascii) {
		if (key_binds.has(key))
			key_binds[key][$]();
		else if (ascii != 0)
			$insert(String.from_code(ascii));
		h = xraft.application().font().height();
		$scroll_viewable($_view.caret_row() * h, h);
	};
	$on_input_compose = @{
		if ($_composing_size <= 0) $_composing_begin = $_view.caret_text();
		composition = $input_context().composition();
		text = composition[0];
		attributes = composition[1];
		segments = [];
		caret = 0;
		i = 0;
		while (i < attributes.size()) {
			a = attributes[i];
			j = i + 1;
			while (j < attributes.size() && attributes[j] == a) j = j + 1;
			segments.push('(j - i, $_attribute_compose[a]));
			if ((a & xraft.InputAttribute.REVERSE) != 0) caret = j;
			i = j;
		}
		$_model.replace($_composing_begin, $_composing_size, segments, text);
		$_view.caret_move_text($_composing_begin + caret);
		$_composing_size = text.size();
	};
	$on_input_commit = @(text) {
		if ($_composing_size <= 0) $_composing_begin = $_view.caret_text();
		$_model.replace($_composing_begin, $_composing_size, '('(text.size(), $_attribute_default)), text);
		$_composing_size = 0;
	};
	$on_input_spot = @{
		h = xraft.application().font().height();
		xraft.Rectangle($_view.caret_row_x(), $_view.caret_row() * h, 0, h);
	};
	$on_button_press = @(modifier, button, x, y) {
		if (button == xraft.Button.BUTTON3) $on_close();
	};
	$on_close = @{
		xraft.application().exit();
	};
	$invalidated = @(y, height) {
		extent = $geometry();
		top = y - $_position;
		bottom = top + height;
		if (top < 0) top = 0;
		$invalidate(0, top, extent.width(), bottom - top);
	};
	$resized = @{
		$position__($_position);
	};
	$moved = @(y, height, delta) {
		extent = $geometry();
		top = y - $_position;
		bottom = top + height;
		if (top < 0) top = 0;
		$scroll(0, top, extent.width(), bottom - top, 0, delta);
	};
	$__initialize = @{
		:$^__initialize[$]();
		$_end = xraft.Color(255, 255, 255);
		$_attribute_default = Attribute(xraft.Color(0, 0, 0).pixel(), xraft.Color(255, 255, 255).pixel());
		$_attribute_compose = [
			Attribute(xraft.Color(63, 63, 63).pixel(), xraft.Color(255, 255, 255).pixel()),
			Attribute(xraft.Color(191, 191, 191).pixel(), xraft.Color(0, 0, 0).pixel()),
			Attribute(xraft.Color(0, 0, 0).pixel(), xraft.Color(191, 191, 191).pixel()),
			Attribute(xraft.Color(255, 255, 255).pixel(), xraft.Color(0, 0, 0).pixel())
		];
		$_position = 0;
		$_composing_size = 0;
		$_composing_begin = 0;
		$_model = xrafttv.TextModel();
		$_model.enable_notification();
		$_view = xrafttv.WrappedView();
		$_view.model__($_model);
		$_view.add_invalidated($invalidated);
		$_view.add_resized($resized);
		$_view.add_moved($moved);
		$_view.caret_visible__(true);
		$input_context__(xraft.application().input_context());
	};
	$position__ = @(position) {
		extent = $geometry();
		limit = $_view.rows_size() * xraft.application().font().height();
		if (position + extent.height() > limit) position = limit - extent.height();
		if (position < 0) position = 0;
		if (position == $_position) return;
		$scroll(0, 0, extent.width(), extent.height(), 0, $_position - position);
		$_position = position;
	};
	$scroll_viewable = @(y, height) {
		extent = $geometry();
		position = $_position;
		if (position + extent.height() < y + height) position = y + height - extent.height();
		if (position > y) position = y;
		$position__(position);
	};
};

xraft.main(system.arguments, @(application) {
	frame = Editor();
	frame.caption__("Editor");
	frame.move(xraft.Rectangle(0, 0, 320, 240));
	application.add(frame);
	frame.show();
	application.run();
});
