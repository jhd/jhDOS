#ifndef MEMORY_C
#define MEMORY_C

#include "memory.h"
#include "terminal.h"
#include "multiboot.h"

void memory_printMMap(multiboot_info_t* mbd){
    if((mbd->flags & 2 << 4) == 1){
        terminal_writestring("Error: No MMap avaliable\n");
        return;
    }

    terminal_writestring("GRUB MMAP:\nStart:");
    terminalWriteHexInt(mbd->mmap_addr);
    terminal_writestring("\nLength:");
    terminalWriteInt(mbd->mmap_length);
    terminal_writestring("\n");
    
    memory_map_t* mmap = (memory_map_t*)mbd->mmap_addr;

    while((unsigned int)mmap < mbd->mmap_addr + mbd->mmap_length){
        memory_printMMapEntry(mmap);
        mmap = (memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(unsigned int) );
    }
}

void memory_printMMapEntry(memory_map_t* mmap){
    terminalWriteHexLong(mmap->base_addr_high, mmap->base_addr_low);
    terminal_writestring(" : ");
    terminalWriteHexLong(mmap->length_high, mmap->length_low);
    terminal_writestring(" : ");

    terminalWriteInt(mmap->type);
    terminal_writestring("\n");
}

#endif
