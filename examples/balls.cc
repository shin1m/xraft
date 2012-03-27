#include <deque>
#include <vector>
#include <cmath>
#include <xraft/application.h>
#include <xraft/graphics.h>

using namespace xraft;

struct t_ball
{
	t_pointer<t_pixmap> v_image;
	double v_r;
	double v_x;
	double v_y;
	double v_vx;
	double v_vy;

	t_ball(const t_pointer<t_pixmap>& a_image, double a_x, double a_y, double a_vx, double a_vy) :
	v_image(a_image),
	v_r(v_image->f_width() * 0.5),
	v_x(a_x), v_y(a_y), v_vx(a_vx), v_vy(a_vy)
	{
	}
	double f_move(const t_extent& a_extent)
	{
		v_x += v_vx;
		v_y += v_vy;
		if (v_x < v_r)
			v_vx = std::fabs(v_vx);
		else if (v_x > a_extent.v_width - v_r)
			v_vx = -std::fabs(v_vx);
		double y = a_extent.v_height - v_r - v_y;
		if (y > 0.0)
			v_vy += 1.0;
		else
			v_vy = -std::fabs(v_vy);
		return (v_vx * v_vx + v_vy * v_vy) * 0.5 + y;
	}
	void f_impact(t_ball& a_ball)
	{
		double dx = a_ball.v_x - v_x;
		double dy = a_ball.v_y - v_y;
		double l = std::sqrt(dx * dx + dy * dy);
		if (l >= v_r + a_ball.v_r) return;
		if (l > 0.0) {
			dx /= l;
			dy /= l;
		}
		double v1 = v_vx * dx + v_vy * dy;
		double w1 = v_vy * dx - v_vx * dy;
		double v2 = a_ball.v_vx * dx + a_ball.v_vy * dy;
		double w2 = a_ball.v_vy * dx - a_ball.v_vx * dy;
		if (v2 - v1 < 0.0) {
			//double vv1 = v2;
			//double vv2 = v1;
			double vv1 = v2 * 0.999;
			double vv2 = v1 * 0.999;
			v_vx = vv1 * dx - w1 * dy;
			v_vy = w1 * dx + vv1 * dy;
			a_ball.v_vx = vv2 * dx - w2 * dy;
			a_ball.v_vy = w2 * dx + vv2 * dy;
		}
	}
};

class t_balls : public t_frame
{
	struct t_stepper : t_timer
	{
		t_balls* v_balls;

		t_stepper(t_balls* a_balls) : v_balls(a_balls)
		{
		}
		virtual void operator()()
		{
			v_balls->f_step();
		}
	};
	struct t_remover : t_timer
	{
		t_balls* v_balls;

		t_remover(t_balls* a_balls) : v_balls(a_balls)
		{
		}
		virtual void operator()()
		{
			v_balls->f_remove();
		}
	};

	t_color v_foreground;
	t_color v_background;
	t_pointer<t_pixmap> v_ball;
	t_pointer<t_bitmap> v_mask;
	std::deque<t_ball> v_balls;
	double v_energy;
	t_point v_cursor;
	bool v_double_bufferred;
	t_pointer<t_pixmap> v_pixmap;
	t_pointer<t_timer> v_stepper;
	t_pointer<t_timer> v_remover;

