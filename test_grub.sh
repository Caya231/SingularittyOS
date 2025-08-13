#!/bin/bash

# Script para testar o Kernel-V com GRUB

echo "=== Testando Kernel-V com GRUB ==="
echo ""

# Compilar o kernel
echo "1. Compilando o kernel..."
make -f Makefile_grub clean
make -f Makefile_grub

if [ $? -eq 0 ]; then
    echo "✓ Kernel compilado com sucesso!"
else
    echo "✗ Erro na compilação"
    exit 1
fi

echo ""
echo "2. Testando o kernel no QEMU..."
echo "   Pressione Ctrl+C para sair"
echo ""

# Executar no QEMU
make -f Makefile_grub run

echo ""
echo "=== Teste concluído ==="
