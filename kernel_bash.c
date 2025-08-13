#include <stdint.h>

// Definição de NULL
#define NULL ((void*)0)

// Cabeçalho Multiboot para compatibilidade com QEMU
#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
#define MULTIBOOT_HEADER_FLAGS 0x00000003
#define MULTIBOOT_HEADER_CHECKSUM -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

// Estrutura do cabeçalho Multiboot
struct multiboot_header {
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
} __attribute__((packed));

// Cabeçalho Multiboot (deve estar no início do arquivo)
__attribute__((section(".multiboot")))
struct multiboot_header multiboot_header = {
    MULTIBOOT_HEADER_MAGIC,
    MULTIBOOT_HEADER_FLAGS,
    MULTIBOOT_HEADER_CHECKSUM
};

// Definições do VGA
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_BLACK 0
#define VGA_BLUE 1
#define VGA_GREEN 2
#define VGA_CYAN 3
#define VGA_RED 4
#define VGA_MAGENTA 5
#define VGA_BROWN 6
#define VGA_LIGHT_GREY 7
#define VGA_DARK_GREY 8
#define VGA_LIGHT_BLUE 9
#define VGA_LIGHT_GREEN 10
#define VGA_LIGHT_CYAN 11
#define VGA_LIGHT_RED 12
#define VGA_LIGHT_MAGENTA 13
#define VGA_LIGHT_YELLOW 14
#define VGA_WHITE 15

// Portas do teclado
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Estrutura para comandos
#define MAX_COMMAND_LENGTH 256
#define MAX_ARGS 16
#define MAX_HISTORY 50

// Buffer VGA
volatile uint16_t* vga_buffer = (volatile uint16_t*)0xB8000;
volatile int vga_x = 0;
volatile int vga_y = 0;
volatile uint8_t vga_color = VGA_WHITE | (VGA_BLACK << 4);

// Variáveis globais
char command_buffer[MAX_COMMAND_LENGTH];
int command_pos = 0;
char command_history[MAX_HISTORY][MAX_COMMAND_LENGTH];
int history_pos = 0;
int current_history = 0;

// Funções VGA básicas
void vga_clear() {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = (uint16_t)' ' | (uint16_t)vga_color << 8;
    }
    vga_x = 0;
    vga_y = 0;
}

void vga_set_color(uint8_t color) {
    vga_color = color;
}

void vga_putchar(char c) {
    if (c == '\n') {
        vga_x = 0;
        vga_y++;
        if (vga_y >= VGA_HEIGHT) {
            vga_y = 0;
        }
        return;
    }
    
    if (vga_x >= VGA_WIDTH) {
        vga_x = 0;
        vga_y++;
        if (vga_y >= VGA_HEIGHT) {
            vga_y = 0;
        }
    }
    
    const int index = vga_y * VGA_WIDTH + vga_x;
    vga_buffer[index] = (uint16_t)c | (uint16_t)vga_color << 8;
    vga_x++;
}

void vga_puts(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        vga_putchar(str[i]);
    }
}

void vga_putint(int num) {
    if (num == 0) {
        vga_putchar('0');
        return;
    }
    
    if (num < 0) {
        vga_putchar('-');
        num = -num;
    }
    
    char buffer[20];
    int i = 0;
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    for (int j = i - 1; j >= 0; j--) {
        vga_putchar(buffer[j]);
    }
}

// Funções de I/O
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Função para verificar se há tecla disponível
int keyboard_available() {
    return (inb(KEYBOARD_STATUS_PORT) & 0x01) != 0;
}

// Função para ler tecla do teclado
char read_keyboard() {
    return inb(KEYBOARD_DATA_PORT);
}

// Função para comparar strings
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

