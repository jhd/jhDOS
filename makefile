CC = i586-elf-gcc
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra
LINKSCRIPT = source/linker.ld

SRC = $(wildcard source/*.c)
OBJS = $(SRCS:.c=.o)

all: $(SRC)
	$(CC) -c $^ $(CFLAGS)
	$(CC) -t $(LINKSCRIPT) -o source/jhdos.bin -ffreestanding -O2 -nostdlib $(OBJS) -lgcc
