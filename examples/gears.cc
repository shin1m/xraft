#include <cmath>
#include <xraft/graphics.h>
#include <xraft/opengl.h>
#include <GL/gl.h>

using namespace xraft;

class t_label : public t_widget
{
	const t_color& v_foreground;
	const t_color& v_background;
	std::wstring v_text;

protected:
	virtual void f_on_paint(t_graphics& a_g);

public:
	t_label(const t_color& a_foreground, const t_color& a_background, const std::wstring& a_text) : v_foreground(a_foreground), v_background(a_background), v_text(a_text)
	{
	}
};

void t_label::f_on_paint(t_graphics& a_g)
{
	const t_extent& extent = f_geometry();
	a_g.f_color(v_background.f_pixel());
	a_g.f_fill(0, 0, extent.v_width, extent.v_height);
	const t_pointer<t_font>& font = f_application()->f_font();
	int x = (extent.v_width - font->f_width(v_text.c_str(), v_text.size())) / 2;
	int y = (extent.v_height + font->f_ascent()) / 2;
	a_g.f_color(v_foreground.f_pixel());
	a_g.f_draw(x, y, v_text.c_str(), v_text.size());
}

class t_gears : public t_opengl_widget
{
	t_pointer<t_opengl_context> v_context;
	double v_rotate_x;
	double v_rotate_y;
	double v_rotate_z;
	double v_angle;
	int v_gear1;
	int v_gear2;
	int v_gear3;

	void f_gear(double a_inner, double a_outer, double a_width, size_t a_teeth, double a_depth);

protected:
	virtual void f_on_create();
	virtual void f_on_destroy();
	virtual void f_on_move();
	virtual void f_on_paint(t_graphics& a_g);
	virtual void f_on_key_press(t_modifier a_modifier, t_key a_key, char a_ascii);

public:
	t_gears(const t_pointer<t_opengl_format>& a_format) : t_opengl_widget(a_format), v_context(new t_opengl_context(a_format)), v_rotate_x(20.0), v_rotate_y(30.0), v_rotate_z(0.0), v_angle(0.0), v_gear1(0), v_gear2(0), v_gear3(0)
	{
	}
};

