#include <cstdio>
#include <cassert>

#include "text_model.h"

int main(int argc, char* argv[])
{
	using namespace xraft;
	typedef t_text_model<int> t_model;
	t_model model;
	{
		assert(model.f_text_size() == 0);
		assert(model.f_segments_size() == 0);
	}
	{
		std::wstring s = L"Hello, World!";
		t_model::t_segment ss[] = {{s.size(), 0}};
		model.f_replace(0, 0, &ss[0], &ss[1], s.begin());
		assert(model.f_text_size() == s.size());
		assert(*model.f_begin() == L'H');
		assert(*(model.f_begin() + s.size() - 1) == L'!');
		assert(model.f_segments_size() == 1);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
	}
	// Hello, World!
	// 0000000000000
	//      11111111
	{
		model.f_attribute(5, 8, 1);
		assert(model.f_segments_size() == 2);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 5);
		assert(model.f_segment_to_attribute(1) == 1);
	}
	// Hello, World!
	// 0000011111111
	//      00000000
	{
		model.f_attribute(5, 8, 0);
		assert(model.f_segments_size() == 1);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
	}
	// Hello, World!
	// 0000000000000
	// 11111
	{
		model.f_attribute(0, 5, 1);
		assert(model.f_segments_size() == 2);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 1);
		assert(model.f_segment_to_text(1) == 5);
		assert(model.f_segment_to_attribute(1) == 0);
	}
	// Hello, World!
	// 1111100000000
	// 00000
	{
		model.f_attribute(0, 5, 0);
		assert(model.f_segments_size() == 1);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
	}
	// Hello, World!
	// 0000000000000
	//      1111
	{
		model.f_attribute(5, 4, 1);
		assert(model.f_segments_size() == 3);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 5);
		assert(model.f_segment_to_attribute(1) == 1);
		assert(model.f_segment_to_text(2) == 9);
		assert(model.f_segment_to_attribute(2) == 0);
	}
	// Hello, World!
	// 0000011110000
	//          000
	{
		model.f_attribute(9, 3, 0);
		assert(model.f_segments_size() == 3);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 5);
		assert(model.f_segment_to_attribute(1) == 1);
		assert(model.f_segment_to_text(2) == 9);
		assert(model.f_segment_to_attribute(2) == 0);
	}
	// Hello, World!
	// 0000011110000
	//           000
	{
		model.f_attribute(10, 3, 0);
		assert(model.f_segments_size() == 3);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 5);
		assert(model.f_segment_to_attribute(1) == 1);
		assert(model.f_segment_to_text(2) == 9);
		assert(model.f_segment_to_attribute(2) == 0);
	}
	// Hello, World!
	// 0000011110000
	//      111
	{
		model.f_attribute(5, 3, 1);
		assert(model.f_segments_size() == 3);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 5);
		assert(model.f_segment_to_attribute(1) == 1);
		assert(model.f_segment_to_text(2) == 9);
		assert(model.f_segment_to_attribute(2) == 0);
	}
	// Hello, World!
	// 0000011110000
	//       111
	{
		model.f_attribute(6, 3, 1);
		assert(model.f_segments_size() == 3);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 5);
		assert(model.f_segment_to_attribute(1) == 1);
		assert(model.f_segment_to_text(2) == 9);
		assert(model.f_segment_to_attribute(2) == 0);
	}
	// Hello, World!
	// 0000011110000
	//       1111
	{
		model.f_attribute(6, 4, 1);
		assert(model.f_segments_size() == 3);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 5);
		assert(model.f_segment_to_attribute(1) == 1);
		assert(model.f_segment_to_text(2) == 10);
		assert(model.f_segment_to_attribute(2) == 0);
	}
	// Hello, World!
	// 0000011111000
	//     1111111
	{
		model.f_attribute(4, 7, 1);
		assert(model.f_segments_size() == 3);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 4);
		assert(model.f_segment_to_attribute(1) == 1);
		assert(model.f_segment_to_text(2) == 11);
		assert(model.f_segment_to_attribute(2) == 0);
	}
	// Hello, World!
	// 0000111111100
	//     00000000
	{
		model.f_attribute(4, 8, 0);
		assert(model.f_segments_size() == 1);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
	}
	// Hello, World!
	// 0000000000000
	//    1111111
	{
		model.f_attribute(3, 7, 1);
		assert(model.f_segments_size() == 3);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 3);
		assert(model.f_segment_to_attribute(1) == 1);
		assert(model.f_segment_to_text(2) == 10);
		assert(model.f_segment_to_attribute(2) == 0);
	}
	// Hello, World!
	// 0001111111000
	//      222
	{
		model.f_attribute(5, 3, 2);
		assert(model.f_segments_size() == 5);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 3);
		assert(model.f_segment_to_attribute(1) == 1);
		assert(model.f_segment_to_text(2) == 5);
		assert(model.f_segment_to_attribute(2) == 2);
		assert(model.f_segment_to_text(3) == 8);
		assert(model.f_segment_to_attribute(3) == 1);
		assert(model.f_segment_to_text(4) == 10);
		assert(model.f_segment_to_attribute(4) == 0);
	}
	// Hello, World!
	// 0001122211000
	//      22
	{
		model.f_attribute(5, 2, 2);
		assert(model.f_segments_size() == 5);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 3);
		assert(model.f_segment_to_attribute(1) == 1);
		assert(model.f_segment_to_text(2) == 5);
		assert(model.f_segment_to_attribute(2) == 2);
		assert(model.f_segment_to_text(3) == 8);
		assert(model.f_segment_to_attribute(3) == 1);
		assert(model.f_segment_to_text(4) == 10);
		assert(model.f_segment_to_attribute(4) == 0);
	}
	// Hello, World!
	// 0001122211000
	// 111
	{
		model.f_attribute(0, 3, 1);
		assert(model.f_segments_size() == 4);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 1);
		assert(model.f_segment_to_text(1) == 5);
		assert(model.f_segment_to_attribute(1) == 2);
		assert(model.f_segment_to_text(2) == 8);
		assert(model.f_segment_to_attribute(2) == 1);
		assert(model.f_segment_to_text(3) == 10);
		assert(model.f_segment_to_attribute(3) == 0);
	}
	// Hello, World!
	// 1111122211000
	// 000
	{
		model.f_attribute(0, 3, 0);
		assert(model.f_segments_size() == 5);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 3);
		assert(model.f_segment_to_attribute(1) == 1);
		assert(model.f_segment_to_text(2) == 5);
		assert(model.f_segment_to_attribute(2) == 2);
		assert(model.f_segment_to_text(3) == 8);
		assert(model.f_segment_to_attribute(3) == 1);
		assert(model.f_segment_to_text(4) == 10);
		assert(model.f_segment_to_attribute(4) == 0);
	}
	// Hello, World!
	// 0001122211000
	//       22
	{
		model.f_attribute(6, 2, 2);
		assert(model.f_segments_size() == 5);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 3);
		assert(model.f_segment_to_attribute(1) == 1);
		assert(model.f_segment_to_text(2) == 5);
		assert(model.f_segment_to_attribute(2) == 2);
		assert(model.f_segment_to_text(3) == 8);
		assert(model.f_segment_to_attribute(3) == 1);
		assert(model.f_segment_to_text(4) == 10);
		assert(model.f_segment_to_attribute(4) == 0);
	}
	// Hello, World!
	// 0001122211000
	//       33
	{
		model.f_attribute(6, 2, 3);
		assert(model.f_segments_size() == 6);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 3);
		assert(model.f_segment_to_attribute(1) == 1);
		assert(model.f_segment_to_text(2) == 5);
		assert(model.f_segment_to_attribute(2) == 2);
		assert(model.f_segment_to_text(3) == 6);
		assert(model.f_segment_to_attribute(3) == 3);
		assert(model.f_segment_to_text(4) == 8);
		assert(model.f_segment_to_attribute(4) == 1);
		assert(model.f_segment_to_text(5) == 10);
		assert(model.f_segment_to_attribute(5) == 0);
	}
	// Hello, World!
	// 0001123311000
	//       11
	{
		model.f_attribute(6, 2, 1);
		assert(model.f_segments_size() == 5);
		assert(model.f_segment_to_text(0) == 0);
		assert(model.f_segment_to_attribute(0) == 0);
		assert(model.f_segment_to_text(1) == 3);
		assert(model.f_segment_to_attribute(1) == 1);
		assert(model.f_segment_to_text(2) == 5);
		assert(model.f_segment_to_attribute(2) == 2);
		assert(model.f_segment_to_text(3) == 6);
		assert(model.f_segment_to_attribute(3) == 1);
		assert(model.f_segment_to_text(4) == 10);
		assert(model.f_segment_to_attribute(4) == 0);
	}
	// Hello, World!
	// 0001121111000
	{
		model.f_replace(0, model.f_text_size(), static_cast<t_model::t_segment*>(0), static_cast<t_model::t_segment*>(0), static_cast<wchar_t*>(0));
		assert(model.f_text_size() == 0);
		assert(model.f_segments_size() == 0);
	}
	return 0;
}
