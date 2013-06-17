#ifndef MEMTEST_C
#define MEMTEST_C

#include "memtest.h"
#include "multiboot.h"

int memCheck(multiboot_info_t* mbd){
    if((mbd->flags & 1) == 1){
        return 1;
    }
    return 0;
}

#endif
