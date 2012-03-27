#include "context.h"

#include <stdexcept>
#include <shlobj.h>

#include "io.h"

namespace
{

class t_device
{
	static HANDLE v_mutex;

	HANDLE v_handle;

public:
	t_device() : v_handle(INVALID_HANDLE_VALUE)
	{
		WCHAR key[MAX_PATH];
		if (SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, key) != S_OK) throw std::runtime_error("SHGetFolderPath");
		std::replace(key, key + std::wcslen(key), L'\\', L'/');
		if (v_mutex == NULL) v_mutex = CreateMutex(NULL, FALSE, (std::wstring(L"Global\\xraftim/mutex/") + key).c_str());
		std::wstring name(L"\\\\.\\pipe\\xraftim/");
		name += key;
		WaitForSingleObject(v_mutex, INFINITE);
		do {
			v_handle = CreateFile(name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
			if (v_handle != INVALID_HANDLE_VALUE) return;
		} while (GetLastError() == ERROR_PIPE_BUSY && WaitNamedPipe(name.c_str(), NMPWAIT_USE_DEFAULT_WAIT));
		extern HINSTANCE v_hinstance;
		WCHAR cs[MAX_PATH];
		GetModuleFileName(v_hinstance, cs, MAX_PATH);
		std::wstring server = cs;
		server.erase(server.find_last_of(L'.'));
		server += L".exe";
		STARTUPINFO si;
		ZeroMemory(&si, sizeof(STARTUPINFO));
		si.cb = sizeof(STARTUPINFO);
		PROCESS_INFORMATION pi;
		if (CreateProcess(server.c_str(), NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
			if (WaitForInputIdle(pi.hProcess, INFINITE) == 0) v_handle = CreateFile(name.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
		}
		if (v_handle == INVALID_HANDLE_VALUE) throw std::runtime_error("INVALID_HANDLE_VALUE");
	}
	~t_device()
	{
		CloseHandle(v_handle);
		ReleaseMutex(v_mutex);
	}
	size_t f_read(char* a_p, size_t a_n)
	{
		DWORD dw;
		if (!ReadFile(v_handle, a_p, a_n, &dw, NULL)) throw std::runtime_error("ReadFile");
		return dw;
	}
	void f_write(const char* a_p, size_t a_n)
	{
		while (a_n > 0) {
			DWORD dw;
			if (!WriteFile(v_handle, a_p, a_n, &dw, NULL)) throw std::runtime_error("WriteFile");
			a_p += dw;
			a_n -= dw;
		}
	}
};

HANDLE t_device::v_mutex = NULL;

}

void t_dictionary_proxy::f_load()
{
	try {
		t_device device;
		t_writer<t_device> writer(device);
		writer.f_write(0);
		writer.f_write(GetCurrentProcessId());
	} catch (std::exception&) {
	}
}

void t_dictionary_proxy::f_save() const
{
	try {
		t_device device;
		t_writer<t_device> writer(device);
		writer.f_write(1);
		writer.f_write(GetCurrentProcessId());
	} catch (std::exception&) {
	}
}

void t_dictionary_proxy::f_search(const wchar_t* a_entry, size_t a_n, size_t a_okuri, std::deque<t_candidate>& a_candidates) const
{
	try {
		t_device device;
		{
			t_writer<t_device> writer(device);
			writer.f_write(2);
			writer.f_write(GetCurrentProcessId());
			writer.f_write(a_n);
			writer.f_write(a_okuri);
			writer.f_write(reinterpret_cast<const char*>(a_entry), sizeof(wchar_t) * (a_n + a_okuri));
		}
		t_reader<t_device> reader(device);
		size_t n;
		reader.f_read(n);
		for (size_t i = 0; i < n; ++i) {
			{
				size_t m;
				reader.f_read(m);
				std::vector<wchar_t> cs(m);
				reader.f_read(reinterpret_cast<char*>(&cs[0]), sizeof(wchar_t) * m);
				a_candidates.push_back(t_candidate(std::wstring(cs.begin(), cs.end())));
			}
			std::vector<std::wstring>& annotations = a_candidates.back().v_annotations;
			size_t m;
			reader.f_read(m);
			for (size_t j = 0; j < m; ++j) {
				size_t n;
				reader.f_read(n);
				std::vector<wchar_t> cs(n);
				reader.f_read(reinterpret_cast<char*>(&cs[0]), sizeof(wchar_t) * n);
				annotations.push_back(std::wstring(cs.begin(), cs.end()));
			}
		}
	} catch (std::exception&) {
	}
}

void t_dictionary_proxy::f_register(const wchar_t* a_entry, size_t a_n, size_t a_okuri, const wchar_t* a_text, size_t a_m)
{
	try {
		t_device device;
		t_writer<t_device> writer(device);
		writer.f_write(3);
		writer.f_write(GetCurrentProcessId());
		writer.f_write(a_n);
		writer.f_write(a_okuri);
		writer.f_write(reinterpret_cast<const char*>(a_entry), sizeof(wchar_t) * (a_n + a_okuri));
		writer.f_write(a_m);
		writer.f_write(reinterpret_cast<const char*>(a_text), sizeof(wchar_t) * a_m);
	} catch (std::exception&) {
	}
}

t_dictionary_proxy v_proxy;

t_modifier t_ime_engine::f_modifier(CONST LPBYTE a_states)
{
	int modifier = e_modifier__NONE;
	if ((a_states[VK_SHIFT] & 0x80) != 0) modifier |= e_modifier__SHIFT;
	if ((a_states[VK_CONTROL] & 0x80) != 0) modifier |= e_modifier__CONTROL;
	if ((a_states[VK_LWIN] & 0x80) != 0 || (a_states[VK_RWIN] & 0x80) != 0) modifier |= e_modifier__META;
	if ((a_states[VK_MENU] & 0x80) != 0) modifier |= e_modifier__ALTERNATE;
	return static_cast<t_modifier>(modifier);
}

void t_ime_engine::f_push(UINT a_message, WPARAM a_wparam, LPARAM a_lparam)
{
	if (v_n >= v_buffer->uMsgCount) f_flush();
	TRANSMSG& message = v_buffer->TransMsg[v_n];
	message.message = a_message;
	message.wParam = a_wparam;
	message.lParam = a_lparam;
	++v_n;
}

void t_ime_engine::f_flush()
{
	if (v_n <= 0) return;
	t_imc imc(v_himc);
	if (IsWindow(imc->hWnd)) {
		imc->dwNumMsgBuf = v_n;
		imc->hMsgBuf = ImmReSizeIMCC(imc->hMsgBuf, sizeof(TRANSMSG) * v_n);
		{
			t_imcc<TRANSMSG> buffer(imc->hMsgBuf);
			for (UINT i = 0; i < v_n; ++i) buffer[i] = v_buffer->TransMsg[i];
		}
		ImmGenerateMessage(v_himc);
	}
	v_n = 0;
}

void t_ime_engine::f_guide(const WCHAR* a_cs, UINT a_n)
{
	t_imc imc(v_himc);
	DWORD n = sizeof(GUIDELINE) + sizeof(WCHAR) * a_n;
	imc->hGuideLine = ImmReSizeIMCC(imc->hGuideLine, n);
	t_imcc<GUIDELINE> guide(imc->hGuideLine);
	ZeroMemory(guide, sizeof(GUIDELINE));
	guide->dwSize = n;
	guide->dwLevel = GL_LEVEL_INFORMATION;
	guide->dwIndex = GL_ID_UNKNOWN;
	guide->dwStrLen = a_n;
	guide->dwStrOffset = sizeof(GUIDELINE);
	std::copy(a_cs, a_cs + a_n, reinterpret_cast<WCHAR*>(guide + 1));
}

void t_ime_engine::f_on_forward()
{
	f_push(WM_IME_KEYDOWN, LOWORD(v_key), (v_code << 16) | 1);
}

void t_ime_engine::f_on_compose(size_t a_i, size_t a_m, const wchar_t* a_cs, const t_attribute* a_as, size_t a_n)
{
	{
		std::vector<WCHAR>::iterator i = v_cs.begin() + a_i;
		v_cs.insert(v_cs.erase(i, i + a_m), a_cs, a_cs + a_n);
	}
	{
		std::vector<BYTE>::iterator i = v_as.begin() + a_i;
		v_as.insert(v_as.erase(i, i + a_m), a_n, ATTR_INPUT_ERROR);
		i = v_as.begin() + a_i;
		for (size_t j = 0; j < a_n; ++j) {
			switch (a_as[j]) {
			case e_attribute__ROMAN:
				*i = ATTR_INPUT;
				break;
			case e_attribute__TEXT:
				*i = ATTR_CONVERTED;
				break;
			case e_attribute__ENTRY:
				*i = ATTR_TARGET_NOTCONVERTED;
				break;
			case e_attribute__OKURI:
				*i = ATTR_FIXEDCONVERTED;
				break;
			case e_attribute__CANDIDATE:
				*i = ATTR_TARGET_CONVERTED;
				break;
			}
			++i;
		}
	}
	f_flush();
	if (!v_composing) {
		if (v_cs.empty()) return;
		v_composing = true;
		f_push(WM_IME_STARTCOMPOSITION, 0, 0);
	}
	{
		std::wstring read = f_entry(f_states().size() - 1);
		t_imc imc(v_himc);
		DWORD n = sizeof(COMPOSITIONSTRING) + sizeof(DWORD) * 4 + (sizeof(BYTE) + sizeof(WCHAR)) * (read.size() + v_cs.size());
		imc->hCompStr = ImmReSizeIMCC(imc->hCompStr, n);
		t_imcc<COMPOSITIONSTRING> composition(imc->hCompStr);
		ZeroMemory(composition, sizeof(COMPOSITIONSTRING));
		composition->dwSize = n;
		composition->dwCompReadAttrLen = sizeof(BYTE) * read.size();
		composition->dwCompReadAttrOffset = sizeof(COMPOSITIONSTRING);
		composition->dwCompReadClauseLen = sizeof(DWORD) * 2;
		composition->dwCompReadClauseOffset = composition->dwCompReadAttrOffset + composition->dwCompReadAttrLen;
		composition->dwCompReadStrLen = read.size();
		composition->dwCompReadStrOffset = composition->dwCompReadClauseOffset + composition->dwCompReadClauseLen;
		composition->dwCompAttrLen = sizeof(BYTE) * v_as.size();
		composition->dwCompAttrOffset = composition->dwCompReadStrOffset + sizeof(WCHAR) * composition->dwCompReadStrLen;
		composition->dwCompClauseLen = sizeof(DWORD) * 2;
		composition->dwCompClauseOffset = composition->dwCompAttrOffset + composition->dwCompAttrLen;
		composition->dwCompStrLen = v_cs.size();
		composition->dwCompStrOffset = composition->dwCompClauseOffset + composition->dwCompClauseLen;
		composition->dwCursorPos = f_caret();
		composition->dwDeltaStart = a_i;
		LPCOMPOSITIONSTRING c = composition;
		char* p = reinterpret_cast<char*>(c) + composition->dwCompReadAttrOffset;
		std::fill_n(reinterpret_cast<BYTE*>(p), read.size(), ATTR_INPUT);
		p = reinterpret_cast<char*>(c) + composition->dwCompReadClauseOffset;
		reinterpret_cast<DWORD*>(p)[0] = 0;
		reinterpret_cast<DWORD*>(p)[1] = read.size();
		p = reinterpret_cast<char*>(c) + composition->dwCompReadStrOffset;
		std::copy(read.begin(), read.end(), reinterpret_cast<WCHAR*>(p));
		p = reinterpret_cast<char*>(c) + composition->dwCompAttrOffset;
		std::copy(v_as.begin(), v_as.end(), reinterpret_cast<BYTE*>(p));
		p = reinterpret_cast<char*>(c) + composition->dwCompClauseOffset;
		reinterpret_cast<DWORD*>(p)[0] = 0;
		reinterpret_cast<DWORD*>(p)[1] = v_cs.size();
		p = reinterpret_cast<char*>(c) + composition->dwCompStrOffset;
		std::copy(v_cs.begin(), v_cs.end(), reinterpret_cast<WCHAR*>(p));
	}
	if (v_cs.empty()) {
		f_push(WM_IME_COMPOSITION, 0, 0);
		v_composing = false;
		f_push(WM_IME_ENDCOMPOSITION, 0, 0);
	} else {
		f_push(WM_IME_COMPOSITION, 0, GCS_COMPREAD | GCS_COMP | GCS_CURSORPOS | GCS_DELTASTART);
	}
}

void t_ime_engine::f_on_commit(const wchar_t* a_cs, size_t a_n)
{
	v_cs.clear();
	v_as.clear();
	f_flush();
	if (!v_composing) f_push(WM_IME_STARTCOMPOSITION, 0, 0);
	{
		t_imc imc(v_himc);
		DWORD n = sizeof(COMPOSITIONSTRING) + sizeof(DWORD) * 2 + sizeof(WCHAR) * a_n;
		imc->hCompStr = ImmReSizeIMCC(imc->hCompStr, n);
		t_imcc<COMPOSITIONSTRING> composition(imc->hCompStr);
		ZeroMemory(composition, sizeof(COMPOSITIONSTRING));
		composition->dwSize = n;
		composition->dwResultClauseLen = sizeof(DWORD) * 2;
		composition->dwResultClauseOffset = sizeof(COMPOSITIONSTRING);
		composition->dwResultStrLen = a_n;
		composition->dwResultStrOffset = composition->dwResultClauseOffset + composition->dwResultClauseLen;
		LPCOMPOSITIONSTRING c = composition;
		char* p = reinterpret_cast<char*>(c) + composition->dwResultClauseOffset;
		reinterpret_cast<DWORD*>(p)[0] = 0;
		reinterpret_cast<DWORD*>(p)[1] = a_n;
		p = reinterpret_cast<char*>(c) + composition->dwResultStrOffset;
		std::copy(a_cs, a_cs + a_n, reinterpret_cast<WCHAR*>(p));
	}
	f_push(WM_IME_COMPOSITION, 0, GCS_RESULT);
	v_composing = false;
	f_push(WM_IME_ENDCOMPOSITION, 0, 0);
}

void t_ime_engine::f_on_status()
{
	t_imc imc(v_himc);
	if (!imc->fOpen || v_choosing) {
		f_guide(0, 0);
	} else {
		std::wstring s = f_status();
		std::vector<WCHAR> cs(s.begin(), s.end());
		size_t n = f_states().size() - 1;
		while (n > 0) {
			cs.push_back(L'\n');
			cs.push_back(L'?');
			s = f_entry(--n);
			cs.insert(cs.end(), s.begin(), s.end());
		}
		f_guide(&cs[0], cs.size());
	}
	f_push(WM_IME_NOTIFY, IMN_GUIDELINE, 0);
}

void t_ime_engine::f_on_candidates()
{
	if (f_candidates().empty()) {
		{
			t_imc imc(v_himc);
			imc->hCandInfo = ImmReSizeIMCC(imc->hCandInfo, sizeof(CANDIDATEINFO));
			t_imcc<CANDIDATEINFO> candidates(imc->hCandInfo);
			ZeroMemory(candidates, sizeof(CANDIDATEINFO));
		}
		v_choosing = false;
		f_push(WM_IME_NOTIFY, IMN_CLOSECANDIDATE, 1);
		f_on_status();
	} else {
		std::vector<DWORD> is;
		std::vector<WCHAR> cs;
		for (size_t i = 0; i < f_candidates().size(); ++i) {
			is.push_back(cs.size());
			const t_candidate& candidate = f_candidates()[i];
			const std::wstring& text = candidate.v_text;
			cs.insert(cs.end(), text.begin(), text.end());
			const std::vector<std::wstring>& annotations = candidate.v_annotations;
			for (std::vector<std::wstring>::const_iterator j = annotations.begin(); j != annotations.end(); ++j) {
				cs.push_back(L' ');
				cs.push_back(L';');
				cs.insert(cs.end(), j->begin(), j->end());
			}
		}
		{
			t_imc imc(v_himc);
			DWORD n = sizeof(CANDIDATEINFO) + sizeof(CANDIDATELIST) + sizeof(DWORD) * (is.size() - 1) + sizeof(WCHAR) * cs.size();
			imc->hCandInfo = ImmReSizeIMCC(imc->hCandInfo, n);
			t_imcc<CANDIDATEINFO> candidates(imc->hCandInfo);
			ZeroMemory(candidates, sizeof(CANDIDATEINFO) + sizeof(CANDIDATELIST));
			candidates->dwSize = n;
			candidates->dwCount = 1;
			candidates->dwOffset[0] = sizeof(CANDIDATEINFO);
			LPCANDIDATELIST list = reinterpret_cast<LPCANDIDATELIST>(candidates + 1);
			list->dwSize = n - sizeof(CANDIDATEINFO);
			list->dwStyle = IME_CAND_READ;
			list->dwCount = is.size();
			DWORD m = sizeof(CANDIDATELIST) + sizeof(DWORD) * (is.size() - 1);
			for (size_t i = 0; i < is.size(); ++i) list->dwOffset[i] = m + sizeof(WCHAR) * is[i];
			std::copy(cs.begin(), cs.end(), reinterpret_cast<WCHAR*>(list->dwOffset + is.size()));
		}
		v_choosing = true;
		f_on_status();
		f_push(WM_IME_NOTIFY, IMN_OPENCANDIDATE, 1);
		f_on_choose();
	}
}

void t_ime_engine::f_on_choose()
{
	size_t first = f_chosen() / 8 * 8;
	size_t last = std::min(first + 8, f_candidates().size());
	{
		t_imc imc(v_himc);
		t_imcc<CANDIDATEINFO> candidates(imc->hCandInfo);
		LPCANDIDATELIST list = reinterpret_cast<LPCANDIDATELIST>(candidates + 1);
		list->dwSelection = f_chosen();
		list->dwPageStart = first;
		list->dwPageSize = last - first;
	}
	f_push(WM_IME_NOTIFY, IMN_CHANGECANDIDATE, 1);
}

void t_ime_engine::f_switch()
{
	t_imc imc(v_himc);
	if (!imc->fOpen) f_reset();
	f_on_status();
	f_flush();
}

UINT t_ime_engine::f_key_pressed(UINT a_key, UINT a_code, CONST LPBYTE a_states, LPTRANSMSGLIST a_buffer)
{
	v_key = a_key;
	v_code = a_code;
	v_buffer = a_buffer;
	t_modifier modifier = f_modifier(a_states);
	t_key key = f_win32_key(LOWORD(a_key));
	WORD ascii = HIWORD(a_key);
	t_engine::f_key_pressed(modifier, key, static_cast<char>(ascii));
	f_push(WM_IME_NOTIFY, IMN_GUIDELINE, 0);
	v_buffer = &v_messages0;
	UINT n = v_n;
	v_n = 0;
	return n;
}