	void f_invalidate_all()
	{
		t_extent extent = f_geometry();
		f_invalidate(0, 0, extent.v_width, extent.v_height);
	}
	void f_step()
	{
		size_t n = v_balls.size();
		if (n > 0)
			for (size_t i = 0; i < n - 1; ++i)
				for (size_t j = i + 1; j < n; ++j)
					v_balls[i].f_impact(v_balls[j]);
		t_extent extent = f_geometry();
		v_energy = 0.0;
		for (size_t i = 0; i < n; ++i) v_energy += v_balls[i].f_move(extent);
		f_invalidate_all();
	}
	void f_remove()
	{
		if (v_balls.empty()) return;
		v_balls.pop_front();
		f_invalidate_all();
	}
	void f_append()
	{
		if (v_cursor.v_x < 0) return;
		v_balls.push_back(t_ball(v_ball, v_cursor.v_x, v_cursor.v_y, 0.0, 0.0));
		f_invalidate_all();
	}
	virtual void f_on_move()
	{
		t_extent extent = f_geometry();
		v_pixmap = new t_pixmap(extent.v_width, extent.v_height);
	}
	template<typename T>
	void f_draw(t_graphics& a_g, int a_x, int a_y, const wchar_t* a_format, T a_value)
	{
		wchar_t cs[1024];
		int n = swprintf(cs, sizeof(cs) / sizeof(wchar_t), a_format, a_value);
		a_g.f_draw(a_x, a_y, cs, n);
	}
	void f_paint(t_graphics& a_g)
	{
		t_extent extent = f_geometry();
		unsigned ew = extent.v_width;
		unsigned eh = extent.v_height;
		//wall = $wall;
		//ww = wall.width();
		//wh = wall.height();
		//x = (ew - ww) / 2;
		//y = (eh - wh) / 2;
		a_g.f_color(v_background.f_pixel());
		//g.fill(0, 0, ew, y);
		//g.fill(0, y, x, wh);
		//g.fill(x + ww, y, ew - x - ww, wh);
		//g.fill(0, y + wh, ew, eh - y - wh);
		//g.draw(x, y, wall, 0, 0, ww, wh);
		a_g.f_fill(0, 0, ew, eh);
		t_region region;
		region.f_unite(region, 0, 0, ew / 2 - 8, eh / 2 - 8);
		region.f_unite(region, ew / 2 + 8, 0, ew / 2 - 8, eh / 2 - 8);
		region.f_unite(region, 0, eh / 2 + 8, ew / 2 - 8, eh / 2 - 8);
		region.f_unite(region, ew / 2 + 8, eh / 2 + 8, ew / 2 - 8, eh / 2 - 8);
		region.f_intersect(region, a_g.f_region());
		a_g.f_clip(region);
		for (size_t i = 0; i < v_balls.size(); ++i) {
			const t_ball& ball = v_balls[i];
			a_g.f_draw(static_cast<int>(ball.v_x - ball.v_r), static_cast<int>(ball.v_y - ball.v_r), v_ball, 0, 0, v_ball->f_width(), v_ball->f_height(), v_mask);
		}
		a_g.f_clip(region);
		const t_pointer<t_font>& font = f_application()->f_font();
		a_g.f_color(v_foreground.f_pixel());
		a_g.f_font(font);
		unsigned h = font->f_height();
		f_draw(a_g, 0, h, L"Number of Balls: %d", v_balls.size());
		f_draw(a_g, 0, h * 2, L"Kinetic and Potential Energy: %g", v_energy);
		f_draw(a_g, 0, h * 3, L"Double Bufferred: %ls", v_double_bufferred ? L"true" : L"false");
		if (v_cursor.v_x < 0) return;
		unsigned r = v_ball->f_width() / 2;
		a_g.f_draw(v_cursor.v_x - r, v_cursor.v_y - r, v_ball->f_width(), v_ball->f_height());
	}
	virtual void f_on_paint(t_graphics& a_g)
	{
		if (v_double_bufferred) {
			{
				t_graphics g(v_pixmap);
				f_paint(g);
			}
			a_g.f_draw(0, 0, v_pixmap, 0, 0, v_pixmap->f_width(), v_pixmap->f_height());
		} else {
			f_paint(a_g);
		}
	}
	virtual void f_on_key_press(t_modifier a_modifier, t_key a_key, char a_ascii)
	{
		if (a_key == e_key__SPACE) {
			v_double_bufferred ^= true;
			f_invalidate_all();
		} else if (a_key == e_key__Q) {
			f_on_close();
		}
	}
	virtual void f_on_button_press(t_modifier a_modifier, t_button a_button, int a_x, int a_y)
	{
		if (a_button == e_button__1) {
			f_append();
		} else if (a_button == e_button__3) {
			v_balls.clear();
			f_invalidate_all();
		}
	}
	virtual void f_on_pointer_enter(t_modifier a_modifier, int a_x, int a_y, t_cross_mode a_mode, t_cross_detail a_detail)
	{
		v_cursor = t_point(a_x, a_y);
		f_invalidate_all();
	}
	virtual void f_on_pointer_leave(t_modifier a_modifier, int a_x, int a_y, t_cross_mode a_mode, t_cross_detail a_detail)
	{
		v_cursor.v_x = -1;
		f_invalidate_all();
	}
	virtual void f_on_pointer_move(t_modifier a_modifier, int a_x, int a_y)
	{
		v_cursor = t_point(a_x, a_y);
		f_invalidate_all();
	}
	virtual void f_on_close()
	{
		f_application()->f_exit();
	}

public:
	t_balls(const t_color& a_foreground, const t_color& a_background, const t_pointer<t_pixmap>& a_ball, const t_pointer<t_bitmap>& a_mask) :
	v_foreground(a_foreground), v_background(a_background),
	v_ball(a_ball), v_mask(a_mask),
	v_energy(0.0),
	v_cursor(-1, -1),
	v_double_bufferred(true)
	{
		v_stepper = new t_stepper(this);
		v_remover = new t_remover(this);
		v_stepper->f_start(30);
		v_remover->f_start(3000);
	}
};

