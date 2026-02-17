CC = gcc
CFLAGS = -m32 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs -Wall -Wextra -c -ffreestanding
AS = nasm
ASFLAGS = -f elf32
LD = ld
LDFLAGS = -m elf_i386 -T linker.ld

OBJECTS = boot.o kernel.o graphics.o fs.o window.o mouse.o timer.o process.o switch.o \
          syscall.o syscall_isr.o string.o kheap.o kprintf.o \
          idt.o isr.o irq.o irq_asm.o

all: kernel.bin

boot.o: boot.s
	as --32 boot.s -o boot.o

kernel.o: kernel.c
	$(CC) $(CFLAGS) kernel.c -o kernel.o

graphics.o: graphics.c
	$(CC) $(CFLAGS) graphics.c -o graphics.o

fs.o: fs.c
	$(CC) $(CFLAGS) fs.c -o fs.o

window.o: window.c
	$(CC) $(CFLAGS) window.c -o window.o

mouse.o: mouse.c
	$(CC) $(CFLAGS) mouse.c -o mouse.o

timer.o: timer.c
	$(CC) $(CFLAGS) timer.c -o timer.o

process.o: process.c
	$(CC) $(CFLAGS) process.c -o process.o

switch.o: switch.asm
	$(AS) $(ASFLAGS) switch.asm -o switch.o

syscall.o: syscall.c
	$(CC) $(CFLAGS) syscall.c -o syscall.o

syscall_isr.o: syscall_isr.asm
	$(AS) $(ASFLAGS) syscall_isr.asm -o syscall_isr.o

string.o: string.c
	$(CC) $(CFLAGS) string.c -o string.o

kheap.o: kheap.c
	$(CC) $(CFLAGS) kheap.c -o kheap.o

kprintf.o: kprintf.c
	$(CC) $(CFLAGS) kprintf.c -o kprintf.o

idt.o: idt.c
	$(CC) $(CFLAGS) idt.c -o idt.o

isr.o: isr.c
	$(CC) $(CFLAGS) isr.c -o isr.o

irq.o: irq.c
	$(CC) $(CFLAGS) irq.c -o irq.o

irq_asm.o: irq.asm
	$(AS) $(ASFLAGS) irq.asm -o irq_asm.o

kernel.bin: $(OBJECTS)
	$(LD) $(LDFLAGS) -o kernel.bin $(OBJECTS)

clean:
	rm -f $(OBJECTS) kernel.bin

run: kernel.bin
	qemu-system-i386 -kernel kernel.bin -m 256