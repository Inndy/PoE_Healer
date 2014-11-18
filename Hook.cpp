//---------------------------------------------------------------------------

#pragma hdrstop

#include "Hook.h"
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

void Hook(HMODULE mod_poe)
{
    // MakeHook(mod_poe, 0x312D9A, hook_get_magic, 0);
    MakeHook(mod_poe, 0xDC806 , hook_get_hp, 0);
    MakeHook(mod_poe, 0x0FC7C6, hook_get_mp, 0);

	pGameHWND = (HWND *)((char *)mod_poe + 0x9A43F8);

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
}
