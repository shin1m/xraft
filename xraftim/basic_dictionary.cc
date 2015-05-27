#include "basic_dictionary.h"

#include <cstdio>
#include <algorithm>
#include <iterator>

namespace
{

void f_skip(std::FILE* a_fp)
{
	while (true) {
		switch (std::getc(a_fp)) {
		case EOF:
		case '\n':
			return;
		}
	}
}

struct t_compare_separator
{
	bool f_equal(std::FILE* a_fp) const
	{
		while (true) {
			int c = std::getc(a_fp);
			if (c == EOF || c == '\n') return false;
			if (c == ' ') break;
		}
		return std::getc(a_fp) != '/';
	}
	bool f_greater(std::FILE* a_fp) const
	{
		std::vector<unsigned char> a;
		while (true) {
			int c = std::getc(a_fp);
			if (c == EOF || c == '\n') return true;
			if (c == ' ') break;
			a.push_back(c);
		}
		if (std::getc(a_fp) != '/') return false;
		f_skip(a_fp);
		std::vector<unsigned char> b;
		while (true) {
			int c = std::getc(a_fp);
			if (c == EOF || c == '\n') return true;
			if (c == ' ') break;
			b.push_back(c);
		}
		if (std::getc(a_fp) != '/') return false;
		return a < b;
	}
};

class t_compare_nashi
{
	const char* v_cs;
	size_t v_n;

protected:
	int f_compare(std::FILE* a_fp) const
	{
		for (size_t i = 0; i < v_n; ++i) {
			int a = std::getc(a_fp);
			if (a == EOF) return 1;
			if (a == ' ') return -1;
			a = static_cast<unsigned char>(a);
			int b = static_cast<unsigned char>(v_cs[i]);
			if (a != b) return a - b;
		}
		return std::getc(a_fp) == ' ' ? 0 : 1;
	}

public:
	t_compare_nashi(const char* a_cs, size_t a_n) : v_cs(a_cs), v_n(a_n)
	{
	}
	bool f_equal(std::FILE* a_fp) const
	{
		return f_compare(a_fp) == 0;
	}
	bool f_greater(std::FILE* a_fp) const
	{
		return f_compare(a_fp) > 0;
	}
};

struct t_compare_ari : t_compare_nashi
{
	t_compare_ari(const char* a_cs, size_t a_n) : t_compare_nashi(a_cs, a_n)
	{
	}
	bool f_greater(std::FILE* a_fp) const
	{
		return f_compare(a_fp) < 0;
	}
};

class t_file
{
	std::FILE* v_fp;

