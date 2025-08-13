#include <stdint.h>

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

// Buffer VGA
volatile uint16_t* vga_buffer = (volatile uint16_t*)0xB8000;
volatile int vga_x = 0;
volatile int vga_y = 0;
volatile uint8_t vga_color = VGA_WHITE | (VGA_BLACK << 4);

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

// Função principal do kernel
void kernel_main() {
    // Limpa a tela
    vga_clear();
    
    // Exibe banner
    vga_set_color(VGA_LIGHT_MAGENTA | (VGA_BLACK << 4));
    vga_puts("=== KERNEL SIMPLES - TESTE DE TECLADO ===\n\n");
    
    // Exibe instruções
    vga_set_color(VGA_LIGHT_CYAN | (VGA_BLACK << 4));
    vga_puts("Terminal funcionando! Digite normalmente...\n");
    vga_puts("Use Enter para nova linha, Backspace para apagar\n\n");
    
    // Mostra prompt inicial
    vga_set_color(VGA_LIGHT_YELLOW | (VGA_BLACK << 4));
    vga_puts("kernel> ");
    vga_set_color(vga_color);
    
    // Loop principal ultra-simples
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
                vga_putchar(ch);
            }
            else if (key >= 0x10 && key <= 0x1B) {
                char ch = "qwertyuiop[]"[key - 0x10];
                vga_putchar(ch);
            }
            else if (key >= 0x1E && key <= 0x28) {
                char ch = "asdfghjkl;'"[key - 0x1E];
                vga_putchar(ch);
            }
            else if (key >= 0x2C && key <= 0x35) {
                char ch = "zxcvbnm,./"[key - 0x2C];
                vga_putchar(ch);
            }
            else if (key == 0x39) {
                vga_putchar(' ');
            }
            else if (key == 0x1C) { // Enter
                vga_putchar('\n');
                vga_set_color(VGA_LIGHT_YELLOW | (VGA_BLACK << 4));
                vga_puts("kernel> ");
                vga_set_color(vga_color);
            }
            else if (key == 0x0E) { // Backspace
                if (vga_x > 0) {
                    vga_x--;
                    vga_putchar('\b');
                    vga_putchar(' ');
                    vga_putchar('\b');
                }
            }
            else if (key == 0x0F) { // Tab
                vga_putchar('\t');
            }
            else if (key == 0x1A) { // [
                vga_putchar('[');
            }
            else if (key == 0x1B) { // ]
                vga_putchar(']');
            }
            else if (key == 0x27) { // ;
                vga_putchar(';');
            }
            else if (key == 0x28) { // '
                vga_putchar('\'');
            }
            else if (key == 0x33) { // ,
                vga_putchar(',');
            }
            else if (key == 0x34) { // .
                vga_putchar('.');
            }
            else if (key == 0x35) { // /
                vga_putchar('/');
            }
            else if (key == 0x0B) { // 0
                vga_putchar('0');
            }
            else if (key == 0x0C) { // -
                vga_putchar('-');
            }
            else if (key == 0x0D) { // =
                vga_putchar('=');
            }
        }
        
        // Pausa mínima
        for (volatile int i = 0; i < 1; i++) {}
    }
}
