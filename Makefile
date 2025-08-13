# Makefile para o Sistema Operacional Kernel-V
CC = gcc
ASM = nasm
LD = ld
CFLAGS = -Wall -Wextra -std=c99 -O2 -fno-pie -fno-stack-protector -m32 -nostdlib -fno-builtin -fno-pic -mno-red-zone
ASMFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T kernel.ld

# Arquivos
BOOTLOADER = bootloader.bin
KERNEL = kernel.bin
OS_IMAGE = kernel-v.img

# Regra padrão
all: $(OS_IMAGE)

# Compilar o bootloader
$(BOOTLOADER): bootloader.asm
	$(ASM) -f bin -o $(BOOTLOADER) bootloader.asm

# Compilar o kernel
kernel.o: kernel_real.c
	$(CC) $(CFLAGS) -c kernel_real.c -o kernel.o

# Linkar o kernel
$(KERNEL): kernel.o
	$(LD) $(LDFLAGS) -o $(KERNEL) kernel.o

# Criar imagem do sistema operacional
$(OS_IMAGE): $(BOOTLOADER) $(KERNEL)
	dd if=/dev/zero of=$(OS_IMAGE) bs=512 count=2880
	dd if=$(BOOTLOADER) of=$(OS_IMAGE) conv=notrunc
	dd if=$(KERNEL) of=$(OS_IMAGE) conv=notrunc seek=1

# Limpar arquivos compilados
clean:
	rm -f *.o *.bin *.img kernel-v

# Executar no QEMU (emulador)
run: $(OS_IMAGE)
	qemu-system-i386 -fda $(OS_IMAGE)

# Executar no QEMU com console
run-console: $(OS_IMAGE)
	qemu-system-i386 -fda $(OS_IMAGE) -nographic

# Executar no QEMU com debug
run-debug: $(OS_IMAGE)
	qemu-system-i386 -fda $(OS_IMAGE) -s -S

# Mostrar ajuda
help:
	@echo "Comandos disponíveis:"
	@echo "  make        - Compilar o sistema operacional completo"
	@echo "  make run    - Compilar e executar no QEMU (GUI)"
	@echo "  make run-console - Executar no QEMU (console)"
	@echo "  make run-debug   - Executar no QEMU com debug"
	@echo "  make clean  - Limpar arquivos compilados"
	@echo "  make help   - Mostrar esta ajuda"
	@echo ""
	@echo "Requisitos:"
	@echo "  - NASM (assembler)"
	@echo "  - GCC com suporte a 32-bit"
	@echo "  - QEMU (emulador)"

.PHONY: all clean run run-console run-debug help
