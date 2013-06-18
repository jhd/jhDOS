CC = i586-elf-gcc
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -s -g
LINKSCRIPT = linker.ld

SRC = $(wildcard *.c)
OBJS = $(wildcard *.o)

all: $(src)
	$(CC) -c $(SRC) $(CFLAGS)
	$(CC) -T $(LINKSCRIPT) -o jhdos.bin -ffreestanding -O2 -nostdlib $(OBJS) -lgcc
	cp jhdos.bin isodir/boot/jhdos.bin
	grub-mkrescue -o jhdos.iso isodir


iso:
	cp jhdos.bin isodir/boot/jhdos.bin
	grub-mkrescue -o jhdos.iso isodir
