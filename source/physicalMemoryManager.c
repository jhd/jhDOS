#ifndef PHYSICALMEMORYMANAGER_C
#define PHYSICALMEMORYMANAGER_C

#include "physicalMemoryManager.h"
#include "multiboot.h"
#include <stdint.h>

#define SIZE_OF_STRUCTURE_ENTRY 0.125

/* Functions for managing physical memory 
 * Note that this is currently limited to the contigous block of memory containg the kernel */

uint32_t* initialisePhysicalMemory(multiboot_info_t* mbd, uint32_t* kernelEnd){

    /* Find the memory block containg the kernel */

    memory_map_t* mmap = (memory_map_t*)mbd->mmap_addr;

    uint32_t kernelMemoryBlockStart = 0x100000;
    uint32_t kernelMemoryBlockLength;
    uint32_t kernelSize;

    while((unsigned int)mmap < mbd->mmap_addr + mbd->mmap_length){
        
        if(mmap->base_addr_low == kernelMemoryBlockStart){

            if(mmap->type != 1){
                
                /* Kernel memoryblock is unavaliable */

                return 0;
            }
            
            /* Grub has been told to load the kernel at 0x100000 */

            kernelMemoryBlockLength = mmap->length_low;
            kernelSize = (uint32_t)kernelEnd - kernelMemoryBlockStart;
            break;

        }

        mmap = (memory_map_t*) ( (unsigned int)mmap + mmap->size + sizeof(unsigned int) );
    }

    /* Find next page aligned boundry after the end of the kernel */
    
    uint32_t* nextPageBoundry = kernelEnd + ((uint32_t)kernelEnd % 4196);
    
    if(nextPageBoundry > kernelMemoryBlockStart + kernelMemoryBlockLength){

        /* Next page boundry is outside of avaliable memory */
        /* TODO Attempt to find another suitable area to store the structure */

        return 0;
    }

    /* Check that there is enough space afeter the boundry to store the management structure */

    uint32_t numberOfKernelBlockPages = kernelMemoryBlockLength / 4196 + (kernelMemoryBlockLength % 4196 == 0 ? 0 : 1);
    uint32_t sizeOfKernelInPages = kernelSize / 4196 + (kernelSize % 4196 == 0? 0 : 1);
    uint32_t sizeOfManagementStructure = numberOfKernelBlockPages * sizeof(uint32_t*);
    uint32_t sizeOfManagementStructureInPages = sizeOfManagementStructure / 4196 + (sizeOfManagementStructure % 4196 == 0? 0 : 1);
    uint32_t numberOfAvaliablePages = numberOfKernelBlockPages - sizeOfKernelInPages - sizeOfManagementStructureInPages;


    if(sizeOfManagementStructureInPages > numberOfKernelBlockPages - sizeOfKernelInPages){
        
        /* There is not enough space after the boundry to store the management structure */
        /* TODO Sttempt to find another suitable space for the structure */

        return 0;
    }

    /* Create management structure */
    
    *(nextPageBoundry) = numberOfAvaliablePages + nextPageBoundry;

    /* Initalise management structure */

    uint32_t* nextPageAfterManagementStructure = nextPageBoundry + sizeOfManagementStructureInPages * 4096;
        
    for(uint32_t i = 0; i < numberOfAvaliablePages; i++){

        *(nextPageBoundry + i + 1) = nextPageAfterManagementStructure + i * 4096;

    }

    return (uint32_t)nextPageBoundry;
}

void __physicalMemory_markAsInUse(){

}

#endif