// Função para obter tamanho da string
int strlen(const char* str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

// Função para copiar string
void strcpy(char* dest, const char* src) {
    while (*src) {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

// Função para dividir comando em argumentos
int parse_command(char* command, char* args[]) {
    int argc = 0;
    int i = 0;
    int start = 0;
    
    while (command[i] != '\0' && argc < MAX_ARGS - 1) {
        if (command[i] == ' ' || command[i] == '\t') {
            if (i > start) {
                command[i] = '\0';
                args[argc++] = (char*)&command[start];
                start = i + 1;
            }
        }
        i++;
    }
    
    if (i > start) {
        args[argc++] = (char*)&command[start];
    }
    
    args[argc] = NULL;
    return argc;
}

// Função para executar comandos
void execute_command(char* command) {
    char* args[MAX_ARGS];
    int argc = parse_command(command, args);
    
    if (argc == 0) return;
    
    // Adiciona ao histórico
    if (history_pos < MAX_HISTORY) {
        strcpy(command_history[history_pos++], command);
    }
    
    // Comandos básicos
    if (strcmp(args[0], "help") == 0) {
        vga_puts("Comandos disponíveis:\n");
        vga_puts("  help     - Mostra esta ajuda\n");
        vga_puts("  clear    - Limpa a tela\n");
        vga_puts("  ls       - Lista arquivos (simulado)\n");
        vga_puts("  pwd      - Mostra diretório atual\n");
        vga_puts("  echo     - Exibe texto\n");
        vga_puts("  date     - Mostra data/hora (simulado)\n");
        vga_puts("  whoami   - Mostra usuário atual\n");
        vga_puts("  uname    - Informações do sistema\n");
        vga_puts("  history  - Mostra histórico de comandos\n");
        vga_puts("  exit     - Sai do shell\n");
    }
    else if (strcmp(args[0], "clear") == 0) {
        vga_clear();
        vga_set_color(VGA_LIGHT_YELLOW | (VGA_BLACK << 4));
        vga_puts("kernel> ");
        vga_set_color(vga_color);
        vga_x = 8;
    }
    else if (strcmp(args[0], "ls") == 0) {
        vga_puts("Arquivos do sistema:\n");
        vga_puts("  kernel_bash.bin\n");
        vga_puts("  kernel_bash.c\n");
        vga_puts("  Makefile_bash\n");
        vga_puts("  README.md\n");
        vga_puts("  .git/\n");
    }
    else if (strcmp(args[0], "pwd") == 0) {
        vga_puts("/home/cayazita/kernel-v\n");
    }
    else if (strcmp(args[0], "echo") == 0) {
        for (int i = 1; i < argc; i++) {
            vga_puts(args[i]);
            if (i < argc - 1) vga_puts(" ");
        }
        vga_putchar('\n');
    }
    else if (strcmp(args[0], "date") == 0) {
        vga_puts("Wed Aug 13 16:15:00 BRT 2025\n");
    }
    else if (strcmp(args[0], "whoami") == 0) {
        vga_puts("cayazita\n");
    }
    else if (strcmp(args[0], "uname") == 0) {
        vga_puts("Kernel-V 1.0.0\n");
    }
    else if (strcmp(args[0], "history") == 0) {
        vga_puts("Histórico de comandos:\n");
        for (int i = 0; i < history_pos; i++) {
            vga_putint(i + 1);
            vga_puts("  ");
            vga_puts(command_history[i]);
            vga_putchar('\n');
        }
    }
    else if (strcmp(args[0], "exit") == 0) {
        vga_puts("Saindo do shell...\n");
        // Aqui você pode implementar saída real
    }
    else {
        vga_puts("Comando não encontrado: ");
        vga_puts(args[0]);
        vga_puts("\nDigite 'help' para ver comandos disponíveis\n");
    }
}

// Função principal do kernel
void kernel_main() {
    // Limpa a tela
    vga_clear();
    
    // Exibe banner
    vga_set_color(VGA_LIGHT_MAGENTA | (VGA_BLACK << 4));
    vga_puts("=== KERNEL-V BASH - TERMINAL FUNCIONAL ===\n\n");
    
    // Exibe instruções
    vga_set_color(VGA_LIGHT_CYAN | (VGA_BLACK << 4));
    vga_puts("Terminal BASH funcionando! Digite comandos...\n");
    vga_puts("Digite 'help' para ver comandos disponíveis\n\n");
    
    // Mostra prompt inicial
    vga_set_color(VGA_LIGHT_YELLOW | (VGA_BLACK << 4));
    vga_puts("kernel> ");
    vga_set_color(vga_color);
    // Cursor fica na frente do prompt
    vga_x = 8; // "kernel> " tem 8 caracteres
    
    // Loop principal
    int frame_counter = 0;
    
    while (1) {
        frame_counter++;
        
        // Atualiza contador a cada 10 frames
        if (frame_counter % 10 == 0) {
            // Salva posição
            int old_x = vga_x;
            int old_y = vga_y;
            
            // Vai para canto superior direito
            vga_x = VGA_WIDTH - 15;
            vga_y = 0;
            
            // Mostra contador
            vga_set_color(VGA_LIGHT_RED | (VGA_BLACK << 4));
            vga_puts("FRAME:");
            vga_putint(frame_counter / 10);
            
            // Restaura posição
            vga_x = old_x;
            vga_y = old_y;
            vga_set_color(vga_color);
        }
        
        // Verifica teclado
        if (keyboard_available()) {
            char key = read_keyboard();
            
            // Converte para caractere se possível
            if (key >= 0x02 && key <= 0x0D) {
                char ch = "1234567890-="[key - 0x02];
                if (command_pos < MAX_COMMAND_LENGTH - 1) {
                    command_buffer[command_pos++] = ch;
                    vga_putchar(ch);
                }
            }
            else if (key >= 0x10 && key <= 0x1B) {
                char ch = "qwertyuiop[]"[key - 0x10];
                if (command_pos < MAX_COMMAND_LENGTH - 1) {
                    command_buffer[command_pos++] = ch;
                    vga_putchar(ch);
                }
            }
            else if (key >= 0x1E && key <= 0x28) {
                char ch = "asdfghjkl;'"[key - 0x1E];
                if (command_pos < MAX_COMMAND_LENGTH - 1) {
                    command_buffer[command_pos++] = ch;
                    vga_putchar(ch);
                }
            }
            else if (key >= 0x2C && key <= 0x35) {
                char ch = "zxcvbnm,./"[key - 0x2C];
                if (command_pos < MAX_COMMAND_LENGTH - 1) {
                    command_buffer[command_pos++] = ch;
                    vga_putchar(ch);
                }
            }
            else if (key == 0x39) {
                if (command_pos < MAX_COMMAND_LENGTH - 1) {
                    command_buffer[command_pos++] = ' ';
                    vga_putchar(' ');
                }
            }
            else if (key == 0x1C) { // Enter
                vga_putchar('\n');
                // Executa o comando
                command_buffer[command_pos] = '\0';
                execute_command(command_buffer);
                // Reseta buffer e posição
                command_pos = 0;
                // Mostra novo prompt
                vga_set_color(VGA_LIGHT_YELLOW | (VGA_BLACK << 4));
                vga_puts("kernel> ");
                vga_set_color(vga_color);
                // Cursor fica na frente do prompt
                vga_x = 8; // "kernel> " tem 8 caracteres
            }
            else if (key == 0x0E) { // Backspace
                if (command_pos > 0) {
                    command_pos--;
                    if (vga_x > 0) {
                        vga_x--;
                        vga_putchar('\b');
                        vga_putchar(' ');
                        vga_putchar('\b');
                    }
                }
            }
            else if (key == 0x0F) { // Tab
                if (command_pos < MAX_COMMAND_LENGTH - 1) {
                    command_buffer[command_pos++] = '\t';
                    vga_putchar('\t');
                }
            }
            else if (key == 0x1A) { // [
                if (command_pos < MAX_COMMAND_LENGTH - 1) {
                    command_buffer[command_pos++] = '[';
                    vga_putchar('[');
                }
            }
            else if (key == 0x1B) { // ]
                if (command_pos < MAX_COMMAND_LENGTH - 1) {
                    command_buffer[command_pos++] = ']';
                    vga_putchar(']');
                }
            }
            else if (key == 0x27) { // ;
                if (command_pos < MAX_COMMAND_LENGTH - 1) {
                    command_buffer[command_pos++] = ';';
                    vga_putchar(';');
                }
            }
            else if (key == 0x28) { // '
                if (command_pos < MAX_COMMAND_LENGTH - 1) {
                    command_buffer[command_pos++] = '\'';
                    vga_putchar('\'');
                }
            }
            else if (key == 0x33) { // ,
                if (command_pos < MAX_COMMAND_LENGTH - 1) {
                    command_buffer[command_pos++] = ',';
                    vga_putchar(',');
                }
            }
            else if (key == 0x34) { // .
                if (command_pos < MAX_COMMAND_LENGTH - 1) {
                    command_buffer[command_pos++] = '.';
                    vga_putchar('.');
                }
            }
            else if (key == 0x0B) { // 0
                if (command_pos < MAX_COMMAND_LENGTH - 1) {
                    command_buffer[command_pos++] = '0';
                    vga_putchar('0');
                }
            }
            else if (key == 0x0C) { // -
                if (command_pos < MAX_COMMAND_LENGTH - 1) {
                    command_buffer[command_pos++] = '-';
                    vga_putchar('-');
                }
            }
            else if (key == 0x0D) { // =
                if (command_pos < MAX_COMMAND_LENGTH - 1) {
                    command_buffer[command_pos++] = '=';
                    vga_putchar('=');
                }
            }
        }
        
        // Pausa mínima
        for (volatile int i = 0; i < 1; i++) {}
    }
}
