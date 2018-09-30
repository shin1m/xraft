#include <cstdio>
#include <cassert>

#include "wrapped_view.h"

using namespace std::literals;

struct t_test_font
{
	size_t f_height() const
	{
		return 1;
	}
	size_t f_width() const
	{
		return 1;
	}
	size_t f_width(wchar_t a_c) const
	{
		return a_c < 256 ? 1 : 2;
	}
};

struct t_test_context
{
	size_t v_skip;
	std::wstring v_s;

	t_test_context(size_t a_skip = 0) : v_skip(a_skip)
	{
	}
	bool f_invalid(size_t a_y, size_t a_width, size_t a_height)
	{
		return v_skip <= 0 || (a_y + 1) % v_skip != 0;
	}
	void f_attribute(int a_attribute)
	{
		wchar_t cs[32];
		int n = std::swprintf(cs, 32, L"<attribute:%d>", a_attribute);
		v_s.insert(v_s.end(), cs, cs + n);
	}
	void f_draw_string(size_t a_x, size_t a_y, size_t a_width, const wchar_t* a_cs, size_t a_n)
	{
		v_s += L"<string:"sv;
		v_s.insert(v_s.end(), a_cs, a_cs + a_n);
		v_s += L'>';
	}
	void f_draw_tab(size_t a_x, size_t a_y, size_t a_width)
	{
		v_s += L"<tab>"sv;
	}
	void f_draw_line_terminator(size_t a_x, size_t a_y)
	{
		v_s += L"<terminator>"sv;
	}
	void f_draw_line_end(size_t a_x, size_t a_y, size_t a_width)
	{
		v_s += L"<line_end>\n"sv;
	}
	void f_draw_page_end(size_t a_y, size_t a_width, size_t a_height)
	{
		v_s += L"<page_end>"sv;
	}
	void f_draw_caret(size_t a_x, size_t a_y)
	{
		v_s += L"<caret>"sv;
	}
};

int main(int argc, char* argv[])
{
	using namespace xraft;
	typedef t_text_model<int> t_model;
	typedef t_wrapped_view<int, t_test_font> t_view;
	t_model model;
	t_view view(t_test_font {});
	view.f_model__(&model);
	{
		assert(model.f_text_size() == 0);
		assert(model.f_segments_size() == 0);
	}
	{
		auto s =
			L"Hello, World!\n"
			L"This\tis\tShin-ichi\tMORITA."sv;
		t_model::t_segment ss[] = {{s.size(), 0}};
		model.f_replace(0, 0, &ss[0], &ss[1], s.begin());
	}
	{
		model.f_enable_notification();
		t_test_context context;
		view.f_draw(context, 0, 100);
		std::fprintf(stderr, "%ls\n", context.v_s.c_str());
		assert(context.v_s ==
			L"<attribute:0><string:H><line_end>\n"
			L"<string:e><line_end>\n"
			L"<string:l><line_end>\n"
			L"<string:l><line_end>\n"
			L"<string:o><line_end>\n"
			L"<string:,><line_end>\n"
			L"<string: ><line_end>\n"
			L"<string:W><line_end>\n"
			L"<string:o><line_end>\n"
			L"<string:r><line_end>\n"
			L"<string:l><line_end>\n"
			L"<string:d><line_end>\n"
			L"<string:!><line_end>\n"
			L"<terminator><line_end>\n"
			L"<string:T><line_end>\n"
			L"<string:h><line_end>\n"
			L"<string:i><line_end>\n"
			L"<string:s><line_end>\n"
			L"<tab><line_end>\n"
			L"<string:i><line_end>\n"
			L"<string:s><line_end>\n"
			L"<tab><line_end>\n"
			L"<string:S><line_end>\n"
			L"<string:h><line_end>\n"
			L"<string:i><line_end>\n"
			L"<string:n><line_end>\n"
			L"<string:-><line_end>\n"
			L"<string:i><line_end>\n"
			L"<string:c><line_end>\n"
			L"<string:h><line_end>\n"
			L"<string:i><line_end>\n"
			L"<tab><line_end>\n"
			L"<string:M><line_end>\n"
			L"<string:O><line_end>\n"
			L"<string:R><line_end>\n"
			L"<string:I><line_end>\n"
			L"<string:T><line_end>\n"
			L"<string:A><line_end>\n"
			L"<string:.><line_end>\n"
			L"<page_end>"sv
		);
	}
	{
		view.f_width__(10);
		t_test_context context;
		view.f_draw(context, 0, 100);
		std::fprintf(stderr, "%ls\n", context.v_s.c_str());
		assert(context.v_s ==
			L"<attribute:0><string:Hello, Wor><line_end>\n"
			L"<string:ld!><terminator><line_end>\n"
			L"<string:This><tab><string:is><line_end>\n"
			L"<tab><string:Shin-i><line_end>\n"
			L"<string:chi><tab><string:MORITA><line_end>\n"
			L"<string:.><line_end>\n"
			L"<page_end>"sv
		);
	}
	{
		t_test_context context(3);
		view.f_draw(context, 1, 5);
		std::fprintf(stderr, "%ls\n", context.v_s.c_str());
		assert(context.v_s ==
			L"<attribute:0><string:ld!><terminator><line_end>\n"
			L"<tab><string:Shin-i><line_end>\n"
			L"<string:chi><tab><string:MORITA><line_end>\n"sv
		);
	}
	{
		auto s = L" is ten years o"sv;
		t_model::t_segment ss[] = {{s.size(), 0}};
		model.f_replace(2, 8, &ss[0], &ss[1], s.begin());
	}
	{
		auto s = L"\nGood bye.\n"sv;
		t_model::t_segment ss[] = {{s.size(), 0}};
		model.f_replace(model.f_text_size(), 0, &ss[0], &ss[1], s.begin());
	}
	{
		t_test_context context;
		view.f_draw(context, 0, 100);
		std::fprintf(stderr, "%ls\n", context.v_s.c_str());
		assert(context.v_s ==
			L"<attribute:0><string:He is ten ><line_end>\n"
			L"<string:years old!><line_end>\n"
			L"<terminator><line_end>\n"
			L"<string:This><tab><string:is><line_end>\n"
			L"<tab><string:Shin-i><line_end>\n"
			L"<string:chi><tab><string:MORITA><line_end>\n"
			L"<string:.><terminator><line_end>\n"
			L"<string:Good bye.><terminator><line_end>\n"
			L"<line_end>\n"
			L"<page_end>"sv
		);
	}
	return 0;
}
