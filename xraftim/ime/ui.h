#ifndef UI_H
#define UI_H

#include <vector>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
#include <immdev.h>

extern HINSTANCE v_hinstance;

class t_ui;

class t_component
{
	static LRESULT CALLBACK f_process(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	HWND v_hwnd;

	virtual void f_on_create(HWND a_parent, int a_x, int a_y, size_t a_width, size_t a_height);
	virtual void f_on_paint(HDC a_hdc) = 0;

public:
	static LPCTSTR f_class()
	{
		return TEXT("xraftim::t_component");
	}
	static BOOL f_register();
	static BOOL f_unregister()
	{
		return UnregisterClass(f_class(), v_hinstance);
	}

	t_component() : v_hwnd(NULL)
	{
	}
	void f_show(t_ui* a_ui, int a_x, int a_y, size_t a_width, size_t a_height);
	void f_hide();
};

class t_composition : public t_component
{
	friend class t_ui;

	size_t v_first;
	size_t v_last;

protected:
	virtual void f_on_paint(HDC a_hdc);

public:
	t_composition() : v_first(0), v_last(1)
	{
	}
};

class t_candidates : public t_component
{
protected:
	virtual void f_on_paint(HDC a_hdc);
};

class t_guide : public t_component
{
protected:
	virtual void f_on_create(HWND a_parent, int a_x, int a_y, size_t a_width, size_t a_height);
	virtual void f_on_paint(HDC a_hdc);
};

class t_ui
{
	friend class t_component;
	friend class t_composition;
	friend class t_guide;

	struct t_line
	{
		size_t v_i;
		size_t v_j;
		int v_x;

		t_line(size_t a_i, int a_x) : v_i(a_i), v_x(a_x)
		{
		}
	};

	static LRESULT CALLBACK f_process(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND v_hwnd;
	HFONT v_font;
	bool v_active;
	bool v_composing;
	std::vector<t_line> v_lines;
	t_composition v_first;
	t_composition v_middle;
	t_composition v_last;
	bool v_choosing;
	t_candidates v_candidates;
	t_guide v_guide;

	POINT f_spot(LONG a_width, LONG a_height) const;

public:
	static LPCTSTR f_class()
	{
		return TEXT("xraftim::t_ui");
	}
	static BOOL f_register();
	static BOOL f_unregister()
	{
		return UnregisterClass(f_class(), v_hinstance);
	}

	t_ui(HWND a_hwnd) : v_hwnd(a_hwnd), v_font(NULL), v_active(false), v_composing(false), v_choosing(false)
	{
	}
	~t_ui()
	{
		f_clear();
		v_candidates.f_hide();
		v_guide.f_hide();
		if (v_font != NULL) DeleteObject(v_font);
	}
	void f_font();
	void f_composition();
	void f_clear()
	{
		v_first.f_hide();
		v_middle.f_hide();
		v_last.f_hide();
	}
	void f_candidates();
	void f_guide();
};

inline void f_log(LPCSTR a_message)
{
#ifdef _DEBUG
	char cs[1024];
	std::sprintf(cs, "%s\n", a_message);
	OutputDebugStringA(cs);
#endif
}

#endif