void t_gears::f_gear(double a_inner, double a_outer, double a_width, size_t a_teeth, double a_depth)
{
	double r0 = a_inner;
	double r1 = a_outer - a_depth / 2.0;
	double r2 = a_outer + a_depth / 2.0;
	double da = 2.0 * M_PI / a_teeth / 4.0;
	glShadeModel(GL_FLAT);
	glNormal3f(0.0, 0.0, 1.0);
	glBegin(GL_QUAD_STRIP);
	for (size_t i = 0; i <= a_teeth; ++i) {
		double angle = i * 2.0 * M_PI / a_teeth;
		glVertex3f(r0 * std::cos(angle), r0 * std::sin(angle), a_width * 0.5);
		glVertex3f(r1 * std::cos(angle), r1 * std::sin(angle), a_width * 0.5);
		glVertex3f(r0 * std::cos(angle), r0 * std::sin(angle), a_width * 0.5);
		glVertex3f(r1 * std::cos(angle + 3 * da), r1 * std::sin(angle + 3 * da), a_width * 0.5);
	}
	glEnd();
	glBegin(GL_QUADS);
	for (size_t i = 0; i < a_teeth; ++i) {
		double angle = i * 2.0 * M_PI / a_teeth;
		glVertex3f(r1 * std::cos(angle), r1 * std::sin(angle), a_width * 0.5);
		glVertex3f(r2 * std::cos(angle + da), r2 * std::sin(angle + da), a_width * 0.5);
		glVertex3f(r2 * std::cos(angle + 2 * da), r2 * std::sin(angle + 2 * da), a_width * 0.5);
		glVertex3f(r1 * std::cos(angle + 3 * da), r1 * std::sin(angle + 3 * da), a_width * 0.5);
	}
	glEnd();
	glNormal3f(0.0, 0.0, -1.0);
	glBegin(GL_QUAD_STRIP);
	for (size_t i = 0; i <= a_teeth; ++i) {
		double angle = i * 2.0 * M_PI / a_teeth;
		glVertex3f(r1 * std::cos(angle), r1 * std::sin(angle), -a_width * 0.5);
		glVertex3f(r0 * std::cos(angle), r0 * std::sin(angle), -a_width * 0.5);
		glVertex3f(r1 * std::cos(angle + 3 * da), r1 * std::sin(angle + 3 * da), -a_width * 0.5);
		glVertex3f(r0 * std::cos(angle), r0 * std::sin(angle), -a_width * 0.5);
	}
	glEnd();
	glBegin(GL_QUADS);
	for (size_t i = 0; i < a_teeth; ++i) {
		double angle = i * 2.0 * M_PI / a_teeth;
		glVertex3f(r1 * std::cos(angle + 3 * da), r1 * std::sin(angle + 3 * da), -a_width * 0.5);
		glVertex3f(r2 * std::cos(angle + 2 * da), r2 * std::sin(angle + 2 * da), -a_width * 0.5);
		glVertex3f(r2 * std::cos(angle + da), r2 * std::sin(angle + da), -a_width * 0.5);
		glVertex3f(r1 * std::cos(angle), r1 * std::sin(angle), -a_width * 0.5);
	}
	glEnd();

	glBegin(GL_QUAD_STRIP);
	for (size_t i = 0; i < a_teeth; ++i) {
		double angle = i * 2.0 * M_PI / a_teeth;
		glVertex3f(r1 * std::cos(angle), r1 * std::sin(angle), a_width * 0.5);
		glVertex3f(r1 * std::cos(angle), r1 * std::sin(angle), -a_width * 0.5);
		double u = r2 * std::cos(angle + da) - r1 * std::cos(angle);
		double v = r2 * std::sin(angle + da) - r1 * std::sin(angle);
		double length = std::sqrt(u * u + v * v);
		u /= length;
		v /= length;
		glNormal3f(v, -u, 0.0);
		glVertex3f(r2 * std::cos(angle + da), r2 * std::sin(angle + da), a_width * 0.5);
		glVertex3f(r2 * std::cos(angle + da), r2 * std::sin(angle + da), -a_width * 0.5);
		glNormal3f(std::cos(angle), std::sin(angle), 0.0);
		glVertex3f(r2 * std::cos(angle + 2 * da), r2 * std::sin(angle + 2 * da), a_width * 0.5);
		glVertex3f(r2 * std::cos(angle + 2 * da), r2 * std::sin(angle + 2 * da), -a_width * 0.5);
		u = r1 * std::cos(angle + 3 * da) - r2 * std::cos(angle + 2 * da);
		v = r1 * std::sin(angle + 3 * da) - r2 * std::sin(angle + 2 * da);
		glNormal3f(v, -u, 0.0);
		glVertex3f(r1 * std::cos(angle + 3 * da), r1 * std::sin(angle + 3 * da),  a_width*0.5);
		glVertex3f(r1 * std::cos(angle + 3 * da), r1 * std::sin(angle + 3 * da), -a_width*0.5);
		glNormal3f(std::cos(angle), std::sin(angle), 0.0);
	}
	glVertex3f(r1 * std::cos(0.0), r1 * std::sin(0.0), a_width * 0.5);
	glVertex3f(r1 * std::cos(0.0), r1 * std::sin(0.0), -a_width * 0.5);
	glEnd();
	glShadeModel(GL_SMOOTH);
	glBegin(GL_QUAD_STRIP);
	for (size_t i = 0; i <= a_teeth; ++i) {
		double angle = i * 2.0 * M_PI / a_teeth;
		glNormal3f(-std::cos(angle), -std::sin(angle), 0.0);
		glVertex3f(r0 * std::cos(angle), r0 * std::sin(angle), -a_width * 0.5);
		glVertex3f(r0 * std::cos(angle), r0 * std::sin(angle), a_width * 0.5);
	}
	glEnd();
}