void f_ball_data(unsigned a_size, double a_cx, double a_cy, double a_radius, std::vector<char>& a_data)
{
	a_data.resize(3 * a_size * a_size);
	for (unsigned i = 0; i < a_size; ++i) {
		for (unsigned j = 0; j < a_size; ++j) {
			double dx = j - a_cx;
			double dy = i - a_cy;
			double t = std::sqrt(dx * dx + dy * dy) / a_radius;
			a_data[(i * a_size + j) * 3] = static_cast<char>((1.0 - t) * 160.0 + t * 0.0);
			a_data[(i * a_size + j) * 3 + 1] = static_cast<char>((1.0 - t) * 192.0 + t * 0.0);
			a_data[(i * a_size + j) * 3 + 2] = static_cast<char>((1.0 - t) * 255.0 + t * 32.0);
		}
	}
}

void f_mask_data(unsigned a_size, std::vector<char>& a_data)
{
	double c = a_size * 0.5;
	double d = c * c;
	unsigned bpl = (a_size + 7) / 8;
	a_data.resize(bpl * 8 * a_size);
	for (unsigned i = 0; i < a_size; ++i) {
		for (unsigned j = 0; j < a_size; ++j) {
			double dx = j - c;
			double dy = i - c;
			unsigned k = i * bpl + j / 8;
			unsigned b = 128 >> j % 8;
			if (dx * dx + dy * dy > d)
				a_data[k] = a_data[k] & ~b;
			else
				a_data[k] = a_data[k] | b;
		}
	}
}

int main(int argc, char* argv[])
{
	std::vector<std::string> arguments(argv, argv + argc);
	t_application application(arguments);
	//base = path.dirname(sys.argv[0])
	//image = Image.open(path.join(base, 'lena.jpg')).convert('RGB')
	//wall = xraft.t_pixmap(image.size[0], image.size[1], image.tostring())
	std::vector<char> data;
	f_ball_data(32, 12.0, 12.0, 24.0, data);
	t_pointer<t_pixmap> ball = new t_pixmap(32, 32, false, &data[0]);
	f_mask_data(32, data);
	t_pointer<t_bitmap> mask = new t_bitmap(32, 32, &data[0]);
	t_pointer<t_balls> frame = new t_balls(t_color(0, 0, 255), t_color(255, 255, 255), ball, mask);
	frame->f_caption__(L"Balls");
	frame->f_move(t_rectangle(0, 0, 320, 240));
	application.f_add(frame);
	frame->f_show();
	application.f_run();
	return 0;
}

#ifdef XRAFT_WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return main(__argc, __argv);
}
#endif