	template<typename T>
	bool f_seek(size_t a_f, size_t a_l, T a_compare);

public:
	t_file(const char* a_path, const char* a_mode) : v_fp(std::fopen(a_path, a_mode))
	{
	}
	~t_file()
	{
		if (v_fp != NULL) std::fclose(v_fp);
	}
	bool f_seek();
	bool f_seek(const char* a_entry, size_t a_n, bool a_ari);
	size_t operator()(char* a_cs, size_t a_n)
	{
		if (v_fp == NULL) return 0;
		char* p = a_cs;
		while (a_n > 0) {
			int c = std::getc(v_fp);
			if (c == EOF) break;
			*p++ = c;
			--a_n;
		}
		return p - a_cs;
	}
	void operator()(const char* a_cs, size_t a_n)
	{
		if (v_fp == NULL) return;
		while (a_n > 0) {
			std::putc(*a_cs++, v_fp);
			--a_n;
		}
	}
};

template<typename T>
bool t_file::f_seek(size_t a_f, size_t a_l, T a_compare)
{
	while (a_f < a_l) {
		std::fseek(v_fp, (a_f + a_l) / 2, SEEK_SET);
		f_skip(v_fp);
		size_t i = std::ftell(v_fp);
		if (i >= a_l) {
			std::fseek(v_fp, a_f, SEEK_SET);
			while (a_f < a_l) {
				if (a_compare.f_equal(v_fp)) return true;
				f_skip(v_fp);
				a_f = std::ftell(v_fp);
			}
			break;
		} else if (a_compare.f_greater(v_fp)) {
			a_l = i;
		} else {
			a_f = i;
		}
	}
	return false;
}

bool t_file::f_seek()
{
	if (v_fp == NULL) return false;
	std::fseek(v_fp, 0, SEEK_SET);
	while (true) {
		const char* p = ";; okuri-ari entries.";
		int c = std::getc(v_fp);
		while (c == *p) {
			c = std::getc(v_fp);
			if (*++p == '\0') break;
		}
		if (c == '\n' && *p == '\0') break;
		while (c != EOF && c != '\n') c = std::getc(v_fp);
		if (c == EOF) return false;
	}
	return true;
}

bool t_file::f_seek(const char* a_entry, size_t a_n, bool a_ari)
{
	if (!f_seek()) return false;
	size_t f = std::ftell(v_fp);
	std::fseek(v_fp, 0, SEEK_END);
	size_t l = std::ftell(v_fp);
	if (!f_seek(f, l, t_compare_separator())) return false;
	if (a_ari) {
		return f_seek(f, std::ftell(v_fp) - 1, t_compare_ari(a_entry, a_n));
	} else {
		f_skip(v_fp);
		return f_seek(std::ftell(v_fp), l, t_compare_nashi(a_entry, a_n));
	}
}

enum t_token
{
	e_token__NONE,
	e_token__TEXT,
	e_token__BEGIN,
	e_token__END
};

template<typename T>
class t_lexer
{
	T& v_source;
	wint_t v_c;
	t_token v_token;
	std::vector<wchar_t> v_text;
	std::vector<wchar_t> v_annotation;

public:
	t_lexer(T& a_source) : v_source(a_source), v_c(v_source())
	{
		f_next();
	}
	t_token f_token() const
	{
		return v_token;
	}
	std::wstring f_text() const
	{
		return std::wstring(v_text.begin(), v_text.end());
	}
	std::wstring f_annotation() const
	{
		return std::wstring(v_annotation.begin(), v_annotation.end());
	}
	void f_next();
};

template<typename T>
void t_lexer<T>::f_next()
{
	v_token = e_token__NONE;
	if (v_c != L'/') return;
	v_c = v_source();
	if (v_c == WEOF || v_c == L'\n') return;
	if (v_c == L'[') {
		v_c = v_source();
		v_token = e_token__BEGIN;
	} else if (v_c == L']') {
		v_c = v_source();
		v_token = e_token__END;
	} else {
		v_token = e_token__TEXT;
	}
	v_text.clear();
	v_annotation.clear();
	while (v_c != L'/' && v_c != L';') {
		if (v_c == L'\\') v_c = v_source();
		if (v_c == WEOF || v_c == L'\n') return;
		v_text.push_back(v_c);
		v_c = v_source();
	}
	if (v_c != L';') return;
	v_c = v_source();
	while (v_c != L'/') {
		if (v_c == L'\\') v_c = v_source();
		if (v_c == WEOF || v_c == L'\n') return;
		v_annotation.push_back(v_c);
		v_c = v_source();
	}
}

template<typename T>
void f_build(const std::vector<std::wstring>& a_texts, T& a_target)
{
	for (const auto& x : a_texts) {
		a_target(L'/');
		for (wchar_t c : x) {
			switch (c) {
			case L'/':
			case L';':
			case L'[':
			case L'\\':
			case L']':
				a_target(L'\\');
				break;
			}
			a_target(c);
		}
	}
}

void f_add(std::deque<t_candidate>& a_candidates, std::map<std::wstring, t_candidate*>& a_map, const std::wstring& a_text, const std::wstring& a_annotation)
{
	if (a_text.empty()) return;
	auto i = a_map.lower_bound(a_text);
	if (i == a_map.end() || i->first != a_text) {
		a_candidates.push_back(t_candidate(a_text));
		i = a_map.emplace_hint(i, a_text, &a_candidates.back());
	}
	if (!a_annotation.empty()) i->second->v_annotations.push_back(a_annotation);
}

void f_add(std::deque<t_candidate>& a_candidates, std::map<std::wstring, t_candidate*>& a_map, const std::vector<std::wstring>& a_texts)
{
	for (const auto& x : a_texts) f_add(a_candidates, a_map, x, std::wstring());
}

void f_put(std::map<std::wstring, std::vector<std::wstring> >& a_map, const std::wstring& a_entry, const std::wstring& a_text)
{
	auto i = a_map.lower_bound(a_entry);
	if (i == a_map.end() || i->first != a_entry) i = a_map.emplace_hint(i, a_entry, std::vector<std::wstring>());
	std::vector<std::wstring>& texts = i->second;
	texts.erase(std::remove(texts.begin(), texts.end(), a_text), texts.end());
	texts.insert(texts.begin(), a_text);
}

void f_search(const char* a_path, const char* a_entry, size_t a_n, bool a_ari, std::deque<t_candidate>& a_candidates, std::map<std::wstring, t_candidate*>& a_map)
{
	t_file file(a_path, "rb");
	if (!file.f_seek(a_entry, a_n, a_ari)) return;
	t_decoder<t_file> decoder(file, "euc-jp");
	t_lexer<t_decoder<t_file> > lexer(decoder);
	while (lexer.f_token() == e_token__TEXT) {
		f_add(a_candidates, a_map, lexer.f_text(), lexer.f_annotation());
		lexer.f_next();
	}
}

}

