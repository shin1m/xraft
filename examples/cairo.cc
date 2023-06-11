#include <deque>
#include <vector>
#include <cstdio>
#include <cmath>
#include <xraft/application.h>
#include <xraft/graphics.h>
#include <xraft/cairo.h>

using namespace xraft;

void f_foo()
{
	cairo_font_face_t* font_face = cairo_toy_font_face_create("Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_matrix_t matrix;
	cairo_matrix_init_identity(&matrix);
	cairo_font_options_t* options = cairo_font_options_create();
	cairo_scaled_font_t* scaled_font = cairo_scaled_font_create(font_face, &matrix, &matrix, options);
	std::fprintf(stderr, "font face: %d\n", cairo_font_face_get_reference_count(font_face));
	cairo_font_face_destroy(font_face);
	cairo_font_options_destroy(options);
	std::fprintf(stderr, "scaled font: %d\n", cairo_scaled_font_get_reference_count(scaled_font));
	cairo_scaled_font_destroy(scaled_font);
};

void f_text(cairo_t* a_context)
{
	cairo_font_options_t* options = cairo_font_options_create();
	cairo_font_options_set_antialias(options, CAIRO_ANTIALIAS_GRAY);
	cairo_font_options_set_hint_style(options, CAIRO_HINT_STYLE_NONE);
	cairo_font_options_set_hint_metrics(options, CAIRO_HINT_METRICS_OFF);
	cairo_set_font_options(a_context, options);
	cairo_font_options_destroy(options);
	cairo_select_font_face(a_context, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(a_context, 90.0);
	cairo_move_to(a_context, 10.0, 135.0);
	cairo_show_text(a_context, "Hello");
	cairo_move_to(a_context, 70.0, 165.0);
	cairo_text_path(a_context, "void");
	cairo_set_source_rgb(a_context, 0.5, 0.5, 1.0);
	cairo_fill_preserve(a_context);
	cairo_set_source_rgb(a_context, 0.0, 0.0, 0.0);
	cairo_set_line_width(a_context, 2.56);
	cairo_stroke(a_context);
	cairo_set_source_rgba(a_context, 1.0, 0.2, 0.2, 0.6);
	cairo_arc(a_context, 10.0, 135.0, 5.12, 0.0, 2.0 * M_PI);
	cairo_close_path(a_context);
	cairo_arc(a_context, 70.0, 165.0, 5.12, 0.0, 2.0 * M_PI);
	cairo_fill(a_context);
};

class t_cairoc : public t_frame
{
	void f_paint(cairo_t* a_context, unsigned a_width, unsigned a_height)
	{
		double width0 = 256.0;
		double height0 = 256.0;
		double margin = 8.0;
		double width1 = a_width - margin * 2.0;
		double height1 = a_height - margin * 2.0;
		double scale = std::min(width1 / width0, height1 / height0);
		cairo_rectangle(a_context, 0.0, 0.0, a_width, a_height);
		cairo_set_source_rgb(a_context, 1.0, 1.0, 1.0);
		cairo_fill(a_context);
		double sw = round(width0 * scale);
		double sh = round(height0 * scale);
		cairo_translate(a_context, round(margin + (width1 - sw) * 0.5), round(margin + (height1 - sh) * 0.5));
		cairo_rectangle(a_context, -0.5, -0.5, sw + 1.0, sh + 1.0);
		cairo_set_source_rgb(a_context, 0.95, 0.95, 0.95);
		cairo_fill_preserve(a_context);
		cairo_set_source_rgb(a_context, 0.6, 0.6, 0.6);
		cairo_set_line_width(a_context, 1.0);
		cairo_stroke_preserve(a_context);
		cairo_clip(a_context);
		cairo_new_path(a_context);
		cairo_set_source_rgb(a_context, 0.0, 0.0, 0.0);
		cairo_scale(a_context, scale, scale);
		f_text(a_context);
	}
	void f_paint(cairo_t* a_context)
	{
		t_extent extent = f_geometry();
		f_paint(a_context, extent.v_width, extent.v_height);
	}
	virtual void f_on_paint(t_graphics& a_g)
	{
		f_draw(a_g, [&](auto a_context)
		{
			f_paint(a_context);
		});
	}
	virtual void f_on_key_press(t_modifier a_modifier, t_key a_key, char a_ascii)
	{
		if (a_key == e_key__SPACE) {
			t_extent extent = f_geometry();
			f_invalidate(0, 0, extent.v_width, extent.v_height);
		} else if (a_key == e_key__I) {
			cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 256, 256);
			cairo_t* context = cairo_create(surface);
			f_paint(context, 256, 256);
			cairo_destroy(context);
			cairo_surface_write_to_png(surface, "foo.png");
			cairo_surface_destroy(surface);
		} else if (a_key == e_key__Q) {
			f_on_close();
		}
	}
	virtual void f_on_close()
	{
		f_application()->f_exit();
	}
};

int main(int argc, char* argv[])
{
	f_foo();
	std::vector<std::string> arguments(argv, argv + argc);
	t_application application(arguments);
	t_pointer<t_cairoc> frame = new t_cairoc();
	frame->f_caption__(L"Cairo");
	frame->f_move(t_rectangle(0, 0, 320, 240));
	application.f_add(frame);
	frame->f_show();
	application.f_run();
	return 0;
}
