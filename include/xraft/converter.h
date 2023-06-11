#ifndef XRAFT__CONVERTER_H
#define XRAFT__CONVERTER_H

#include <cerrno>
#include <climits>
#include <cwchar>
#include <algorithm>
#include <iconv.h>

namespace xraft
{

template<typename T>
class t_encoder
{
	T& v_target;
	iconv_t v_cd;
	wchar_t v_cs[16];
	size_t v_n = 0;

	void f_convert()
	{
		char mbs[MB_LEN_MAX * 16];
		char* p = reinterpret_cast<char*>(const_cast<wchar_t*>(v_cs));
		v_n *= sizeof(wchar_t);
		while (v_n > 0) {
			char* q = mbs;
			size_t n = sizeof(mbs);
			if (iconv(v_cd, &p, &v_n, &q, &n) == size_t(-1) && errno != E2BIG) break;
			v_target(static_cast<const char*>(mbs), sizeof(mbs) - n);
		}
		v_n = 0;
	}

public:
	t_encoder(T& a_target, const char* a_encoding) : v_target(a_target), v_cd(iconv_open(a_encoding, "wchar_t"))
	{
	}
	~t_encoder()
	{
		f_flush();
		iconv_close(v_cd);
	}
	void operator()(wchar_t a_c)
	{
		if (v_n >= sizeof(v_cs) / sizeof(wchar_t)) f_convert();
		v_cs[v_n++] = a_c;
	}
	void operator()(const wchar_t* a_cs)
	{
		while (*a_cs != L'\0') (*this)(*a_cs++);
	}
	void operator()(auto a_f, auto a_l)
	{
		while (a_f != a_l) {
			(*this)(*a_f);
			++a_f;
		}
	}
	void f_flush()
	{
		f_convert();
		char mbs[MB_LEN_MAX];
		char* q = mbs;
		size_t n = sizeof(mbs);
		if (iconv(v_cd, NULL, NULL, &q, &n) != size_t(-1)) v_target(static_cast<const char*>(mbs), sizeof(mbs) - n);
	}
};

template<typename T>
class t_decoder
{
	T& v_source;
	iconv_t v_cd;
	char v_mbs[MB_LEN_MAX * 16];
	char* v_p0;
	char* v_q0;
	wchar_t v_cs[16];
	wchar_t* v_p;
	wchar_t* v_q;

	void f_convert()
	{
		v_p = v_q = v_cs;
		do {
			if (v_p0 <= v_mbs) {
				size_t n = v_source(v_q0, v_mbs + sizeof(v_mbs) - v_q0);
				if (n <= 0) break;
				v_q0 += n;
			}
			size_t m = v_q0 - v_p0;
			size_t n = sizeof(v_cs);
			if (iconv(v_cd, &v_p0, &m, reinterpret_cast<char**>(&v_q), &n) == size_t(-1)) {
				if (errno == EINVAL) {
					v_q0 = std::copy(v_p0, v_q0, static_cast<char*>(v_mbs));
					v_p0 = v_mbs;
				} else if (errno != E2BIG) {
					break;
				}
			} else {
				v_p0 = v_q0 = v_mbs;
			}
		} while (v_q <= v_cs);
	}

public:
	t_decoder(T& a_source, const char* a_encoding) : v_source(a_source), v_cd(iconv_open("wchar_t", a_encoding)), v_p0(v_mbs), v_q0(v_mbs), v_p(v_cs), v_q(v_cs)
	{
	}
	~t_decoder()
	{
		iconv_close(v_cd);
	}
	wint_t operator()()
	{
		if (v_p >= v_q) f_convert();
		return v_p < v_q ? *v_p++ : WEOF;
	}
	void f_reset()
	{
		iconv(v_cd, NULL, NULL, NULL, NULL);
		v_p0 = v_q0 = v_mbs;
		v_p = v_q = v_cs;
	}
};

template<typename C0, typename C1>
class t_converter
{
	iconv_t v_cd;

public:
	t_converter(const char* a_from, const char* a_to) : v_cd(iconv_open(a_to, a_from))
	{
	}
	~t_converter()
	{
		iconv_close(v_cd);
	}
	auto operator()(auto f, auto l, auto d) const;
};

template<typename C0, typename C1>
auto t_converter<C0, C1>::operator()(auto f, auto l, auto d) const
{
	char cs0[16];
	char cs1[16];
	char* p0 = cs0;
	while (f != l || p0 > cs0) {
		while (f != l && p0 + sizeof(C0) <= cs0 + sizeof(cs0)) {
			*reinterpret_cast<C0*>(p0) = *f;
			p0 += sizeof(C0);
			++f;
		}
		size_t n0 = p0 - cs0;
		p0 = cs0;
		char* p1 = cs1;
		size_t n1 = sizeof(cs1);
		do {
			size_t n = iconv(v_cd, &p0, &n0, &p1, &n1);
			if (n == static_cast<size_t>(-1)) {
				if (errno == EILSEQ) {
					if (n1 < sizeof(C1)) break;
					*reinterpret_cast<C1*>(p1) = '?';
					p1 += sizeof(C1);
					n1 -= sizeof(C1);
				} else if (errno == EINVAL) {
					if (p0 > cs0) break;
				} else {
					break;
				}
				p0 += sizeof(C0);
				n0 -= sizeof(C0);
			}
		} while (n0 > 0);
		d = std::copy(reinterpret_cast<const C1*>(cs1), reinterpret_cast<const C1*>(p1), d);
		p0 = std::copy(p0, p0 + n0, static_cast<char*>(cs0));
	}
	char* p1 = cs1;
	size_t n1 = sizeof(cs1);
	if (iconv(v_cd, NULL, NULL, &p1, &n1) != static_cast<size_t>(-1)) d = std::copy(reinterpret_cast<const C1*>(cs1), reinterpret_cast<const C1*>(p1), d);
	return d;
}

}

#endif
