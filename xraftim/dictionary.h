#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <deque>
#include <vector>
#include <string>

struct t_candidate
{
	std::wstring v_text;
	std::vector<std::wstring> v_annotations;

	t_candidate(const std::wstring& a_text) : v_text(a_text)
	{
	}
};

struct t_dictionary
{
	virtual void f_load() = 0;
	virtual void f_save() const = 0;
	virtual void f_search(const wchar_t* a_entry, size_t a_n, size_t a_okuri, std::deque<t_candidate>& a_candidates) const = 0;
	virtual void f_register(const wchar_t* a_entry, size_t a_n, size_t a_okuri, const wchar_t* a_text, size_t a_m) = 0;
};

#endif
