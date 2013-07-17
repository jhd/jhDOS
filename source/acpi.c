#ifndef ACPI_C
#define ACPI_C

#include "acpi.h"
#include "memcmp.h"

RSDPDescriptor* acpi_findRSDP(){
    
    for(uint32_t i = 0xe0000; i < 0xfffff; i += 4){

        if(memcmp((char*)i, "RSD PTR ", 8) == 0){

            return (RSDPDescriptor*)i;

        }

    }

/*
    for(uint32_t i = 0xe0000; i < 0xf0000; i += 4){

        if(memcmp((char*)i, "RSD PTR ", 8) == 0){

            return (RSDPDescriptor*)i;

        }

    }
*/

    return NULL;

}

#endif
