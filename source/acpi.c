#ifndef ACPI_C
#define ACPI_C

#include "acpi.h"
#include "memcmp.h"
#include "terminal.h"

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

void acpi_printRSDP(RSDPDescriptor* RSDP){

    terminal_writestring("OEM: ");

    for(uint32_t i = 0; i < 6; i++){

        terminal_putchar(RSDP->OEMID[i]);

    }

    terminal_writestring("\n");

    terminal_writestring("Revision: ");
    terminalWriteInt(RSDP->Revision);
    terminal_writestring("\n");

    terminal_writestring("RSDT Address: ");
    terminalWriteHexInt(RSDP->RsdtAddress);
    terminal_writestring("\n");

    return;

}

#endif
