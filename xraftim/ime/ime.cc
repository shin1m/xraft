#include "context.h"
#include "ui.h"

#pragma comment(linker, "/EXPORT:ImeInquire=_ImeInquire@12,PRIVATE")
#pragma comment(linker, "/EXPORT:ImeConfigure=_ImeConfigure@16,PRIVATE")
#pragma comment(linker, "/EXPORT:ImeConversionList=_ImeConversionList@20,PRIVATE")
#pragma comment(linker, "/EXPORT:ImeDestroy=_ImeDestroy@4,PRIVATE")
#pragma comment(linker, "/EXPORT:ImeEscape=_ImeEscape@12,PRIVATE")
#pragma comment(linker, "/EXPORT:ImeProcessKey=_ImeProcessKey@16,PRIVATE")
#pragma comment(linker, "/EXPORT:ImeSelect=_ImeSelect@8,PRIVATE")
#pragma comment(linker, "/EXPORT:ImeSetActiveContext=_ImeSetActiveContext@8,PRIVATE")
#pragma comment(linker, "/EXPORT:ImeToAsciiEx=_ImeToAsciiEx@24,PRIVATE")
#pragma comment(linker, "/EXPORT:NotifyIME=_NotifyIME@16,PRIVATE")
#pragma comment(linker, "/EXPORT:ImeRegisterWord=_ImeRegisterWord@12,PRIVATE")
#pragma comment(linker, "/EXPORT:ImeUnregisterWord=_ImeUnregisterWord@12,PRIVATE")
#pragma comment(linker, "/EXPORT:ImeGetRegisterWordStyle=_ImeGetRegisterWordStyle@8,PRIVATE")
#pragma comment(linker, "/EXPORT:ImeEnumRegisterWord=_ImeEnumRegisterWord@20,PRIVATE")
#pragma comment(linker, "/EXPORT:ImeSetCompositionString=_ImeSetCompositionString@24,PRIVATE")
#pragma comment(linker, "/EXPORT:ImeGetImeMenuItems=_ImeGetImeMenuItems@24,PRIVATE")

HINSTANCE v_hinstance;

