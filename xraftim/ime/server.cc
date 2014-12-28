#include <cstdarg>
#include <ctime>
#include <algorithm>
#include <set>
#include <stdexcept>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <windows.h>
#include <shlobj.h>

#include "basic_dictionary.h"
#include "io.h"

namespace
{

void f_log(const char* a_format, ...)
{
	CHAR cs[MAX_PATH];
	if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, cs) != S_OK) return;
	std::FILE* fp = std::fopen((std::string(cs) + "\\xraftim\\server.log").c_str(), "a");
	std::time_t t = std::time(NULL);
	std::strftime(cs, sizeof(cs), "%Y/%m/%d %H:%M:%S ", std::localtime(&t));
	std::fputs(cs, fp);
	std::va_list arguments;
	va_start(arguments, a_format);
	std::vfprintf(fp, a_format, arguments);
	va_end(arguments);
	std::fclose(fp);
}

inline LPCTSTR f_class()
{
	return L"xraftim::server";
}

LRESULT CALLBACK f_process(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_ENDSESSION:
f_log("saving by WM_ENDSESSION...\n");
		reinterpret_cast<t_dictionary*>(GetWindowLongPtr(hWnd, 0))->f_save();
f_log("done.\n");
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

class t_device
{
	HANDLE v_handle;
	HANDLE v_event;

public:
	t_device(HANDLE a_handle, HANDLE a_event) : v_handle(a_handle), v_event(a_event)
	{
	}
	~t_device()
	{
		CloseHandle(v_handle);
		CloseHandle(v_event);
	}
	size_t f_read(char* a_p, size_t a_n)
	{
		OVERLAPPED overlapped;
		overlapped.Offset = overlapped.OffsetHigh = 0;
		overlapped.hEvent = v_event;
		if (!ReadFile(v_handle, a_p, a_n, NULL, &overlapped) && GetLastError() != ERROR_IO_PENDING) throw std::runtime_error("ReadFile");
		DWORD dw;
		if (!GetOverlappedResult(v_handle, &overlapped, &dw, TRUE)) throw std::runtime_error("GetOverlappedResult");
		return dw;
	}
	void f_write(const char* a_p, size_t a_n)
	{
		OVERLAPPED overlapped;
		overlapped.Offset = overlapped.OffsetHigh = 0;
		overlapped.hEvent = v_event;
		while (a_n > 0) {
			if (!WriteFile(v_handle, a_p, a_n, NULL, &overlapped) && GetLastError() != ERROR_IO_PENDING) throw std::runtime_error("WriteFile");
			DWORD dw;
			if (!GetOverlappedResult(v_handle, &overlapped, &dw, TRUE)) throw std::runtime_error("GetOverlappedResult");
			a_p += dw;
			a_n -= dw;
		}
	}
};

}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASS wc;
	wc.style = 0;
	wc.lpfnWndProc = f_process;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = sizeof(LONG_PTR);
	wc.hInstance = hInstance;
	wc.hIcon = NULL;
	wc.hCursor = NULL;
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = f_class();
	if (!RegisterClass(&wc)) return -1;
	WCHAR key[MAX_PATH];
	if (SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, key) != S_OK) return -1;
	std::replace(key, key + std::wcslen(key), L'\\', L'/');
	HANDLE hpipe = CreateNamedPipe((std::wstring(L"\\\\.\\pipe\\xraftim/") + key).c_str(), PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 0, 0, 5000, NULL);
	if (hpipe == INVALID_HANDLE_VALUE) return -1;
	HANDLE hevent = CreateEvent(NULL, TRUE, FALSE, NULL);
	t_device device(hpipe, hevent);
	OVERLAPPED overlapped;
	overlapped.Offset = overlapped.OffsetHigh = 0;
	overlapped.hEvent = hevent;
	if (!ConnectNamedPipe(hpipe, &overlapped)) {
		DWORD error = GetLastError();
		if (error != ERROR_PIPE_CONNECTED && error != ERROR_IO_PENDING) return -1;
	}
	CHAR cs[MAX_PATH];
	if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, SHGFP_TYPE_CURRENT, cs) != S_OK) return -1;
	std::string home = std::string(cs) + "\\xraftim";
