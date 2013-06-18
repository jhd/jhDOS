#include <stddef.h>
#include <stdint.h>

#include "terminal.h"
#include "multiboot.h"
#include "memtest.h"
#include "memory.h"
#include "physicalMemoryManager.h"

extern uint32_t endkernel;
uint32_t* kernelEnd;

void kernel_main(multiboot_info_t* mbd, unsigned int magic){
	terminal_initialize();
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
    terminalWriteHexInt(kernelEnd);
    terminal_writestring("\n");

    memory_printMMap(mbd);

    terminal_writestring("Initalising physical memory manager...\n");
    uint32_t* physicalMemoryManagerAddress = initialisePhysicalMemory(mbd, kernelEnd);
    if(physicalMemoryManagerAddress == 0){
        
        terminal_writestring("Error in initialising memory\n");
        return;

    }
    
    else{

        terminalWriteHexInt(physicalMemoryManagerAddress);
        terminal_writestring("\n");
        /*uint32_t* ptr = *(physicalMemoryManagerAddress);
        terminalWriteHexInt(*(ptr));*/
        uint32_t* page = physicalMemoryManager_getPage();
        terminalWriteHexInt(page);
        terminal_writestring("\n");
        physicalMemoryManager_freePage(page);

    }

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
