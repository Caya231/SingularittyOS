/* Linker script para o SingularittyOS 64-bit */
ENTRY(kernel_main)

SECTIONS
{
    /* O kernel é carregado em 0x200000 (2MB) para 64-bit */
    . = 0x200000;
    
    /* Seção de código */
    .text : {
        *(.text)
        *(.text.*)
    }
    
    /* Seção de dados somente leitura */
    .rodata : {
        *(.rodata)
        *(.rodata.*)
    }
    
    /* Seção de dados */
    .data : {
        *(.data)
        *(.data.*)
    }
    
    /* Seção BSS (dados não inicializados) */
    .bss : {
        *(.bss)
        *(.bss.*)
        *(COMMON)
    }
    
    /* Alinhar o final para 4KB */
    . = ALIGN(4096);
}