void t_basic_dictionary::f_load()
{
	if (v_private.empty()) return;
	v_nashis.clear();
	v_aris.clear();
	t_file file(v_private.c_str(), "rb");
	if (!file.f_seek()) return;
	t_decoder<t_file> decoder(file, "euc-jp");
	while (true) {
		wint_t c = decoder();
		if (c == WEOF) break;
		if (c == L';') {
			do c = decoder(); while (c != WEOF && c != L'\n');
			break;
		}
		std::vector<wchar_t> cs;
		while (c != WEOF && c != L'\n' && c != L' ') {
			cs.push_back(c);
			c = decoder();
		}
		if (c != L' ') continue;
		t_map0& map = v_aris.emplace(std::wstring(cs.begin(), cs.end()), t_map0()).first->second;
		auto texts = map.emplace(std::wstring(), std::vector<std::wstring>()).first;
		t_lexer<t_decoder<t_file> > lexer(decoder);
		while (lexer.f_token() != e_token__NONE) {
			if (lexer.f_token() == e_token__TEXT)
				texts->second.push_back(lexer.f_text());
			else if (lexer.f_token() == e_token__BEGIN)
				texts = map.emplace(lexer.f_text(), std::vector<std::wstring>()).first;
			else if (lexer.f_token() == e_token__END)
				texts = map.emplace(std::wstring(), std::vector<std::wstring>()).first;
			lexer.f_next();
		}
	}
	while (true) {
		wint_t c = decoder();
		if (c == WEOF) break;
		std::vector<wchar_t> cs;
		while (c != WEOF && c != L'\n' && c != L' ') {
			cs.push_back(c);
			c = decoder();
		}
		if (c != L' ') continue;
		std::vector<std::wstring>& texts = v_nashis.emplace(std::wstring(cs.begin(), cs.end()), std::vector<std::wstring>()).first->second;
		t_lexer<t_decoder<t_file> > lexer(decoder);
		while (lexer.f_token() == e_token__TEXT) {
			texts.push_back(lexer.f_text());
			lexer.f_next();
		}
	}
}

void t_basic_dictionary::f_save() const
{
	if (v_private.empty()) return;
	t_file file(v_private.c_str(), "wb");
	t_encoder<t_file> encoder(file, "euc-jp");
	encoder(L";; okuri-ari entries.\n");
	for (auto i = v_aris.rbegin(); i != v_aris.rend(); ++i) {
		encoder(i->first.begin(), i->first.end());
		encoder(L' ');
		auto j = i->second.begin();
		f_build(j->second, encoder);
		while (++j != i->second.end()) {
			encoder(L'/');
			encoder(L'[');
			encoder(j->first.begin(), j->first.end());
			f_build(j->second, encoder);
			encoder(L'/');
			encoder(L']');
		}
		encoder(L'/');
		encoder(L'\n');
	}
	encoder(L";; okuri-nasi entries.\n");
	for (const auto& p : v_nashis) {
		encoder(p.first.begin(), p.first.end());
		encoder(L' ');
		f_build(p.second, encoder);
		encoder(L'/');
		encoder(L'\n');
	}
}

void t_basic_dictionary::f_search(const wchar_t* a_entry, size_t a_n, size_t a_okuri, std::deque<t_candidate>& a_candidates) const
{
	bool ari = a_okuri > 0;
	std::map<std::wstring, t_candidate*> map;
	if (ari) {
		++a_n;
		auto i = v_aris.find(std::wstring(a_entry, a_n));
		if (i != v_aris.end()) {
			const t_map0& aris = i->second;
			auto j = aris.find(std::wstring(a_entry + a_n, a_okuri - 1));
			if (j != aris.end()) f_add(a_candidates, map, j->second);
			f_add(a_candidates, map, aris.find(std::wstring())->second);
		}
	} else {
		auto i = v_nashis.find(std::wstring(a_entry, a_n));
		if (i != v_nashis.end()) f_add(a_candidates, map, i->second);
	}
	std::vector<char> cs;
	v_converter(a_entry, a_entry + a_n, std::back_inserter(cs));
	for (const auto& x : v_publics) ::f_search(x.c_str(), &cs[0], cs.size(), ari, a_candidates, map);
}

void t_basic_dictionary::f_register(const wchar_t* a_entry, size_t a_n, size_t a_okuri, const wchar_t* a_text, size_t a_m)
{
	std::wstring text(a_text, a_m);
	if (a_okuri > 0) {
		++a_n;
		std::wstring entry(a_entry, a_n);
		auto i = v_aris.find(entry);
		if (i == v_aris.end()) i = v_aris.emplace_hint(i, entry, t_map0());
		f_put(i->second, std::wstring(), text);
		f_put(i->second, std::wstring(a_entry + a_n, a_okuri - 1), text);
	} else {
		f_put(v_nashis, std::wstring(a_entry, a_n), text);
	}
}
