//---------------------------------------------------------------------------

#ifndef FunctionsH
#define FunctionsH
//---------------------------------------------------------------------------
#include <windows.h>

void Press(int k);

namespace Hack {
    class MemoryRegion {
        public:
            DWORD QuerySize();
            DWORD QuerySize(void *adr);
            DWORD QuerySize(char *adr);
        private:
            void *next_region;
    };

    class AobScanner {
        public:
            AobScanner(void *buffer, unsigned long size);
            AobScanner(char *buffer, unsigned long size);
            char *Scan(const char *feature, bool alignment = false, long offset = 0);
            void ParseFeature(const char *feature, char *out, char *mask, long *len);
        private:
            char *buffer;
            unsigned long size;
    };
}
#endif
