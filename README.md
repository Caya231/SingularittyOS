# SingularittyOS

Um sistema operacional completo em C que exibe informações do sistema no estilo neofetch, com suporte ao GRUB como bootloader e **suporte completo para 64-bit**.

## Características

- **Bootloader**: GRUB (padrão da indústria)
- **Kernel Real**: Kernel em C que roda em modo protegido
- **Multiboot**: Compatível com padrão Multiboot
- **Sistema de Vídeo**: Driver VGA básico com cores
- **Interface**: Exibição de informações no estilo neofetch
- **Arquitetura**: x86 32-bit e **x86_64 64-bit**
- **Emulação**: Suporte completo ao QEMU (32-bit e 64-bit)

## Instalação de Dependências

Primeiro, instale as dependências necessárias:

```bash
./install_deps.sh
```

**Para o kernel GRUB (recomendado):**
```bash
sudo pacman -S grub libisoburn
```

## Compilação

### Kernel Original (Bootloader Assembly):
```bash
make
```

### Kernel GRUB (Recomendado):
```bash
make -f Makefile_grub
```

### Kernel 64-bit (Novo!):
```bash
make -f Makefile_64
```

## Execução

### Kernel Original:
```bash
make run          # Interface gráfica
make run-console  # Console de texto
make run-debug    # Modo debug
```

### Kernel GRUB:
```bash
make -f Makefile_grub run    # Kernel direto
make -f Makefile_grub iso    # Criar ISO bootável
make -f Makefile_grub run-iso # Executar ISO
```

### Kernel 64-bit:
```bash
make -f Makefile_64 run      # Kernel direto
make -f Makefile_64 iso      # Criar ISO bootável
make -f Makefile_64 run-iso  # Executar ISO
```

## Comandos Make

### Kernel Original:
- `make` - Compila o sistema operacional completo
- `make run` - Compila e executa no QEMU (GUI)
- `make run-console` - Executa no QEMU (console)
- `make run-debug` - Executa no QEMU com debug
- `make clean` - Remove arquivos compilados
- `make help` - Mostra ajuda dos comandos

### Kernel GRUB:
- `make -f Makefile_grub` - Compila o kernel GRUB
- `make -f Makefile_grub run` - Executa kernel diretamente
- `make -f Makefile_grub iso` - Cria ISO bootável
- `make -f Makefile_grub run-iso` - Executa ISO no QEMU
- `make -f Makefile_grub clean` - Remove arquivos compilados
- `make -f Makefile_grub help` - Mostra ajuda dos comandos

### Kernel 64-bit:
- `make -f Makefile_64` - Compila o kernel 64-bit
- `make -f Makefile_64 run` - Executa kernel diretamente
- `make -f Makefile_64 iso` - Cria ISO bootável
- `make -f Makefile_64 run-iso` - Executa ISO no QEMU
- `make -f Makefile_64 clean` - Remove arquivos compilados
- `make -f Makefile_64 help` - Mostra ajuda dos comandos

## Requisitos

- **GCC**: Compilador C com suporte a 32-bit
- **NASM**: Assembler para o bootloader
- **QEMU**: Emulador para executar o sistema operacional
- **Sistema**: Linux (qualquer distribuição)

## Estrutura do Código

### Kernel Original:
- `bootloader.asm` - Bootloader em Assembly
- `kernel_real.c` - Kernel principal em C
- `kernel.ld` - Script de linker
- `Makefile` - Script de compilação

### Kernel GRUB (Recomendado):
- `kernel_grub.c` - Kernel principal compatível com GRUB
- `kernel_grub.ld` - Script de linker para GRUB
- `Makefile_grub` - Script de compilação para GRUB
- `grub.cfg` - Configuração do GRUB

### Arquivos Gerais:
- `install_deps.sh` - Instalador de dependências
- `test_grub.sh` - Script de teste do kernel GRUB
- `README.md` - Este arquivo de documentação

## Como Funciona

### Kernel Original:
O sistema operacional funciona em camadas:

1. **Bootloader** - Carregador em Assembly que carrega o kernel
2. **Kernel** - Inicializa o sistema e drivers básicos
3. **Driver VGA** - Gerencia a saída de vídeo
4. **Interface** - Exibe informações do sistema
5. **Sistema** - Loop principal do kernel

### Kernel GRUB (Recomendado):
1. **GRUB** - Bootloader padrão da indústria
2. **Multiboot** - Carrega o kernel na memória
3. **Kernel** - Inicializa o sistema e drivers básicos
4. **Driver VGA** - Gerencia a saída de vídeo
5. **Interface** - Exibe informações do sistema
6. **Sistema** - Loop principal do kernel

### Kernel 64-bit:
1. **GRUB** - Bootloader padrão da indústria
2. **Multiboot** - Carrega o kernel na memória
3. **Kernel 64-bit** - Inicializa o sistema em modo 64-bit
4. **Driver VGA** - Gerencia a saída de vídeo (endereços 64-bit)
5. **Interface** - Exibe informações do sistema com arquitetura x86_64
6. **Sistema** - Loop principal do kernel com suporte a 64-bit

## Exemplo de Saída

```
=== Kernel-V - Sistema Operacional ===

                    Kernel-V@kernel-v
                   ---------------
OS:                 SingularittyOS x86_64
Kernel:             2.0.0
Architecture:       x86_64
Uptime:             0s
Memory:             2048MB
Shell:              singularity-shell

Sistema operacional carregado com sucesso!
Digite 'help' para comandos disponíveis.

singularitty>
```

## Licença

Este projeto é de código aberto e pode ser usado livremente.
# SingularittyOS
