#include <clocale>
#include <cstdio>
#include <xraft/application.h>
#include <xraft/graphics.h>

using namespace xraft;

class t_ibus : public t_frame
{
	t_color v_foreground;
	t_color v_background;
	std::vector<std::wstring> v_texts;

	virtual void f_on_paint(t_graphics& a_g)
	{
		t_extent extent = f_geometry();
		a_g.f_color(v_background.f_pixel());
		a_g.f_fill(0, 0, extent.v_width, extent.v_height);
		a_g.f_color(v_foreground.f_pixel());
		t_pointer<t_font> font = f_application()->f_font();
		int y = font->f_ascent();
		for (const auto& x : v_texts) {
			a_g.f_draw(0, y, x.c_str(), x.size());
			y += font->f_height();
		}
	}
	virtual void f_on_key_press(t_modifier a_modifier, t_key a_key, char a_ascii)
	{
		if (a_key == e_key__Q) f_on_close();
	}
	virtual void f_on_input_commit(const wchar_t* a_cs, size_t a_n)
	{
		v_texts.push_back(std::wstring(a_cs, a_cs + a_n));
		t_extent extent = f_geometry();
		f_invalidate(0, 0, extent.v_width, extent.v_height);
	}
	virtual t_rectangle f_on_input_spot()
	{
		t_pointer<t_font> font = f_application()->f_font();
		return t_rectangle(0, v_texts.size() * font->f_height(), 0, font->f_height());
	}
	virtual void f_on_close()
	{
		f_application()->f_exit();
	}

public:
	t_ibus() : v_foreground("black"), v_background("white")
	{
		f_input_context__(f_application()->f_input_context());
	}
};

int main(int argc, char* argv[])
{
	std::setlocale(LC_ALL, "");
	std::vector<std::string> arguments(argv, argv + argc);
	t_application application(arguments);
	t_pointer<t_ibus> frame = new t_ibus();
	frame->f_caption__(L"iBus");
	frame->f_move(t_rectangle(0, 0, 320, 240));
	application.f_add(frame);
	frame->f_show();
	application.f_run();
	return 0;
}
