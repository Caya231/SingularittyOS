/* Linker script para o Kernel-V */
ENTRY(kernel_main)

SECTIONS
{
    /* O kernel é carregado em 0x1000 */
    . = 0x1000;
    
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
