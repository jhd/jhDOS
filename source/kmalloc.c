#ifndef KMALLOC_C
#define KMALLOC_C

#include "physicalMemoryManager.h"
#include <stddef.h>

uint32_t* kmallocStartPage;
uint32_t* nextFreeBlock;

uint32_t* initialiseKmalloc(){

    kmallocStartPage = physicalMemoryManager_getPage();

    if(kmallocStartPage == NULL){

        return NULL;

    }

    kmallocStartPage[0] = -4084;
    kmallocStartPage[1] = 0;
    kmallocStartPage[2] = 0;

    nextFreeBlock = kmallocStartPage;

    return kmallocStartPage;

}

void* kmalloc(uint32_t bytes){
    
    uint32_t* currentBlock = nextFreeBlock;

    while(*(currentBlock + 2) != (uint32_t)NULL){

        /* Until we reach the end of allocated pages */

        if(*(currentBlock) * -1 > bytes + 3){

            /* The next free block has enough space to allocate bytes */

            *((uint32_t*)*(currentBlock + 2)) = (uint32_t)(currentBlock + bytes + 3);
            
            /* Size of free block = previous size - (bytes + 3 * int) */

            *(currentBlock + bytes + 3) = *currentBlock + (bytes + 3);

            /* Next Block.prev = currentBlock */

            *(currentBlock + bytes + 4) = *currentBlock;

            /*Next block.next = current block.next */

            *(currentBlock + bytes + 5) = *(currentBlock + 2);

            *currentBlock = bytes + 3;

            *(currentBlock + 2) = (uint32_t)currentBlock + bytes + 3;

            return (void*)((uint32_t)currentBlock + 3);

        }

    }

    return NULL;

}

#endif
