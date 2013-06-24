#ifndef VIRTUALMEMORYMANAGER_C
#define VIRTUALMEMORYMANAGER_C

#include "virtualMemoryManager.h"
#include "physicalMemoryManager.h"
#include "multiboot.h"

/* Sets up identity paging for the kernel and memory management structures and enables paging */

uint32_t* intialisePaging(multiboot_info_t* mbd, uint32_t numberOfInUsePages){
    
    uint32_t* pageDirectory = physicalMemoryManager_getPage();
    
    for(int i = 0; i < 1024; i++){

        /* attribute: supervisor level, read/write, not present. */
        pageDirectory[i] = 0 | 2;

    }

    /* Identity map the entire memory space */

    /* Memory for VGA terminal */
    /* Attributes: supervisor level, read/write, present */

    mapPage(pageDirectory, (void*)(0xb8000 & 0xfffff000), (void*)(0xb8000 & 0xfffff000), 3);
    
    /* Memory for kernel and physical memory manager */
    
    uint32_t i;
    for(i = 1; i < numberOfInUsePages + 1; i++){
        
        mapPage(pageDirectory, (void*)(0x1000000 + i * 4196), (void*)(0x1000000 + i * 4196), 3);

    }
    
    mapPage(pageDirectory, pageDirectory, pageDirectory, 3);
    
    /* Enable paging */
    
    asm volatile("mov %0, %%cr3":: "b"(pageDirectory));

    uint32_t cr0;
    asm volatile("mov %%cr0, %0": "=b"(cr0));
    cr0 |= 0x80000000;
    asm volatile("mov %0, %%cr0":: "b"(cr0));

    return pageDirectory;

}

/* Insert a map of virtualAddress -> physicalAddress into pageDirectory with attributes flags */

void mapPage(uint32_t* pageDirectory, void* physicalAddress, void* virtualAddress, uint32_t flags){
    
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
