#include <stddef.h>
#include <stdint.h>

#include "acpi.h"
#include "terminal.h"
#include "multiboot.h"
#include "memtest.h"
#include "memory.h"
#include "physicalMemoryManager.h"
#include "kmalloc.h"

extern uint32_t endkernel;
uint32_t* kernelEnd;

void kernel_main(multiboot_info_t* mbd, unsigned int magic){

	terminal_initialize();

    if(magic != MULTIBOOT_BOOTLOADER_MAGIC){
        
        terminal_writestring("Error: Non-multiboot bootloader used");
        return;

    }

	terminal_writestring("Welcome to jhDOS!\n");
    unsigned int cr0Val;
    asm volatile("mov %%cr0, %0" : "=r"(cr0Val));
    if((cr0Val & 1) == 1){
        terminal_writestring("\nCPU is in protected mode\n");
    }
    else{
        terminal_writestring("CPU is in real mode\n");
    }
    terminal_writestring("Kernel ends at: ");
    kernelEnd = &endkernel;
    terminalWriteHexInt((int32_t)kernelEnd);
    terminal_writestring("\n");

    memory_printMMap(mbd);
    
    uint32_t* RSDP = acpi_findRSDP();

    terminal_writestring("RSDP: ");
    terminalWriteHexInt((int32_t)RSDP);
    terminal_writestring("\n");

    terminal_writestring("Initalising physical memory manager...\n");
    uint32_t* physicalMemoryManagerAddress = initialisePhysicalMemory(mbd, kernelEnd);
    if(physicalMemoryManagerAddress == 0){
        
        terminal_writestring("Error in initialising memory\n");
        return;

    }
    
    else{

        terminalWriteHexInt((int32_t)physicalMemoryManagerAddress);
        terminal_writestring("\n");
        /*uint32_t* ptr = *(physicalMemoryManagerAddress);
        terminalWriteHexInt(*(ptr));*/
        uint32_t* page = physicalMemoryManager_getPage();
        terminalWriteHexInt((int32_t)page);
        terminal_writestring("\n");
        physicalMemoryManager_freePage(page);

    }
    
    terminal_writestring("Initialising kmalloc...\n");

    uint32_t* kmallocStart = initialiseKmalloc();

    terminalWriteHexInt((int32_t)kmallocStart);

    terminal_writestring("\n");

    uint32_t* testMemory = kmalloc(20);

    terminalWriteHexInt((int32_t)testMemory);

    terminal_writestring("\n");

    uint32_t* testMemory2 = kmalloc(20);

    terminalWriteHexInt((int32_t)testMemory2);

    terminal_writestring("\n");

    uint32_t* testMemory3 = kmalloc(4070);

    terminalWriteHexInt((int32_t)testMemory3);

    terminal_writestring("\n");

    kfree(testMemory);
   
/*
    if(memCheck(mbd) == 1){
        terminal_writestring("Avaliable low memory: ");
        terminalWriteInt(mbd->mem_lower);
        terminal_writestring("\nAvaliable high memory: ");
        terminalWriteInt(mbd->mem_upper);
        terminal_writestring("\n");
    }
    else{
        terminal_writestring("Memory invalid\n");
    }
*/
}
