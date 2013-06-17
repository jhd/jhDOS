#ifndef PHYSICALMEMORYMANAGER_C
#define PHYSICALMEMORYMANAGER_C

#include "physicalMemoryManager.h"
#include "multiboot.h"
#include <stdint.h>

#define SIZE_OF_STRUCTURE_ENTRY 0.125

/* Functions for managing physical memory 
 * Note that this is currently limited to the contigous block of memory containg the kernel */

int initialisePhysicalMemory(multiboot_info_t* mbd, uint32_t* kernelEnd){

    /* Find the memory block containg the kernel */

    memory_map_t* mmap = (memory_map_t*)mbd->mmap_addr;

    uint32_t kernelMemoryBlockStart = 0x100000;
    uint32_t kernelMemoryBlockLength;
    uint32_t kernelSize;

    while((unsigned int)mmap < mbd->mmap_addr + mbd->mmap_length){
        
        if(mmap->base_addr_low == kernelMemoryBlockStart){

            if(mmap->type != 1){
                
                /* Kernel memoryblock is unavaliable */

                return -1;
            }
            
            /* Grub has been told to load the kernel at 0x100000 */

            kernelMemoryBlockLength = mmap->length_low;
            kernelSize = (uint32_t)kernelEnd - kernelMemoryBlockStart;
            break;

        }

        mmap = (memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(unsigned int) );
    }

    /* Find next page aligned boundry after the end of the kernel */
    
    uint32_t* nextPageBoundry = kernelEnd + ((uint32_t)kernelEnd % 0x4000);
    
    if(nextPageBoundry > kernelMemoryBlockStart + kernelMemoryBlockLength){

        /* Next page boundry is outside of avaliable memory */
        /* TODO Attempt to find another suitable area to store the structure */

        return -1;
    }

    /* Check that there is enough space afeter the boundry to store the management structure */

    uint32_t numberOfKernelBlockPages = kernelMemoryBlockLength / 0x4000 + (kernelMemoryBlockLength % 0x4000 == 0 ? 0 : 1);
    uint32_t sizeOfKernelInPages = kernelSize / 0x4000 + (kernelSize % 0x4000 == 0? 0 : 1);
    uint32_t sizeOfManagementStructure = sizeOfKernelInPages * SIZE_OF_STRUCTURE_ENTRY + sizeof(uint32_t);
    uint32_t sizeOfManagementStructureInPages = sizeOfManagementStructure / 0x4000 + (sizeOfManagementStructure % 0x4000 == 0? 0 : 1);

    if(sizeOfManagementStructureInPages > numberOfKernelBlockPages - sizeOfKernelInPages){
        
        /* There is not enough space after the boundry to store the management structure */
        /* TODO Sttempt to find another suitable space for the structure */

        return -1;
    }

    /* Create management structure */

    *nextPageBoundry = numberOfKernelBlockPages;
    for(uint32_t i = 0; i < numberOfKernelBlockPages / 32 + (numberOfKernelBlockPages % 32 == 0? 0 : 1); i += 4){

        nextPageBoundry[i + 4] = 0;

    }

    /* Initalise management structure */
    
    /* All memory from kernelMemoryBlockStart to nextPageBoundry + 1 is in use */
    
    uint32_t numberOfInUsePages = sizeOfKernelInPages + sizeOfManagementStructureInPages;

    for(uint32_t j = 0; j < numberOfInUsePages; j++){

        /* Mark as in use the page starting at kernelMemoryBlockStart + j * 0x4000.
         * This includes all of the kernel and the physical memory mangement structure */

        __physicalMemory_markAsInUse(j);

    }

    return (int)nextPageBoundry;
}

#endif
