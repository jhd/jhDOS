#ifndef MEMCMP_C
#define MEMCMP_C

#include "memcmp.h"

int32_t memcmp(void* dest, void* src, uint32_t count){
    char* dst8 = (char*)dest;
    char* src8 = (char*)src;

    while (count--) {

        if(*dst8 != *src8){

            if(*dst8 > *src8){

                return 1;

            }

            else{

                return -1;

            }

        }

        dst8++;
        src8++;

    }

    return 0;
}

#endif
