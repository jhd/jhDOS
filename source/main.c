#include <stddef.h>
#include <stdint.h>

#include "terminal.h"

void kernel_main(){
	terminal_initialize();
	terminal_writestring("Welcome to jhDOS!\n\tTabs you say?\nNumber three thousand: ");
    terminalWriteInt(3000);
    terminal_writestring("! : ");
    terminalWriteHexInt(3000);
    terminal_writestring("! : ");
    terminalWriteBinInt(3000);
    terminal_writestring("!\n");

    unsigned int cr0Val;
    asm volatile("mov %%cr0, %0" : "=r"(cr0Val));
    if((cr0Val & 1) == 1){
        terminal_writestring("CPU is in protected mode\n");
    }
    else{
        terminal_writestring("CPU is in real mode\n");
    }
}