f_log("home=%s\n", home.c_str());
	std::string path = home + "\\public";
	std::vector<std::string> publics;
	WIN32_FIND_DATAA wfd;
	HANDLE h = FindFirstFileA((path + "\\*").c_str(), &wfd);
	if (h != INVALID_HANDLE_VALUE) {
		do {
			if ((wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0) continue;
			publics.push_back(path + '\\' + wfd.cFileName);
		} while (FindNextFileA(h, &wfd));
		FindClose(h);
	}
	std::sort(publics.begin(), publics.end());
	t_basic_dictionary dictionary(publics, home + "\\private");
	dictionary.f_load();
	std::vector<HANDLE> handles(1, hevent);
	std::set<DWORD> processes;
	HWND hwnd = CreateWindow(f_class(), NULL, WS_DISABLED | WS_POPUP, 0, 0, 0, 0, NULL, NULL, hInstance, NULL);
	if (hwnd == NULL) return -1;
	SetWindowLongPtr(hwnd, 0, reinterpret_cast<LONG_PTR>(&dictionary));
	while (true) {
		DWORD result = MsgWaitForMultipleObjects(handles.size(), &handles[0], FALSE, INFINITE, QS_ALLINPUT);
		if (result < WAIT_OBJECT_0) {
			break;
		} else if (result == WAIT_OBJECT_0) {
			t_reader<t_device> reader(device);
			int request;
			reader.f_read(request);
f_log("request=%d\n", request);
			DWORD process;
			reader.f_read(process);
			auto i = processes.lower_bound(process);
			if (i == processes.end() || *i != process) {
				handles.push_back(OpenProcess(PROCESS_QUERY_INFORMATION | SYNCHRONIZE, FALSE, process));
				processes.insert(i, process);
			}
			switch (request) {
			case 0:
				dictionary.f_load();
				break;
			case 1:
				dictionary.f_save();
				break;
			case 2:
				{
					size_t n;
					reader.f_read(n);
					size_t okuri;
					reader.f_read(okuri);
					std::vector<wchar_t> entry(n + okuri);
					reader.f_read(reinterpret_cast<char*>(&entry[0]), sizeof(wchar_t) * entry.size());
					std::deque<t_candidate> candidates;
					dictionary.f_search(&entry[0], n, okuri, candidates);
					t_writer<t_device> writer(device);
					writer.f_write(candidates.size());
					for (const auto& x : candidates) {
						writer.f_write(x.v_text.size());
						writer.f_write(reinterpret_cast<const char*>(x.v_text.c_str()), sizeof(wchar_t) * x.v_text.size());
						const std::vector<std::wstring>& annotations = x.v_annotations;
						writer.f_write(annotations.size());
						for (const auto& y : annotations) {
							writer.f_write(y.size());
							writer.f_write(reinterpret_cast<const char*>(y.c_str()), sizeof(wchar_t) * y.size());
						}
					}
				}
				break;
			case 3:
				{
					size_t n;
					reader.f_read(n);
					size_t okuri;
					reader.f_read(okuri);
					std::vector<wchar_t> entry(n + okuri);
					reader.f_read(reinterpret_cast<char*>(&entry[0]), sizeof(wchar_t) * entry.size());
					size_t m;
					reader.f_read(m);
					std::vector<wchar_t> text(m);
					reader.f_read(reinterpret_cast<char*>(&text[0]), sizeof(wchar_t) * m);
					dictionary.f_register(&entry[0], n, okuri, &text[0], m);
				}
				break;
			}
			FlushFileBuffers(hpipe);
			DisconnectNamedPipe(hpipe);
			if (!ConnectNamedPipe(hpipe, &overlapped)) {
				DWORD error = GetLastError();
				if (error != ERROR_PIPE_CONNECTED && error != ERROR_IO_PENDING) break;
			}
		} else if (result < WAIT_OBJECT_0 + handles.size()) {
			auto i = handles.begin() + (result - WAIT_OBJECT_0);
f_log("exited=%d\n", GetProcessId(*i));
			processes.erase(GetProcessId(*i));
			CloseHandle(*i);
			handles.erase(i);
			if (processes.empty()) break;
		} else if (result == WAIT_OBJECT_0 + handles.size()) {
			MSG msg;
			while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) DispatchMessage(&msg);
		} else {
			break;
		}
	}
f_log("saving...\n");
	dictionary.f_save();
f_log("done.\n");
	DestroyWindow(hwnd);
	return 0;
}
