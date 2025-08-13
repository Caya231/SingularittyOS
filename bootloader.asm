; Bootloader para o Kernel-V
; Este é um bootloader simples que carrega o kernel na memória

[org 0x7c00]        ; BIOS carrega o bootloader em 0x7c00
[bits 16]           ; Modo 16-bit

; Configuração inicial
    mov ax, 0       ; Limpa registradores
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7c00  ; Stack pointer

; Limpa a tela
    mov ah, 0x00    ; Função BIOS: limpar tela
    mov al, 0x03    ; Modo texto 80x25
    int 0x10

; Exibe mensagem de boot
    mov si, boot_msg
    call print_string

; Carrega o kernel do disco
    mov ah, 0x02    ; Função BIOS: ler setores
    mov al, 15      ; Número de setores para ler
    mov ch, 0       ; Cilindro 0
    mov cl, 2       ; Setor 2 (setor 1 é o bootloader)
    mov dh, 0       ; Cabeça 0
    mov dl, 0x80    ; Drive 0 (primeiro disco)
    mov bx, 0x1000  ; Endereço de destino na memória
    int 0x13        ; Interrupção BIOS para disco

    jc disk_error   ; Se houve erro, pula para tratamento

; Pula para o kernel
    mov si, kernel_loaded_msg
    call print_string
    
    jmp 0x1000      ; Pula para o kernel

; Função para exibir string
print_string:
    lodsb           ; Carrega byte de [si] em al
    or al, al       ; Verifica se al é zero (fim da string)
    jz print_done   ; Se zero, termina
    mov ah, 0x0e    ; Função BIOS: exibir caractere
    int 0x10        ; Interrupção BIOS para vídeo
    jmp print_string
print_done:
    ret

; Tratamento de erro de disco
disk_error:
    mov si, disk_error_msg
    call print_string
    jmp $           ; Loop infinito

; Mensagens
boot_msg:        db 'Kernel-V Bootloader v1.0', 13, 10, 'Carregando sistema...', 13, 10, 0
kernel_loaded_msg: db 'Kernel carregado com sucesso!', 13, 10, 'Iniciando sistema...', 13, 10, 0
disk_error_msg:  db 'Erro ao carregar kernel!', 13, 10, 0

; Padding para completar 512 bytes
    times 510-($-$$) db 0
    dw 0xaa55       ; Assinatura de boot
