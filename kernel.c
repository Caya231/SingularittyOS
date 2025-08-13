#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/types.h>
#include <netdb.h>

// Estrutura para armazenar informações do sistema
typedef struct {
    char hostname[256];
    char kernel[256];
    char os[256];
    char arch[256];
    char uptime[256];
    char shell[256];
    char cpu[256];
    char memory[256];
    char user[256];
} SystemInfo;

// Função para obter o hostname
void get_hostname(SystemInfo *info) {
    FILE *file = fopen("/proc/sys/kernel/hostname", "r");
    if (file) {
        if (fgets(info->hostname, sizeof(info->hostname), file)) {
            // Remove quebra de linha
            char *newline = strchr(info->hostname, '\n');
            if (newline) *newline = '\0';
            
            // Remove o domínio se existir
            char *dot = strchr(info->hostname, '.');
            if (dot) *dot = '\0';
        } else {
            strcpy(info->hostname, "unknown");
        }
        fclose(file);
    } else {
        strcpy(info->hostname, "unknown");
    }
}

// Função para obter informações do kernel
void get_kernel_info(SystemInfo *info) {
    struct utsname uts;
    if (uname(&uts) == 0) {
        strcpy(info->kernel, uts.release);
        strcpy(info->os, uts.sysname);
        strcpy(info->arch, uts.machine);
    } else {
        strcpy(info->kernel, "unknown");
        strcpy(info->os, "unknown");
        strcpy(info->arch, "unknown");
    }
}

// Função para obter uptime
void get_uptime(SystemInfo *info) {
    struct sysinfo si;
    if (sysinfo(&si) == 0) {
        unsigned long uptime_sec = si.uptime;
        unsigned long days = uptime_sec / 86400;
        unsigned long hours = (uptime_sec % 86400) / 3600;
        unsigned long minutes = (uptime_sec % 3600) / 60;
        
        if (days > 0) {
            sprintf(info->uptime, "%lud %luh %lum", days, hours, minutes);
        } else if (hours > 0) {
            sprintf(info->uptime, "%luh %lum", hours, minutes);
        } else {
            sprintf(info->uptime, "%lum", minutes);
        }
    } else {
        strcpy(info->uptime, "unknown");
    }
}

// Função para obter shell atual
void get_shell(SystemInfo *info) {
    char *shell_path = getenv("SHELL");
    if (shell_path) {
        char *shell_name = strrchr(shell_path, '/');
        if (shell_name) {
            strcpy(info->shell, shell_name + 1);
        } else {
            strcpy(info->shell, shell_path);
        }
    } else {
        strcpy(info->shell, "unknown");
    }
}

// Função para obter informações da CPU
void get_cpu_info(SystemInfo *info) {
    FILE *file = fopen("/proc/cpuinfo", "r");
    if (file) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            if (strncmp(line, "model name", 10) == 0) {
                char *colon = strchr(line, ':');
                if (colon) {
                    colon++; // Pula o ':'
                    while (*colon == ' ' || *colon == '\t') colon++; // Remove espaços
                    strcpy(info->cpu, colon);
                    // Remove quebra de linha
                    char *newline = strchr(info->cpu, '\n');
                    if (newline) *newline = '\0';
                    break;
                }
            }
        }
        fclose(file);
    } else {
        strcpy(info->cpu, "unknown");
    }
}

// Função para obter informações de memória
void get_memory_info(SystemInfo *info) {
    struct sysinfo si;
    if (sysinfo(&si) == 0) {
        unsigned long total_mem = si.totalram / 1024 / 1024; // MB
        unsigned long free_mem = si.freeram / 1024 / 1024;   // MB
        unsigned long used_mem = total_mem - free_mem;
        
        sprintf(info->memory, "%luM / %luM", used_mem, total_mem);
    } else {
        strcpy(info->memory, "unknown");
    }
}

// Função para obter usuário atual
void get_user_info(SystemInfo *info) {
    uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    if (pw) {
        strcpy(info->user, pw->pw_name);
    } else {
        strcpy(info->user, "unknown");
    }
}

// Função para coletar todas as informações
void collect_system_info(SystemInfo *info) {
    get_hostname(info);
    get_kernel_info(info);
    get_uptime(info);
    get_shell(info);
    get_cpu_info(info);
    get_memory_info(info);
    get_user_info(info);
}

// Função para exibir as informações no estilo neofetch
void display_neofetch_style(SystemInfo *info) {
    printf("\n");
    printf("                    %s@%s\n", info->user, info->hostname);
    printf("                   ---------------\n");
    printf("OS:                 %s %s\n", info->os, info->arch);
    printf("Kernel:             %s\n", info->kernel);
    printf("Uptime:             %s\n", info->uptime);
    printf("Shell:              %s\n", info->shell);
    printf("CPU:                %s\n", info->cpu);
    printf("Memory:             %s\n", info->memory);
    printf("\n");
}

// Função principal do kernel
int kernel_main() {
    SystemInfo sys_info;
    
    printf("=== Kernel-V - Sistema de Informações ===\n");
    
    // Coleta informações do sistema
    collect_system_info(&sys_info);
    
    // Exibe no estilo neofetch
    display_neofetch_style(&sys_info);
    
    printf("Kernel carregado com sucesso!\n");
    return 0;
}

// Função main padrão
int main() {
    return kernel_main();
}
