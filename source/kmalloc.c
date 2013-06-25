#ifndef KMALLOC_C
#define KMALLOC_C

#include "physicalMemoryManager.h"
#include "kmalloc.h"

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

    if(bytes > 4096 - 6 * sizeof(uint32_t)){

        return NULL;

    }
    
    uint32_t* currentBlock = nextFreeBlock;

    uint32_t* previousBlock = NULL;

    /* While we are not at the last block */

    while(*(currentBlock) != (uint32_t)NULL){

        /* Until we reach the end of allocated pages */

        if((int32_t)*(currentBlock) * -1 > (int32_t)(bytes + 12)){

            /* The next free block has enough space to allocate bytes */

            uint32_t* nextBlock = (uint32_t*)((uint32_t)currentBlock + bytes + 12);

            //*((uint32_t*)*(currentBlock + 2)) = (uint32_t)(currentBlock + bytes + 12);
            
            /* Size of free block = previous size - (bytes + 3 * int) */

            *(nextBlock) = *currentBlock + (bytes + 12);

            /* Next Block.prev = currentBlock */

            *(nextBlock + 1) = (uint32_t)currentBlock;

            /*Next block.next = current block.next */

            *(nextBlock + 2) = *(currentBlock + 2);

            *currentBlock = bytes + 12;

            *(currentBlock + 2) = (uint32_t)nextBlock;

            if(currentBlock == nextFreeBlock){

                nextFreeBlock = *(currentBlock + 2);

            }

            return (void*)((uint32_t)currentBlock + 12);

        }

        previousBlock = currentBlock;

        currentBlock = (uint32_t*)*(uint32_t*)((uint32_t)currentBlock + 2);

    }

    /* Get new page and use that to allocate the memory */

    uint32_t* newPage = physicalMemoryManager_getPage();

    if(newPage == NULL){

        return NULL;

    }

    *(previousBlock + 2) = (uint32_t)newPage;
    
    *newPage = 4096 - (bytes + 6 * sizeof(uint32_t));

    *(newPage + 1) = (uint32_t)previousBlock;

    *(newPage + 2) = (uint32_t)NULL;

    *(newPage + bytes + 3) = -4096 + 6 + bytes;

    *(newPage + bytes + 4) = (uint32_t)newPage;

    *(newPage + bytes + 5) = (uint32_t)NULL;

    return (void*)((uint32_t)newPage + 3);

}

#endif
