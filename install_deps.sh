#!/bin/bash

# Script para instalar dependências do Kernel-V
# Sistema Operacional

echo "=== Instalando dependências para o Kernel-V ==="
echo ""

# Verificar se é Arch Linux
if command -v pacman &> /dev/null; then
    echo "Detectado Arch Linux. Instalando dependências..."
    sudo pacman -S --needed base-devel nasm qemu
    echo "Dependências instaladas com sucesso!"
    
elif command -v apt &> /dev/null; then
    echo "Detectado Debian/Ubuntu. Instalando dependências..."
    sudo apt update
    sudo apt install -y build-essential nasm qemu-system-x86
    echo "Dependências instaladas com sucesso!"
    
elif command -v dnf &> /dev/null; then
    echo "Detectado Fedora/RHEL. Instalando dependências..."
    sudo dnf install -y gcc nasm qemu
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

# Verificar QEMU
if command -v qemu-system-i386 &> /dev/null; then
    echo "✓ QEMU instalado: $(qemu-system-i386 --version | head -n1)"
else
    echo "✗ QEMU não encontrado"
fi

echo ""
echo "=== Dependências instaladas! ==="
echo "Agora você pode compilar o sistema operacional com:"
echo "  make"
echo ""
echo "E executá-lo com:"
echo "  make run"
