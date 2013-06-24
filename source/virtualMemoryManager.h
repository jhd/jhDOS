#ifndef VIRTUALMEMORYMANAGER_H
#define VIRTUALMEMORYMANAGER_H

#include <stdint.h>
#include "multiboot.h"

    uint32_t* intialisePaging(multiboot_info_t* mbd, uint32_t numberOfInUsePages);
    void mapPage(uint32_t* pageDirectory, void* physicalAddress, void* virtualAddress, uint32_t flags);

#endif
