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
}
#endif
