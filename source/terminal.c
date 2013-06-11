#ifndef TERMINAL_C
#define TERMINAL_C
#include <stddef.h>
#include <stdint.h>
#include "terminal.h"

#define SPACES_IN_TAB 4
 
uint8_t make_color(enum vga_color fg, enum vga_color bg)
{
    return fg | bg << 4;
}
 
uint16_t make_vgaentry(char c, uint8_t color)
{
    uint16_t c16 = c;
    uint16_t color16 = color;
    return c16 | color16 << 8;
}
 
size_t strlen(const char* str)
{
    size_t ret = 0;
    while ( str[ret] != 0 )
        ret++;
    return ret;
}
 
static const size_t VGA_WIDTH = 80;
static const size_t VGA_HEIGHT = 24;
 
size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer;
 
void terminal_initialize()
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = make_color(COLOR_LIGHT_GREY, COLOR_BLACK);
    terminal_buffer = (uint16_t*) 0xB8000;
    for ( size_t y = 0; y < VGA_HEIGHT; y++ )
    {
        for ( size_t x = 0; x < VGA_WIDTH; x++ )
        {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = make_vgaentry(' ', terminal_color);
        }
    }
}
 
void terminal_setcolor(uint8_t color)
{
    terminal_color = color;
}
 
void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = make_vgaentry(c, color);
}

void terminal_scroll(){
    for(size_t i = 1; i < VGA_HEIGHT; i++){
        for(size_t j = 0; j < VGA_WIDTH; j++){
            size_t index = i + j * VGA_WIDTH;
            terminal_buffer[index] = terminal_buffer[index - VGA_WIDTH];
        }
    }
    for(size_t k = 0; k < VGA_WIDTH; k++){
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + k] = make_vgaentry(' ', terminal_color);   
    }
}
 
void terminal_putchar(char c)
{
    if(c == '\n'){
        terminal_row++;
        terminal_column = 0;
        return;
    }
    else if(c == '\t'){
        for(int i = 0; i < SPACES_IN_TAB; i++){
            terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
            if ( ++terminal_column == VGA_WIDTH )
            {
                terminal_column = 0;
                if ( ++terminal_row == VGA_HEIGHT )
                {
                    terminal_scroll();
                }
                return;
            }
        }
        return;
    }
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if ( ++terminal_column == VGA_WIDTH )
    {
        terminal_column = 0;
        if ( ++terminal_row == VGA_HEIGHT )
        {
            terminal_scroll();
        }
    }
}

void terminalWriteInt(int number){
    char string[12];
    terminal_writestring(intToString(number, string, 10));
}

void terminalWriteHexInt(int number){
    char string[12];
    terminal_writestring("0x");
    terminal_writestring(intToString(number, string, 16));
}

void terminalWriteBinInt(int number){
    char string[33];
    terminal_writestring("0b");
    terminal_writestring(intToString(number, string, 2));
}

char* intToString(int number, char* string, int base){
    int i, sign;
     if ((sign = number) < 0)  /* record sign */
         number = -number;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
        char digit = number % base;
        if(digit < 10){
            string[i++] = digit + '0';   /* get next digit */
        }
        else{
            string[i++] = 'A' + digit - 10;
        }
     } while ((number /= base) > 0);     /* delete it */
     if (sign < 0)
         string[i++] = '-';
     string[i] = '\0';
     reverse(string);
     return string;
}

void reverse(char* string){
    int i, j;
    char c;

    for (i = 0, j = strlen(string)-1; i<j; i++, j--) {
        c = string[i];
        string[i] = string[j];
        string[j] = c;
    }
}
 
void terminal_writestring(const char* data)
{
    size_t datalen = strlen(data);
    for ( size_t i = 0; i < datalen; i++ )
        terminal_putchar(data[i]);
}
#endif
