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

// Portas do teclado
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Tamanho máximo do buffer de comando
#define MAX_COMMAND_LENGTH 256

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
static char command_buffer[MAX_COMMAND_LENGTH];
static int command_pos = 0;
static int cursor_x = 0;
static int cursor_y = 0;

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
    cursor_x = 0;
    cursor_y = 0;
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
        cursor_x = 0;
        cursor_y = vga_y;
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
    cursor_x = vga_x;
    cursor_y = vga_y;
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

// Função para ler byte de uma porta
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// Função para escrever byte em uma porta
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Função para verificar se há tecla disponível (versão melhorada)
int keyboard_available() {
    // Verifica se há dados disponíveis na porta 0x64
    unsigned char status = inb(KEYBOARD_STATUS_PORT);
    
    // Bit 0 = Output buffer full (dados disponíveis)
    // Bit 1 = Input buffer full (buffer de entrada cheio)
    // Bit 2 = System flag
    // Bit 3 = Command/data flag
    // Bit 4 = Keyboard enabled
    // Bit 5 = Transmit timeout
    // Bit 6 = Receive timeout
    // Bit 7 = Parity error
    
    // Retorna true se há dados para ler (bit 0 = 1)
    return (status & 0x01) != 0;
}

// Função para ler tecla do teclado (versão melhorada)
char read_keyboard() {
    // Aguarda até que uma tecla esteja disponível
    while (!keyboard_available()) {
        // Pausa muito pequena para não travar
        for (volatile int i = 0; i < 100; i++) {}
    }
    
    // Lê o scancode da tecla
    char scancode = inb(KEYBOARD_DATA_PORT);
    
    // Pausa mínima para estabilização
    for (volatile int i = 0; i < 500; i++) {}
    
    return scancode;
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
size_t strlen(const char* str) {
    size_t len = 0;
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

// Função para processar comandos
void process_command(const char* command) {
    if (strcmp(command, "help") == 0) {
        vga_puts("Comandos disponíveis:\n");
        vga_puts("  help     - Mostra esta ajuda\n");
        vga_puts("  clear    - Limpa a tela\n");
        vga_puts("  info     - Mostra informações do sistema\n");
        vga_puts("  date     - Mostra data/hora (simulado)\n");
        vga_puts("  test     - Testa o teclado\n");
        vga_puts("  debug    - Modo debug do teclado\n");
        vga_puts("  qemu-test- Testa se QEMU captura input\n");
        vga_puts("  exit     - Reinicia o sistema\n");
        vga_puts("  reboot   - Reinicia o sistema\n");
    }
    else if (strcmp(command, "clear") == 0) {
        vga_clear();
        vga_set_color(VGA_LIGHT_YELLOW | (VGA_BLACK << 4));
        vga_puts("kernel-v> ");
    }
    else if (strcmp(command, "info") == 0) {
        vga_putchar('\n');
        SystemInfo sys_info;
        get_system_info(&sys_info);
        display_system_info(&sys_info);
        vga_set_color(VGA_LIGHT_YELLOW | (VGA_BLACK << 4));
        vga_puts("kernel-v> ");
    }
    else if (strcmp(command, "date") == 0) {
        vga_puts("Data: 13/08/2025 - Hora: 04:00:00 (simulado)\n");
        vga_set_color(VGA_LIGHT_YELLOW | (VGA_BLACK << 4));
        vga_puts("kernel-v> ");
    }
    else if (strcmp(command, "test") == 0) {
        vga_puts("Testando teclado... Digite algumas teclas:\n");
        vga_puts("Pressione qualquer tecla para testar (ESC para sair):\n");
        vga_puts("Scancodes serão mostrados entre colchetes [XX]\n");
        vga_set_color(VGA_LIGHT_YELLOW | (VGA_BLACK << 4));
        vga_puts("kernel-v> ");
    }
    else if (strcmp(command, "debug") == 0) {
        vga_puts("Modo debug ativado!\n");
        vga_puts("Aguardando teclas... (pressione ESC para sair)\n");
        vga_puts("Se não aparecer nada, o QEMU não está capturando o teclado!\n");
        
        // Loop de debug
        while (1) {
            if (keyboard_available()) {
                char debug_key = inb(KEYBOARD_DATA_PORT);
                vga_puts("[");
                vga_putint(debug_key);
                vga_puts("]");
                
                // ESC para sair
                if (debug_key == 0x01) {
                    vga_puts("\nSaindo do modo debug...\n");
                    break;
                }
            }
            
            // Pausa para não sobrecarregar
            for (volatile int i = 0; i < 10000; i++) {}
        }
        
        vga_set_color(VGA_LIGHT_YELLOW | (VGA_BLACK << 4));
        vga_puts("kernel-v> ");
    }
    else if (strcmp(command, "qemu-test") == 0) {
        vga_puts("Testando se o QEMU está capturando input...\n");
        vga_puts("Pressione qualquer tecla por 10 segundos...\n");
        
        int timeout = 0;
        while (timeout < 100) {
            if (keyboard_available()) {
                char test_key = inb(KEYBOARD_DATA_PORT);
                vga_puts("Tecla detectada: [");
                vga_putint(test_key);
                vga_puts("] - QEMU funcionando!\n");
                break;
            }
            
            // Contador de timeout
            if (timeout % 10 == 0) {
                vga_puts(".");
            }
            
            timeout++;
            for (volatile int i = 0; i < 100000; i++) {}
        }
        
        if (timeout >= 100) {
            vga_puts("\nNENHUMA tecla detectada! QEMU não está capturando input!\n");
            vga_puts("Tente usar: make run-console\n");
        }
        
        vga_set_color(VGA_LIGHT_YELLOW | (VGA_BLACK << 4));
        vga_puts("kernel-v> ");
    }
    else if (strcmp(command, "exit") == 0 || strcmp(command, "reboot") == 0) {
        vga_puts("Reiniciando sistema...\n");
        // Reinicia o sistema
        outb(0x64, 0xFE);
    }
    else if (strlen(command) > 0) {
        vga_puts("Comando não encontrado: ");
        vga_puts(command);
        vga_puts("\nDigite 'help' para ver comandos disponíveis.\n");
        vga_set_color(VGA_LIGHT_YELLOW | (VGA_BLACK << 4));
        vga_puts("kernel-v> ");
    }
    else {
        vga_set_color(VGA_LIGHT_YELLOW | (VGA_BLACK << 4));
        vga_puts("kernel-v> ");
    }
}



// Função para inicializar o sistema
void kernel_init() {
    // Limpa a tela
    vga_clear();
    
    // Define cor padrão
    vga_set_color(VGA_WHITE | (VGA_BLACK << 4));
    
    // Exibe banner de boas-vindas
    vga_set_color(VGA_LIGHT_MAGENTA | (VGA_BLACK << 4));
    vga_puts("=== Kernel-V - Sistema Operacional (GRUB) ===\n");
    vga_putchar('\n');
    
    // Obtém informações do sistema
    SystemInfo sys_info;
    get_system_info(&sys_info);
    
    // Exibe informações no estilo neofetch
    display_system_info(&sys_info);
    
    // Exibe mensagem de sucesso
    vga_set_color(VGA_LIGHT_GREEN | (VGA_BLACK << 4));
    vga_puts("Sistema operacional carregado com sucesso!\n");
    vga_puts("Digite 'help' para comandos disponíveis.\n");
    vga_putchar('\n');
    
    // Prompt do sistema
    vga_set_color(VGA_LIGHT_YELLOW | (VGA_BLACK << 4));
    vga_puts("kernel-v> ");
    
    // Indicador de que a inicialização terminou
    vga_set_color(VGA_LIGHT_RED | (VGA_BLACK << 4));
    vga_puts("INIT COMPLETE - SHELL STARTING...\n");
}

// Função para executar o shell
void run_shell() {
    char key;
    char ch;
    
    // Indicador de que o shell iniciou
    vga_set_color(VGA_LIGHT_RED | (VGA_BLACK << 4));
    vga_puts("SHELL STARTED - WAITING FOR INPUT...\n");
    vga_set_color(VGA_LIGHT_YELLOW | (VGA_BLACK << 4));
    vga_puts("kernel-v> ");
    
    // Loop principal do shell - sistema fica estável aqui
    while (1) {
        // Indicador visual de que o sistema está funcionando
        static int frame_counter = 0;
        frame_counter++;
        
        // Atualiza indicador a cada 50 frames (muito frequente)
        if (frame_counter % 50 == 0) {
            // Salva posição atual
            int old_x = vga_x;
            int old_y = vga_y;
            
            // Vai para canto superior direito
            vga_x = VGA_WIDTH - 15;
            vga_y = 0;
            
            // Mostra contador de frames
            vga_set_color(VGA_LIGHT_RED | (VGA_BLACK << 4));
            vga_puts("FRAME:");
            vga_putint(frame_counter / 50);
            
            // Restaura posição
            vga_x = old_x;
            vga_y = old_y;
            vga_set_color(vga_color);
        }
        
        // Verifica teclado de forma não-bloqueante
        if (keyboard_available()) {
            key = inb(KEYBOARD_DATA_PORT);
            
            // Debug: mostra todas as teclas
            vga_set_color(VGA_LIGHT_GREEN | (VGA_BLACK << 4));
            vga_puts("[");
            vga_putint(key);
            vga_puts("]");
            vga_set_color(vga_color);
            
            // Processa tecla especial
            if (key == 0xE0) {
                key = inb(KEYBOARD_DATA_PORT);
                continue;
            }
            
            // Converte scancode para caractere
            if (key >= 0x02 && key <= 0x0D) {
                ch = "1234567890-="[key - 0x02];
            }
            else if (key >= 0x10 && key <= 0x1B) {
                ch = "qwertyuiop[]"[key - 0x10];
            }
            else if (key >= 0x1E && key <= 0x28) {
                ch = "asdfghjkl;'"[key - 0x1E];
            }
            else if (key >= 0x2C && key <= 0x35) {
                ch = "zxcvbnm,./"[key - 0x2C];
            }
            else if (key == 0x39) {
                ch = ' ';
            }
            else if (key == 0x1C) { // Enter
                vga_putchar('\n');
                command_buffer[command_pos] = '\0';
                process_command(command_buffer);
                command_pos = 0;
                vga_set_color(VGA_LIGHT_YELLOW | (VGA_BLACK << 4));
                vga_puts("kernel-v> ");
                continue;
            }
            else if (key == 0x0E) { // Backspace
                if (command_pos > 0) {
                    command_pos--;
                    vga_putchar('\b');
                    vga_putchar(' ');
                    vga_putchar('\b');
                }
                continue;
            }
            else if (key == 0x0F) { // Tab
                ch = '\t';
            }
            else if (key == 0x1A) { // [
                ch = '[';
            }
            else if (key == 0x1B) { // ]
                ch = ']';
            }
            else if (key == 0x27) { // ;
                ch = ';';
            }
            else if (key == 0x28) { // '
                ch = '\'';
            }
            else if (key == 0x33) { // ,
                ch = ',';
            }
            else if (key == 0x34) { // .
                ch = '.';
            }
            else if (key == 0x35) { // /
                ch = '/';
            }
            else if (key == 0x0B) { // 0
                ch = '0';
            }
            else if (key == 0x0C) { // -
                ch = '-';
            }
            else if (key == 0x0D) { // =
                ch = '=';
            }
            else {
                // Tecla não reconhecida, mas não trava
                continue;
            }
            
            // Adiciona caractere ao buffer e exibe na tela
            if (command_pos < MAX_COMMAND_LENGTH - 1) {
                command_buffer[command_pos++] = ch;
                vga_putchar(ch);
            }
        }
        
        // Pausa mínima para não travar
        for (volatile int i = 0; i < 10; i++) {}
        
        // Teste adicional: mostra status do teclado a cada 100 frames
        if (frame_counter % 100 == 0) {
            // Salva posição atual
            int old_x = vga_x;
            int old_y = vga_y;
            
            // Vai para canto superior esquerdo
            vga_x = 0;
            vga_y = 1;
            
            // Mostra status do teclado
            vga_set_color(VGA_LIGHT_CYAN | (VGA_BLACK << 4));
            vga_puts("KB:");
            vga_putint(keyboard_available());
            
            // Restaura posição
            vga_x = old_x;
            vga_y = old_y;
            vga_set_color(vga_color);
        }
    }
}

// Função principal do kernel
void kernel_main() {
    kernel_init();
    
    // Inicializa o shell
    run_shell();
}
