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

namespace Hack
{
    // MemoryRegion

    DWORD MemoryRegion::QuerySize()
    {
        return this->QuerySize(this->next_region);
    }

    DWORD MemoryRegion::QuerySize(void *adr)
    {
        MEMORY_BASIC_INFORMATION mbi;
        VirtualQuery(adr, &mbi, sizeof(mbi));
        this->next_region = (void *)((char *)mbi.BaseAddress + mbi.RegionSize);
        return mbi.RegionSize;
    }

    DWORD MemoryRegion::QuerySize(char *adr)
    {
        return this->QuerySize((void*)adr);
    }
}
