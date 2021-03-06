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

                nextFreeBlock = (uint32_t*)*(currentBlock + 2);

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
    
    uint32_t* nextBlock = (uint32_t*)((uint32_t)newPage + bytes + 12);

    *newPage = bytes + 6 * sizeof(uint32_t);

    *(newPage + 1) = (uint32_t)previousBlock;

    *(newPage + 2) = (uint32_t)nextBlock;

    *(nextBlock) = -4096 + 6 * sizeof(uint32_t) + bytes;

    *(nextBlock + 1) = (uint32_t)newPage;

    *(nextBlock + 2) = (uint32_t)NULL;

    return (void*)((uint32_t)newPage + 3 * sizeof(uint32_t));

}

uint32_t kfree(void* address){

    char* memory = (char*)address;

    uint32_t* previousBlock = *(memory - 8);
    uint32_t* nextBlock = *(memory - 4);

    if((uint32_t)previousBlock & 0x1000 != (uint32_t)memory & 0x1000 && (uint32_t)nextBlock & 0x1000 != (uint32_t)memory & 0x1000){

        /* If this is the last block on the page then free the page */

        physicalMemoryManager_freePage((uint32_t*)((uint32_t)memory & 0x1000));

        *(previousBlock + 2) = (uint32_t*)nextBlock;

        *(nextBlock + 1) = (uint32_t*)previousBlock;

        return 1;

    }

    if(*previousBlock > 0 && *nextBlock > 0){

        /* The block is surrounded by in use blocks */

        *(memory - 12) = *(memory - 12) * -1 + 12;

        return 1;

    }

    if(*previousBlock <= 0 && *nextBlock > 0){

        /* The previous block is free and the next block is in use */

        if((uint32_t)previousBlock & 0x1000 != (uint32_t)memory & 0x1000){

            /* The previous block is in another page so the blocks can't be combined */

            *(memory - 12) = (*(memory - 12) * -1 + 12);
            
            return 1;

        }

        *(previousBlock + 2) = (uint32_t*)nextBlock;
        *(nextBlock + 1) = (uint32_t*)previousBlock;
        *previousBlock = *previousBlock - *memory;

        return 1;

    }

    if(*previousBlock > 0 && *nextBlock <= 0){

        /* The previous block is in use and the next block is free */

        if((uint32_t)nextBlock & 0x1000 != (uint32_t)memory & 0x1000){

            /* The next block is in another page so the blocks can't be combined */

            *(memory - 12) = *(memory - 12) * -1 + 12;
            
            return 1;

        }

        *(memory - 12 + 2*sizeof(uint32_t)) = (uint32_t*)*(nextBlock + 2);
        *((uint32_t*)((uint32_t)*((uint32_t*)((uint32_t)nextBlock + 2)) + 1)) = (uint32_t*)((uint32_t)memory - 12);
        *(memory - 12) = *(memory - 12) * -1 + 12;
        *memory = *memory + *nextBlock;

        return 1;

    }

    if(*previousBlock <= 0 && *nextBlock <= 0){

        /* The previous and next blocks are free */

        return 1;

    }

    return 0;

}

#endif
