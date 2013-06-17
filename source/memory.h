#ifndef MEMORY_H
#define MEMORY_H

#include "multiboot.h"

    void memory_printMMap(multiboot_info_t* mbd);
    void memory_printMMapEntry(memory_map_t* mmap);

#endif
