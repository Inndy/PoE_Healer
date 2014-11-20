//---------------------------------------------------------------------------

#ifndef FunctionsH
#define FunctionsH
//---------------------------------------------------------------------------
#include <windows.h>

void Press(int k);

namespace Hack {
    class MemoryRegion {
        public:
            MemoryRegion();
            MemoryRegion(void *region);
            DWORD QuerySize();
            DWORD QuerySize(void *adr);
            DWORD QuerySize(char *adr);
            MemoryRegion *NextRegion();
        private:
            void *region;
            void *next_region;
    };

    class AobScanner {
        public:
            AobScanner(void *buffer, unsigned long size);
            AobScanner(char *buffer, unsigned long size);
            bool Scan(const char *feature, char **out, bool alignment = false, long offset = 0);
            bool ScanOffset(const char *feature, long *out, bool alignment = false, long offset = 0);
            void ParseFeature(const char *feature, char *out, char *mask, long *len);

        private:
            char *buffer;
            unsigned long size;
    };
}
#endif
