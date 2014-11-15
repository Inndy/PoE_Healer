//---------------------------------------------------------------------------

#pragma hdrstop

#include "Functions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

void Press(int k)
{
	k = MapVirtualKey('1' + k, MAPVK_VK_TO_VSC);

	INPUT ip = { 0 };
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = k;
	ip.ki.dwFlags = KEYEVENTF_SCANCODE;

	SendInput(1, &ip, sizeof(ip));
    ip.ki.dwFlags |= KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(ip));
}
