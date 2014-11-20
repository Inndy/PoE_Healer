//---------------------------------------------------------------------------

#ifndef HookH
#define HookH
//---------------------------------------------------------------------------
#include <windows.h>

#define SCRIPT_START(SCRIPT)   void __declspec(naked) SCRIPT () { __asm {
#define SCRIPT_END             }}

bool Hook(HMODULE mod_poe);
extern WNDPROC org_WMHandler;
LRESULT WMHandler(HWND hwnd, UINT message,WPARAM wParam,LPARAM lParam);

#endif
