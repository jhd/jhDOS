#ifndef PYHSICALMEMORYMANAGER_H
#define PHYSICALMEMORYMANAGER_H

#include "multiboot.h"
#include <stdint.h>

    int initialisePhysicalMemory(multiboot_info_t* mbd, uint32_t* kernelEnd);

#endif