BOOL WINAPI DllMain(HINSTANCE hInstDLL, DWORD dwFunction, LPVOID lpNot)
{
	switch (dwFunction) {
	case DLL_PROCESS_ATTACH:
f_log("DLL_PROCESS_ATTACH");
		v_hinstance = hInstDLL;
		t_ui::f_register();
		t_component::f_register();
		break;
        case DLL_PROCESS_DETACH:
f_log("DLL_PROCESS_DETACH");
		t_ui::f_register();
		t_component::f_unregister();
		break;
        case DLL_THREAD_ATTACH:
		break;
        case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

BOOL WINAPI ImeInquire(LPIMEINFO lpIMEInfo, LPTSTR lpszClassName, DWORD dwSystemInfoFlags)
{
f_log("ImeInquire");
	lpIMEInfo->dwPrivateDataSize = sizeof(t_ime_engine);
	lpIMEInfo->fdwProperty = IME_PROP_AT_CARET | IME_PROP_UNICODE | IME_PROP_END_UNLOAD | IME_PROP_KBD_CHAR_FIRST;
	lpIMEInfo->fdwConversionCaps = IME_CMODE_LANGUAGE | IME_CMODE_FULLSHAPE | IME_CMODE_ROMAN;
	lpIMEInfo->fdwSentenceCaps = IME_SMODE_SINGLECONVERT | IME_SMODE_AUTOMATIC;
	lpIMEInfo->fdwUICaps = UI_CAP_2700;
	lpIMEInfo->fdwSCSCaps = 0;
	lpIMEInfo->fdwSelectCaps = SELECT_CAP_CONVERSION;
	lstrcpy(lpszClassName, t_ui::f_class());
	return TRUE;
}

BOOL WINAPI ImeConfigure(HKL hKL, HWND hWnd, DWORD dwMode, LPVOID lpData)
{
f_log("ImeConfigure");
	return TRUE;
}

DWORD WINAPI ImeConversionList(HIMC hIMC, LPCTSTR lpSource, LPCANDIDATELIST lpCandList, DWORD dwBufLen, UINT uFlags)
{
f_log("ImeConversionList");
	return 0;
}

BOOL WINAPI ImeDestroy(UINT uForce)
{
f_log("ImeDestroy");
	return TRUE;
}

LRESULT WINAPI ImeEscape(HIMC hIMC, UINT uSubFunc, LPVOID lpData)
{
f_log("ImeEscape");
	return FALSE;
}

BOOL WINAPI ImeProcessKey(HIMC hIMC, UINT vKey, LPARAM lKeyData, CONST LPBYTE lpbKeyState)
{
f_log("ImeProcessKey");
	if ((lKeyData & 0x80000000) != 0) return FALSE;
	t_imc imc(hIMC);
	if (!imc || !imc->fOpen) return FALSE;
	return (lpbKeyState[VK_MENU] & 0x80) == 0 ? TRUE : FALSE;
}

BOOL WINAPI ImeSelect(HIMC hIMC, BOOL fSelect)
{
f_log("ImeSelect");
	if (!hIMC) return TRUE;
	t_imc imc(hIMC);
	if (!imc) return TRUE;
	if (fSelect) {
		if ((imc->fdwInit & INIT_CONVERSION) == 0) {
			imc->fdwConversion = IME_CMODE_NATIVE | IME_CMODE_FULLSHAPE | IME_CMODE_ROMAN;
			imc->fdwInit |= INIT_CONVERSION;
		}
		if ((imc->fdwInit & INIT_SENTENCE) == 0) {
			imc->fdwSentence = IME_SMODE_SINGLECONVERT;
			imc->fdwInit |= INIT_SENTENCE;
		}
		t_imcc<void> engine(imc->hPrivate);
		if (engine) new(engine) t_ime_engine(hIMC);
	} else {
		t_imcc<t_ime_engine> engine(imc->hPrivate);
		if (engine) engine->~t_ime_engine();
	}
	return TRUE;
}

BOOL WINAPI ImeSetActiveContext(HIMC hIMC, BOOL fFlag)
{
f_log("ImeSetActiveContext");
	return TRUE;
}

UINT WINAPI ImeToAsciiEx(UINT uVKey, UINT uScanCode, CONST LPBYTE lpbKeyState, LPTRANSMSGLIST lpTransBuf, UINT fuState, HIMC hIMC)
{
f_log("ImeToAsciiEx");
	if ((uScanCode & 0x8000) != 0) return 0;
	t_imc imc(hIMC);
	if (!imc || !imc->fOpen) return 0;
	t_imcc<t_ime_engine> engine(imc->hPrivate);
	return engine->f_key_pressed(uVKey, uScanCode, lpbKeyState, lpTransBuf);
}

BOOL WINAPI NotifyIME(HIMC hIMC, DWORD dwAction, DWORD dwIndex, DWORD dwValue)
{
f_log("NotifyIME");
	switch(dwAction) {
	case NI_CONTEXTUPDATED:
f_log("\tNI_CONTEXTUPDATED");
		switch (dwValue) {
		case IMC_SETCANDIDATEPOS:
f_log("\t\tIMC_SETCANDIDATEPOS");
			return FALSE;
		case IMC_SETCOMPOSITIONFONT:
f_log("\t\tIMC_SETCOMPOSITIONFONT");
			return FALSE;
		case IMC_SETCOMPOSITIONWINDOW:
f_log("\t\tIMC_SETCOMPOSITIONWINDOW");
			return FALSE;
		case IMC_SETCONVERSIONMODE:
f_log("\t\tIMC_SETCONVERSIONMODE");
			return FALSE;
		case IMC_SETSENTENCEMODE:
f_log("\t\tIMC_SETSENTENCEMODE");
			return FALSE;
		case IMC_SETOPENSTATUS:
f_log("\t\tIMC_SETOPENSTATUS");
			{
				t_imc imc(hIMC);
				if (!imc) return FALSE;
				t_imcc<t_ime_engine> engine(imc->hPrivate);
				engine->f_switch();
				return TRUE;
			}
		default:
			return FALSE;
		}
	case NI_COMPOSITIONSTR:
f_log("\tNI_COMPOSITIONSTR");
		switch (dwIndex) {
		case CPS_COMPLETE:
f_log("\t\tCPS_COMPLETE");
			return TRUE;
		case CPS_CONVERT:
f_log("\t\tCPS_CONVERT");
			return TRUE;
		case CPS_REVERT:
f_log("\t\tCPS_REVERT");
			return TRUE;
		case CPS_CANCEL:
f_log("\t\tCPS_CANCEL");
			return TRUE;
		default:
			return FALSE;
		}
	case NI_OPENCANDIDATE:
f_log("\tNI_OPENCANDIDATE");
		return FALSE;
	case NI_CLOSECANDIDATE:
f_log("\tNI_CLOSECANDIDATE");
		return FALSE;
	case NI_SELECTCANDIDATESTR:
f_log("\tNI_SELECTCANDIDATESTR");
		return FALSE;
	case NI_CHANGECANDIDATELIST:
f_log("\tNI_CHANGECANDIDATELIST");
		return FALSE;
	case NI_SETCANDIDATE_PAGESIZE:
f_log("\tNI_SETCANDIDATE_PAGESIZE");
		return FALSE;
	case NI_SETCANDIDATE_PAGESTART:
f_log("\tNI_SETCANDIDATE_PAGESTART");
		return FALSE;
	case NI_IMEMENUSELECTED:
f_log("\tNI_IMEMENUSELECTED");
		return FALSE;
	default:
		return FALSE;
	}
}

BOOL WINAPI ImeRegisterWord(LPCTSTR lpRead, DWORD dw, LPCTSTR lpStr)
{
f_log("ImeRegisterWord");
	return FALSE;
}

BOOL WINAPI ImeUnregisterWord(LPCTSTR lpRead, DWORD dw, LPCTSTR lpStr)
{
f_log("ImeUnregisterWord");
	return FALSE;
}

UINT WINAPI ImeGetRegisterWordStyle(UINT u, LPSTYLEBUF lp)
{
f_log("ImeGetRegisterWordStyle");
	return 0;
}

UINT WINAPI ImeEnumRegisterWord(REGISTERWORDENUMPROC lpfn, LPCTSTR lpRead, DWORD dw, LPCTSTR lpStr, LPVOID lpData)
{
f_log("ImeEnumRegisterWord");
	return 0;
}

BOOL WINAPI ImeSetCompositionString(HIMC hIMC, DWORD dwIndex, LPVOID lpComp, DWORD dwComp, LPVOID lpRead, DWORD dwRead)
{
f_log("ImeSetCompositionString");
	return FALSE;
}

extern "C" DWORD WINAPI ImeGetImeMenuItems(HIMC hIMC, DWORD dwFlags, DWORD dwType, LPIMEMENUITEMINFO lpImeParentMenu, LPIMEMENUITEMINFO lpImeMenu, DWORD dwSize)
{
f_log("ImeGetImeMenuItems");
	return 0;
}
