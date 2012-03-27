#ifndef CONTEXT_H
#define CONTEXT_H

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
#include <immdev.h>

#include "engine.h"

class t_imc
{
	HIMC v_himc;
	LPINPUTCONTEXT v_p;

public:
	t_imc(HIMC a_himc) : v_himc(a_himc), v_p(ImmLockIMC(v_himc))
	{
	}
	~t_imc()
	{
		if (v_p) ImmUnlockIMC(v_himc);
	}
	operator LPINPUTCONTEXT() const
	{
		return v_p;
	}
	LPINPUTCONTEXT operator->() const
	{
		return v_p;
	}
};

template<typename T>
class t_imcc
{
	HIMCC v_himcc;
	T* v_p;

public:
	t_imcc(HIMCC a_himcc) : v_himcc(a_himcc), v_p(static_cast<T*>(ImmLockIMCC(v_himcc)))
	{
	}
	~t_imcc()
	{
		if (v_p) ImmUnlockIMCC(v_himcc);
	}
	operator T*() const
	{
		return v_p;
	}
	T* operator->() const
	{
		return v_p;
	}
};

struct t_dictionary_proxy : t_dictionary
{
	virtual void f_load();
	virtual void f_save() const;
	virtual void f_search(const wchar_t* a_entry, size_t a_n, size_t a_okuri, std::deque<t_candidate>& a_candidates) const;
	virtual void f_register(const wchar_t* a_entry, size_t a_n, size_t a_okuri, const wchar_t* a_text, size_t a_m);
};

extern t_dictionary_proxy v_proxy;

class t_ime_engine : public t_engine
{
	static t_modifier f_modifier(CONST LPBYTE a_states);
	static t_key f_key(UINT a_key);

	HIMC v_himc;
	bool v_composing;
	bool v_choosing;
	std::vector<WCHAR> v_cs;
	std::vector<BYTE> v_as;
	struct
	{
		TRANSMSGLIST v_messages0;
		TRANSMSG v_messages1[3];
	};
	UINT v_key;
	UINT v_code;
	LPTRANSMSGLIST v_buffer;
	UINT v_n;

	void f_push(UINT a_message, WPARAM a_wparam, LPARAM a_lparam);
	void f_flush();
	void f_guide(const WCHAR* a_cs, UINT a_n);

protected:
	virtual void f_on_forward();
	virtual void f_on_compose(size_t a_i, size_t a_m, const wchar_t* a_cs, const t_attribute* a_as, size_t a_n);
	virtual void f_on_commit(const wchar_t* a_cs, size_t a_n);
	virtual void f_on_status();
	virtual void f_on_candidates();
	virtual void f_on_choose();

public:
	t_ime_engine(HIMC a_himc) : t_engine(v_proxy), v_himc(a_himc), v_composing(false), v_choosing(false), v_buffer(&v_messages0), v_n(0)
	{
		v_messages0.uMsgCount = 4;
		f_on_status();
	}
	void f_switch();
	UINT f_key_pressed(UINT a_key, UINT a_code, CONST LPBYTE a_states, LPTRANSMSGLIST a_buffer);
};

#endif
