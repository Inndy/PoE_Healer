//---------------------------------------------------------------------------
#include <cstdio>
#pragma hdrstop

#include "Hook.h"
#include "Functions.h"
#include "Form_Main.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

extern int magic, hp, mp;
extern HWND *pGameHWND;
int FixW7BugX = GetModuleHandle(L"kernelbase.dll") ? GetSystemMetrics(SM_CXSIZEFRAME) / 2 : 0;
int FixW7BugY = GetModuleHandle(L"kernelbase.dll") ? GetSystemMetrics(SM_CYSIZEFRAME) / 2 : 0;

SCRIPT_START(hook_get_magic)
    mov eax, [esi+0x14]
    mov [magic], eax
    cmp eax, ecx
    ret
SCRIPT_END

SCRIPT_START(hook_get_hp)
    mov eax, [esi+0x54]
    mov [hp], eax
    test eax, eax
    ret
SCRIPT_END

SCRIPT_START(hook_get_mp)
    mov eax, [esi+0x78]
    mov [mp], eax
    test eax, eax
    ret
SCRIPT_END



WNDPROC org_WMHandler;

LRESULT WMHandler(HWND hwnd, UINT message,WPARAM wParam,LPARAM lParam)
{
	switch(message)
	{
		case WM_MOVE:
			FormMain->FollowGameWindow();
			break;
		case WM_KEYDOWN:
			switch(wParam)
			{
				case VK_F1:
					FormMain->ShowHelp();
					break;
				case VK_F2:
					FormMain->chkProtect->Checked = !FormMain->chkProtect->Checked;
					break;
                case VK_F4:
                    FormMain->chkPressF->Checked = !FormMain->chkPressF->Checked;
                    break;
				case VK_F12:
					FormMain->Visible = !FormMain->Visible;
					break;
            }
			break;
		case WM_CLOSE:
			TerminateProcess(GetCurrentProcess(), 0);
			break;
	}

	return org_WMHandler(hwnd, message, wParam, lParam);
}

void WriteMemory(void *lpAddress, void *lpBuffer, int lpSize)
{
	DWORD OldProtection;
	VirtualProtect(lpAddress,lpSize,PAGE_EXECUTE_READWRITE, &OldProtection);
	memcpy(lpAddress,lpBuffer,lpSize);
	VirtualProtect(lpAddress,lpSize,OldProtection, &OldProtection);
}

void AsmCall(char * lpAddress, LPCVOID Function, unsigned Nops)
{
	DWORD OldProtection;
	VirtualProtect((LPVOID)lpAddress,10,PAGE_EXECUTE_READWRITE, &OldProtection);
	*(LPBYTE)lpAddress = 0xE8;
	*(LPDWORD)(lpAddress + 1) = (DWORD)Function - (DWORD)lpAddress - 5;
	if ((bool)Nops)
		memset(((LPBYTE)lpAddress + 5), 0x90, Nops);
	VirtualProtect((LPVOID)lpAddress,10,OldProtection, &OldProtection);
}

void MakeHook(HMODULE mod, DWORD offset, LPCVOID script, int nops)
{
    AsmCall((char *)mod + offset, script, nops);
}

bool Hook(HMODULE mod_poe)
{
    // 8b ?? ?? 85 c0 89 44 ?? ?? 0f 9f ?? 88
    long offset_get_hp, offset_get_mp;
    char* adr_get_hwnd;

    Hack::MemoryRegion *mr = new Hack::MemoryRegion(mod_poe);
    long scan_size = mr->QuerySize(); // PE head
    scan_size += mr->NextRegion()->QuerySize(); // text section
    scan_size += mr->NextRegion()->QuerySize(); // data section
    delete mr;

    Hack::AobScanner *scanner = new Hack::AobScanner(mod_poe, scan_size);

    if (!scanner->ScanOffset("8b 46 54 85 c0 89 44 24", &offset_get_hp)) {
        goto fail_scan;
    }
    MakeHook(mod_poe, offset_get_hp , hook_get_hp, 0);

    if (!scanner->ScanOffset("8b 46 78 85 c0 89 44 24", &offset_get_mp)) {
        goto fail_scan;
    }
    MakeHook(mod_poe, offset_get_mp, hook_get_mp, 0);

    if (!scanner->Scan("8b 35 ?? ?? ?? ?? eb ?? 84", &adr_get_hwnd, false, 2)) {
        goto fail_scan;
    }
	pGameHWND = *(HWND **)adr_get_hwnd;

    delete scanner;

#ifdef DEBUG_MODE
    char dump[4096];
    sprintf(dump, "mod_poe = %p\n"
                  "scan_size = %d\n"
                  "offset_get_hp = %p\n"
                  "offset_get_mp = %p\n"
                  "adr_get_hwnd = %p\n",
                  mod_poe,
                  scan_size,
                  offset_get_hp,
                  offset_get_mp,
                  adr_get_hwnd);
    MessageBoxA(NULL, dump, "PoE Healer", MB_ICONINFORMATION);
#endif

	char window_text[1024];
	while (true) {
		GetWindowTextA(*pGameHWND, window_text, sizeof(window_text));
		if (strcmp(window_text, "Path of Exile") == 0) {
			org_WMHandler = (WNDPROC)GetWindowLongW(*pGameHWND, GWL_WNDPROC);
			SetWindowLongW(*pGameHWND, GWL_WNDPROC, (long)WMHandler);
			SetWindowPos(*pGameHWND, 0, FixW7BugX, FixW7BugY, 0, 0,
					     SWP_NOSIZE | SWP_NOSENDCHANGING | SWP_NOZORDER);
			break;
		}
    }
    return true;
fail_scan:
    delete scanner;
    return false;
}
