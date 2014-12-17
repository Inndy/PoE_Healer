//---------------------------------------------------------------------------
#include <cstring>
#pragma hdrstop

#include "Functions.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

using namespace std;

char inline is_hex(char ch) {
    return ('0' <= ch && ch <= '9') ||
           ('A' <= ch && ch <= 'F') ||
           ('a' <= ch && ch <= 'f');
}

char inline hex2int(char ch) {
    if ('0' <= ch && ch <= '9') {
        return ch - '0';
    } else if ('A' <= ch && ch <= 'F') {
        return ch - 'A' + 0xA;
    } else if ('a' <= ch && ch <= 'f') {
        return ch - 'a' + 0xA;
    } else {
        return '\0';
    }
}

char inline is_fuzzy(char ch) {
    return (ch == '?' || ch == '.' || ch == '*') ? '?' : '\0';
}

void Press(int k)
{
	k = MapVirtualKey(k, MAPVK_VK_TO_VSC);

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

    MemoryRegion::MemoryRegion()
    {
        this->region = NULL;
        this->next_region = NULL;
    }

    MemoryRegion::MemoryRegion(void *region)
    {
        this->region = region;
        this->next_region = NULL;
    }

    DWORD MemoryRegion::QuerySize()
    {
        return this->QuerySize(this->region);
    }

    DWORD MemoryRegion::QuerySize(void *adr)
    {
        MEMORY_BASIC_INFORMATION mbi;
        VirtualQuery(adr, &mbi, sizeof(mbi));
        this->region = adr;
        this->next_region = (void *)((char *)mbi.BaseAddress + mbi.RegionSize);
        return mbi.RegionSize;
    }

    DWORD MemoryRegion::QuerySize(char *adr)
    {
        return this->QuerySize((void*)adr);
    }

    MemoryRegion *MemoryRegion::NextRegion()
    {
        this->region = this->next_region;
        return this;
    }

    // AobScanner

    AobScanner::AobScanner(void *buffer, unsigned long size)
    {
        this->buffer = (char *)buffer;
        this->size = size;
    }

    AobScanner::AobScanner(char *buffer, unsigned long size)
    {
        this->buffer = buffer;
        this->size = size;
    }

    bool AobScanner::Scan(const char *feature, char **out, bool alignment, long offset)
    {
        char *buffer = this->buffer, *end = this->buffer + this->size;
        long feature_length, t = strlen(feature);
        char *feature_bytes = new char[t],
             *feature_mask = new char[t];
        bool found = false;
        int i;

        this->ParseFeature(feature, feature_bytes, feature_mask, &feature_length);

        end -= feature_length;

        while (buffer < end && !found) {
            for (i = 0; i < feature_length; i++) {
                if ((buffer[i] & feature_mask[i]) != feature_bytes[i]) {
                    break;
                }
            }
            if (i == feature_length) {
                found = true;
            } else {
                buffer++;
            }
        }

        delete [] feature_bytes;
        delete [] feature_mask;

        *out = found ? buffer + offset : NULL;
        return found;
    }

    bool AobScanner::ScanOffset(const char *feature, long *out, bool alignment, long offset)
    {
        char *result;
        bool found = this->Scan(feature, &result, alignment, offset);
        *out = result ? result - this->buffer : -1;
        return found;
    }

    void AobScanner::ParseFeature(const char *feature, char *out, char *mask, long *len)
    {
        enum ParseState { LO, HI };
        ParseState state = HI;
        *len = 0;
        while (*feature) {
            bool is_digit = is_hex(*feature);
            if (state == HI) {
                *out = 0;
                *mask = 0;
            }
            if (is_digit) {
                *out |= hex2int(*feature) << (state == HI ? 4 : 0);
                *mask |= state == HI ? 0xF0 : 0x0F;
            }
            if (is_digit || is_fuzzy(*feature)) {
                if (state == LO) {
                    state = HI;
                    out++;
                    mask++;
                    (*len)++;
                } else {
                    state = LO;
                }
            }
            feature++;
        }
    }
}
