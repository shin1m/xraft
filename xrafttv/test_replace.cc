#include <cstdio>
#include <cassert>

#include "text_model.h"

int main(int argc, char* argv[])
{
	using namespace std::literals;
	using namespace xraft;
	typedef t_text_model<int> t_model;
	t_model model;
	{
		assert(model.f_text_size() == 0);
		assert(model.f_segments_size() == 0);
	}
	{
		auto s = L"Hello, World!"sv;
		t_model::t_segment ss[] = {
			{5, 0},
			{2, 1},
			{5, 2},
			{1, 3},
		};
		model.f_replace(0, 0, &ss[0], &ss[4], s.begin());
		assert(model.f_text_size() == s.size());
		assert(*model.f_begin() == L'H');
		assert(*(model.f_begin() + s.size() - 1) == L'!');
		assert(model.f_segments_size() == 4);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 5);
		assert(model.f_segment_to_attribute(1) == 1);
		assert(model.f_segment_to_text(2) == 7);
		assert(model.f_segment_to_attribute(2) == 2);
		assert(model.f_segment_to_text(3) == 12);
		assert(model.f_segment_to_attribute(3) == 3);
	}
	// Hello, World!
	// 0000011222223
	//   |======|
	//  is ten years o
	// 444455544444442
	{
		auto s = L" is ten years o"sv;
		t_model::t_segment ss[] = {
			{4, 4},
			{3, 5},
			{7, 4},
			{1, 2},
		};
		model.f_replace(2, 8, &ss[0], &ss[4], s.begin());
		assert(model.f_text_size() == 20);
		assert(*(model.f_begin() + 6) == L't');
		assert(*(model.f_begin() + 7) == L'e');
		assert(*(model.f_begin() + 8) == L'n');
		assert(model.f_segments_size() == 6);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 2);
		assert(model.f_segment_to_attribute(1) == 4);
		assert(model.f_segment_to_text(2) == 6);
		assert(model.f_segment_to_attribute(2) == 5);
		assert(model.f_segment_to_text(3) == 9);
		assert(model.f_segment_to_attribute(3) == 4);
		assert(model.f_segment_to_text(4) == 16);
		assert(model.f_segment_to_attribute(4) == 2);
		assert(model.f_segment_to_text(5) == 19);
		assert(model.f_segment_to_attribute(5) == 3);
	}
	// He is ten years old!
	// 00444455544444442223
	{
		model.f_replace(0, model.f_text_size(), static_cast<t_model::t_segment*>(0), static_cast<t_model::t_segment*>(0), static_cast<wchar_t*>(0));
		assert(model.f_text_size() == 0);
		assert(model.f_segments_size() == 0);
	}
	{
		auto s = L"Hello, xem World!"sv;
		t_model::t_segment ss[] = {
			{7, 0},
			{3, 1},
			{7, 0},
		};
		model.f_replace(0, 0, &ss[0], &ss[3], s.begin());
		assert(model.f_text_size() == s.size());
		assert(model.f_segments_size() == 3);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 7);
		assert(model.f_segment_to_attribute(1) == 1);
		assert(model.f_segment_to_text(2) == 10);
		assert(model.f_segment_to_attribute(2) == 0);
	}
	// Hello, xem World!
	// 00000001110000000
	//        |=|
	{
		model.f_replace(7, 3, static_cast<t_model::t_segment*>(0), static_cast<t_model::t_segment*>(0), static_cast<wchar_t*>(0));
		assert(model.f_text_size() == 14);
for (size_t i = 0; i < model.f_segments_size(); ++i) std::fprintf(stderr, "(%d, %d)", model.f_segment_to_text(i), model.f_segment_to_attribute(i));
std::fprintf(stderr, "\n");
		assert(model.f_segments_size() == 1);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
	}
	// Hello,  World!
	// 00000000000000
	//        ^
	// xem
	// 111
	{
		auto s = L"xem"sv;
		t_model::t_segment ss[] = {
			{3, 1},
		};
		model.f_replace(7, 0, &ss[0], &ss[1], s.begin());
		assert(model.f_text_size() == 17);
for (size_t i = 0; i < model.f_segments_size(); ++i) std::fprintf(stderr, "(%d, %d)", model.f_segment_to_text(i), model.f_segment_to_attribute(i));
std::fprintf(stderr, "\n");
		assert(model.f_segments_size() == 3);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 7);
		assert(model.f_segment_to_attribute(1) == 1);
		assert(model.f_segment_to_text(2) == 10);
		assert(model.f_segment_to_attribute(2) == 0);
	}
	// Hello, xem World!
	// 00000001110000000
	//        |=|
	// xemmai
	// 111222
	{
		auto s = L"xemmai"sv;
		t_model::t_segment ss[] = {
			{3, 1},
			{3, 2},
		};
		model.f_replace(7, 3, &ss[0], &ss[2], s.begin());
		assert(model.f_text_size() == 20);
for (size_t i = 0; i < model.f_segments_size(); ++i) std::fprintf(stderr, "(%d, %d)", model.f_segment_to_text(i), model.f_segment_to_attribute(i));
std::fprintf(stderr, "\n");
		assert(model.f_segments_size() == 4);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 7);
		assert(model.f_segment_to_attribute(1) == 1);
		assert(model.f_segment_to_text(2) == 10);
		assert(model.f_segment_to_attribute(2) == 2);
		assert(model.f_segment_to_text(3) == 13);
		assert(model.f_segment_to_attribute(3) == 0);
	}
	// Hello, xemmai World!
	// 00000001112220000000
	return 0;
}
