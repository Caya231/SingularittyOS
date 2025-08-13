#!/bin/bash

# Script para instalar dependências do SingularittyOS
# Sistema Operacional (32-bit e 64-bit)

echo "=== Instalando dependências para o SingularittyOS ==="
echo ""

# Verificar se é Arch Linux
if command -v pacman &> /dev/null; then
    echo "Detectado Arch Linux. Instalando dependências..."
    sudo pacman -S --needed base-devel nasm qemu qemu-arch-extra
    echo "Dependências instaladas com sucesso!"
    
elif command -v apt &> /dev/null; then
    echo "Detectado Debian/Ubuntu. Instalando dependências..."
    sudo apt update
    sudo apt install -y build-essential nasm qemu-system-x86 qemu-system-x86_64
    echo "Dependências instaladas com sucesso!"
    
elif command -v dnf &> /dev/null; then
    echo "Detectado Fedora/RHEL. Instalando dependências..."
    sudo dnf install -y gcc nasm qemu qemu-system-x86_64
    echo "Dependências instaladas com sucesso!"
    
else
    echo "Distribuição não reconhecida. Por favor, instale manualmente:"
    echo "  - GCC (compilador C)"
    echo "  - NASM (assembler)"
    echo "  - QEMU (emulador)"
    echo ""
    echo "Ou execute: make help para ver os requisitos"
    exit 1
fi

echo ""
echo "=== Verificando instalação ==="

# Verificar GCC
if command -v gcc &> /dev/null; then
    echo "✓ GCC instalado: $(gcc --version | head -n1)"
else
    echo "✗ GCC não encontrado"
fi

# Verificar NASM
if command -v nasm &> /dev/null; then
    echo "✓ NASM instalado: $(nasm --version | head -n1)"
else
    echo "✗ NASM não encontrado"
fi

# Verificar QEMU 32-bit
if command -v qemu-system-i386 &> /dev/null; then
    echo "✓ QEMU 32-bit instalado: $(qemu-system-i386 --version | head -n1)"
else
    echo "✗ QEMU 32-bit não encontrado"
fi

# Verificar QEMU 64-bit
if command -v qemu-system-x86_64 &> /dev/null; then
    echo "✓ QEMU 64-bit instalado: $(qemu-system-x86_64 --version | head -n1)"
else
    echo "✗ QEMU 64-bit não encontrado"
fi

echo ""
echo "=== Dependências instaladas! ==="
echo "Agora você pode compilar o sistema operacional com:"
echo ""
echo "Para 32-bit:"
echo "  make                    # Kernel original"
echo "  make -f Makefile_grub  # Kernel GRUB"
echo ""
echo "Para 64-bit:"
echo "  make -f Makefile_64    # Kernel 64-bit"
echo ""
echo "E executá-los com:"
echo "  make run               # 32-bit original"
echo "  make -f Makefile_64 run # 64-bit"
