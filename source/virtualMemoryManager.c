#ifndef VIRTUALMEMORYMANAGER_C
#define VIRTUALMEMORYMANAGER_C

#include "virtualMemoryManager.h"
#include "physicalMemoryManager.h"
#include "multiboot.h"

/* Sets up identity paging for the kernel and memory management structures and enables paging */

uint32_t* virtualMemoryManager_intialisePaging(multiboot_info_t* mbd, uint32_t* endOfInUsePages){
    
    uint32_t* pageDirectory = physicalMemoryManager_getPage();
    
    for(uint32_t i = 0; i < 1024; i++){

        /* attribute: supervisor level, read/write, not present. */
        pageDirectory[i] = 0 | 2;

    }

    /* Identity map the entire memory space */

    /* Memory for kernel and physical memory manager */
    
    for(uint32_t i = 0; i < endOfInUsePages; i += 4096){
        
        mapPage(pageDirectory, (void*)(i), (void*)(i), 3);

    }

    /*Map the pageDirectory into itself */
    
    mapPage(pageDirectory, pageDirectory, endOfInUsePages, 3);
    
    /* Enable paging */
    
    asm volatile("mov %0, %%cr3":: "b"(pageDirectory));

    uint32_t cr0;
    asm volatile("mov %%cr0, %0": "=b"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0":: "b"(cr0));

    return pageDirectory;

}

/* Insert a map of virtualAddress -> physicalAddress into pageDirectory with attributes flags */

void virtualMemoryManager_mapPage(uint32_t* pageDirectory, void* physicalAddress, void* virtualAddress, uint32_t flags){
    
    uint32_t pageDirectoryIndex = (uint32_t)virtualAddress >> 22;
    uint32_t pageTableIndex = (uint32_t)virtualAddress >> 12 & 0x03ff;

    uint32_t* pageTable = (uint32_t*)pageDirectory[pageDirectoryIndex];

    if(((uint32_t)pageTable & 1) != 1){
        
        /* If the pageDirectory entry is not present */

        pageDirectory[pageDirectoryIndex] = (uint32_t)physicalMemoryManager_getPage() | flags;

    }

    pageTable[pageTableIndex] = (uint32_t)physicalAddress | flags;

}

#endif
