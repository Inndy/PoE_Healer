//---------------------------------------------------------------------------
#include <windows.h>
#include <vcl.h>
#include <cstring>
#pragma hdrstop

#include "FormMain.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainForm *MainForm;

bool InjectDll(String DllPath, DWORD Pid);
bool InjectDllByWindow(UnicodeString DllPath, HWND hwnd);
bool UpToSky();
//---------------------------------------------------------------------------
__fastcall TMainForm::TMainForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TMainForm::btnInjectClick(TObject *Sender)
{
    HWND hwnd = NULL;
    do {
        hwnd = FindWindowEx(NULL, hwnd, TEXT("Direct3DWindowClass"), TEXT("Path of Exile"));
        InjectDllByWindow(ExtractFilePath(Application->ExeName) + "PoE_Healer.dll", hwnd);
    } while (hwnd != NULL);
    this->Close();
}
//---------------------------------------------------------------------------
bool UpToSky() {
	HANDLE TokenHandle;

	if(!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &TokenHandle))
		return false;

	TOKEN_PRIVILEGES t_privileges = {0};

	if(!LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &t_privileges.Privileges[0].Luid))
		return false;

	t_privileges.PrivilegeCount = 1;
	t_privileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	if(!AdjustTokenPrivileges(TokenHandle, FALSE, &t_privileges, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
		CloseHandle(TokenHandle);
		return false;
	}
	return true;
}
//---------------------------------------------------------------------------
bool InjectDllByWindow(UnicodeString DllPath, HWND hwnd)
{
    DWORD pid = 0;
    if (hwnd == NULL) return false;
    GetWindowThreadProcessId(hwnd, &pid);
    if (pid == NULL) return false;
    return InjectDll(ExtractFilePath(Application->ExeName) + "PoE_Healer.dll", pid);
}
//---------------------------------------------------------------------------
bool InjectDll(UnicodeString DllPath, DWORD Pid)
{
	int l = (DllPath.Length() + 1) * sizeof(wchar_t);
	HANDLE hProcess = OpenProcess(( PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE ), false, Pid);
	if (hProcess == NULL) {
		return false;
	}
	void* RemoteBuffer = VirtualAllocEx(hProcess, NULL, 1024, MEM_COMMIT, PAGE_READWRITE);
	if (RemoteBuffer == NULL) {
		CloseHandle(hProcess);
		return false;
	}
	void* RemoteAddress = (void*)GetProcAddress(GetModuleHandle(L"Kernel32"), "LoadLibraryW");
	wchar_t path[1024] = {0};
	wcscpy(path, DllPath.c_str());
	if (WriteProcessMemory(hProcess, RemoteBuffer, path, l, false) == 0) {
		CloseHandle(hProcess);
		return false;
	}
	HANDLE hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)RemoteAddress, RemoteBuffer, 0, 0);
	if (hProcess == NULL) {
		CloseHandle(hProcess);
		return false;
	}
	WaitForSingleObject(hThread, 0);
	//VirtualFreeEx(hProcess, RemoteBuffer, 0, MEM_RELEASE);
	CloseHandle(hProcess);
	return true;
}
//---------------------------------------------------------------------------
