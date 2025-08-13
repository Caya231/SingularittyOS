#include <stdint.h>
#include <stddef.h>

// Definições do Multiboot
#define MULTIBOOT_HEADER_MAGIC 0x1BADB002
#define MULTIBOOT_HEADER_FLAGS 0x00000003
#define MULTIBOOT_HEADER_CHECKSUM -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

// Definições de cores para VGA
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
#define VGA_LIGHT_BROWN 14
#define VGA_LIGHT_YELLOW 14
#define VGA_WHITE 15

// Endereço base da memória VGA
#define VGA_BASE 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// Estrutura para informações do sistema
typedef struct {
    char hostname[32];
    char kernel_version[32];
    char cpu_info[64];
    uint32_t memory_mb;
    uint32_t uptime_seconds;
} SystemInfo;

// Variáveis globais
static uint16_t* vga_buffer = (uint16_t*)VGA_BASE;
static uint8_t vga_color = VGA_LIGHT_GREY | (VGA_BLACK << 4);
static size_t vga_x = 0;
static size_t vga_y = 0;

// Header do Multiboot (deve estar no início do arquivo)
__attribute__((section(".multiboot")))
__attribute__((aligned(4)))
const struct {
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
} multiboot_header = {
    MULTIBOOT_HEADER_MAGIC,
    MULTIBOOT_HEADER_FLAGS,
    MULTIBOOT_HEADER_CHECKSUM
};

// Função para limpar a tela
void vga_clear() {
    for (size_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        vga_buffer[i] = (uint16_t)' ' | (uint16_t)vga_color << 8;
    }
    vga_x = 0;
    vga_y = 0;
}

// Função para definir cor
void vga_set_color(uint8_t color) {
    vga_color = color;
}

// Função para colocar caractere na tela
void vga_putchar(char c) {
    if (c == '\n') {
        vga_x = 0;
        vga_y++;
        if (vga_y >= VGA_HEIGHT) {
            // Scroll da tela
            for (size_t i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++) {
                vga_buffer[i] = vga_buffer[i + VGA_WIDTH];
            }
            for (size_t i = VGA_WIDTH * (VGA_HEIGHT - 1); i < VGA_WIDTH * VGA_HEIGHT; i++) {
                vga_buffer[i] = (uint16_t)' ' | (uint16_t)vga_color << 8;
            }
            vga_y = VGA_HEIGHT - 1;
        }
        return;
    }
    
    if (vga_x >= VGA_WIDTH) {
        vga_x = 0;
        vga_y++;
    }
    
    if (vga_y >= VGA_HEIGHT) {
        vga_y = 0;
    }
    
    const size_t index = vga_y * VGA_WIDTH + vga_x;
    vga_buffer[index] = (uint16_t)c | (uint16_t)vga_color << 8;
    vga_x++;
}

// Função para exibir string
void vga_puts(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        vga_putchar(str[i]);
    }
}

// Função para exibir número
void vga_putint(uint32_t num) {
    if (num == 0) {
        vga_putchar('0');
        return;
    }
    
    char buffer[32];
    int i = 0;
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (--i >= 0) {
        vga_putchar(buffer[i]);
    }
}

// Função para obter informações básicas do sistema
void get_system_info(SystemInfo* info) {
    // Informações hardcoded para simplicidade
    const char* hostname = "Kernel-V";
    const char* kernel_ver = "1.0.0";
    const char* cpu = "x86_64";
    
    size_t i;
    for (i = 0; hostname[i] != '\0' && i < 31; i++) {
        info->hostname[i] = hostname[i];
    }
    info->hostname[i] = '\0';
    
    for (i = 0; kernel_ver[i] != '\0' && i < 31; i++) {
        info->kernel_version[i] = kernel_ver[i];
    }
    info->kernel_version[i] = '\0';
    
    for (i = 0; cpu[i] != '\0' && i < 63; i++) {
        info->cpu_info[i] = cpu[i];
    }
    info->cpu_info[i] = '\0';
    
    info->memory_mb = 512; // Simulado
    info->uptime_seconds = 0;
}

// Função para exibir informações do sistema no estilo neofetch
void display_system_info(SystemInfo* info) {
    vga_set_color(VGA_LIGHT_CYAN | (VGA_BLACK << 4));
    vga_puts("                    ");
    vga_puts(info->hostname);
    vga_puts("@kernel-v\n");
    
    vga_set_color(VGA_LIGHT_GREY | (VGA_BLACK << 4));
    vga_puts("                   ---------------\n");
    
    vga_set_color(VGA_LIGHT_GREEN | (VGA_BLACK << 4));
    vga_puts("OS:                 Kernel-V ");
    vga_puts(info->cpu_info);
    vga_putchar('\n');
    
    vga_set_color(VGA_LIGHT_BLUE | (VGA_BLACK << 4));
    vga_puts("Kernel:             ");
    vga_puts(info->kernel_version);
    vga_putchar('\n');
    
    vga_set_color(VGA_LIGHT_MAGENTA | (VGA_BLACK << 4));
    vga_puts("Uptime:             ");
    vga_putint(info->uptime_seconds);
    vga_puts("s\n");
    
    vga_set_color(VGA_LIGHT_RED | (VGA_BLACK << 4));
    vga_puts("Memory:             ");
    vga_putint(info->memory_mb);
    vga_puts("MB\n");
    
    vga_set_color(VGA_LIGHT_BROWN | (VGA_BLACK << 4));
    vga_puts("Shell:              kernel-shell\n");
    
    vga_putchar('\n');
}

// Função para inicializar o sistema
void kernel_init() {
    // Limpa a tela
    vga_clear();
    
    // Define cor padrão
    vga_set_color(VGA_WHITE | (VGA_BLACK << 4));
    
    // Exibe banner de boas-vindas
    vga_set_color(VGA_LIGHT_CYAN | (VGA_BLACK << 4));
    vga_puts("=== Kernel-V - Sistema Operacional (GRUB) ===\n");
    vga_putchar('\n');
    
    // Obtém informações do sistema
    SystemInfo sys_info;
    get_system_info(&sys_info);
    
    // Exibe informações no estilo neofetch
    display_system_info(&sys_info);
    
    // Exibe mensagem de sucesso
    vga_set_color(VGA_LIGHT_GREEN | (VGA_BLACK << 4));
    vga_puts("Sistema operacional carregado com sucesso pelo GRUB!\n");
    vga_puts("Digite 'help' para comandos disponíveis.\n");
    vga_putchar('\n');
    
    // Prompt do sistema
    vga_set_color(VGA_LIGHT_YELLOW | (VGA_BLACK << 4));
    vga_puts("kernel-v> ");
}

// Função principal do kernel
void kernel_main() {
    kernel_init();
    
    // Sistema fica "travado" aqui mostrando as informações
    // Loop infinito que mantém o kernel rodando sem reiniciar
    
    // Desabilita interrupções para evitar reinicializações
    __asm__ volatile("cli");
    
    while (1) {
        // Loop infinito simples
        // O sistema fica parado na tela com as informações
        // Não faz nada, apenas mantém o kernel ativo
        
        // Pequena pausa para não sobrecarregar a CPU
        for (volatile int i = 0; i < 1000000; i++) {
            // Loop vazio para pausa
        }
    }
}