void t_gears::f_on_create()
{
	v_context->f_make_current(this);
	GLfloat ds0[] = {5.0, 5.0, 10.0, 1.0};
	glLightfv(GL_LIGHT0, GL_POSITION, ds0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);

	v_gear1 = glGenLists(1);
	glNewList(v_gear1, GL_COMPILE);
	GLfloat ds1[] = {0.8, 0.1, 0.0, 1.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ds1);
	f_gear(1.0, 4.0, 1.0, 20, 0.7);
	glEndList();

	v_gear2 = glGenLists(1);
	glNewList(v_gear2, GL_COMPILE);
	GLfloat ds2[] = {0.0, 0.8, 0.2, 1.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ds2);
	f_gear(0.5, 2.0, 2.0, 10, 0.7);
	glEndList();

	v_gear3 = glGenLists(1);
	glNewList(v_gear3, GL_COMPILE);
	GLfloat ds3[] = {0.2, 0.2, 1.0, 1.0};
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, ds3);
	f_gear(1.3, 2.0, 0.5, 10, 0.7);
	glEndList();

	glEnable(GL_NORMALIZE);

	f_on_move();
}

void t_gears::f_on_destroy()
{
	v_context->f_make_current(this);
	glDeleteLists(v_gear1, 1);
	glDeleteLists(v_gear2, 1);
	glDeleteLists(v_gear3, 1);
}

void t_gears::f_on_move()
{
	if (!f_created()) return;
	v_context->f_make_current(this);
	const t_extent& extent = f_geometry();
	double w = static_cast<double>(extent.v_width) / extent.v_height;
	double h = 1.0;
	glViewport(0, 0, extent.v_width, extent.v_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustum(-w, w, -h, h, 5.0, 60.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -40.0);
}

void t_gears::f_on_paint(t_graphics& a_g)
{
	v_context->f_make_current(this);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPushMatrix();
	glRotatef(v_rotate_x, 1.0, 0.0, 0.0);
	glRotatef(v_rotate_y, 0.0, 1.0, 0.0);
	glRotatef(v_rotate_z, 0.0, 0.0, 1.0);

	glPushMatrix();
	glTranslatef(-3.0, -2.0, 0.0);
	glRotatef(v_angle, 0.0, 0.0, 1.0);
	glCallList(v_gear1);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(3.1, -2.0, 0.0);
	glRotatef(-2.0 * v_angle - 9.0, 0.0, 0.0, 1.0);
	glCallList(v_gear2);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(-3.1, 2.2, -1.8);
	glRotatef(90.0, 1.0, 0.0, 0.0);
	glRotatef(2.0 * v_angle - 2.0, 0.0, 0.0, 1.0);
	glCallList(v_gear3);
	glPopMatrix();

	glPopMatrix();
	v_context->f_flush();
}

void t_gears::f_on_key_press(t_modifier a_modifier, t_key a_key, char a_ascii)
{
	v_angle += 2.0;
	v_rotate_y += 1.0;
	f_invalidate(0, 0, f_geometry().v_width, f_geometry().v_height);
}

class t_test_frame : public t_frame
{
protected:
	virtual void f_on_move();
	virtual void f_on_focus_enter();
	virtual void f_on_close();
};

void t_test_frame::f_on_move()
{
	const t_extent& extent = f_geometry();
	const t_pointer<t_font>& font = f_application()->f_font();
	f_at(0)->f_move(t_rectangle(0, 0, extent.v_width, font->f_height()));
	f_at(1)->f_move(t_rectangle(0, font->f_height(), extent.v_width, extent.v_height - font->f_height() * 2));
	f_at(2)->f_move(t_rectangle(0, extent.v_height - font->f_height(), extent.v_width, font->f_height()));
}

void t_test_frame::f_on_focus_enter()
{
	f_application()->f_focus__(f_at(1));
}

void t_test_frame::f_on_close()
{
	f_application()->f_exit();
}

int main(int argc, char* argv[])
{
	std::vector<std::string> arguments(argv, argv + argc);
	t_application application(arguments);
	t_color blue(0, 0, 255);
	t_color white(255, 255, 255);
	t_pointer<t_test_frame> frame = new t_test_frame();
	frame->f_caption__(L"test");
	frame->f_add(new t_label(blue, white, L"Hello, World!!"));
	frame->f_add(new t_gears(new t_opengl_format(true, true, false, true)));
	frame->f_add(new t_label(white, blue, L"OpenGL Gears"));
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
