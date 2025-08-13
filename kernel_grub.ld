/* Linker script para o Kernel-V com GRUB */
ENTRY(kernel_main)

SECTIONS
{
    /* O kernel é carregado em 1MB pelo GRUB */
    . = 0x00100000;
    
    /* Seção multiboot deve estar no início */
    .multiboot : {
        *(.multiboot)
    }
    
    /* Seção de código */
    .text : {
        *(.text)
    }
    
    /* Seção de dados somente leitura */
    .rodata : {
        *(.rodata)
    }
    
    /* Seção de dados */
    .data : {
        *(.data)
    }
    
    /* Seção BSS (dados não inicializados) */
    .bss : {
        *(.bss)
    }
}
