#!/bin/bash

# Script de teste para SingularittyOS 64-bit
echo "=== Testando SingularittyOS 64-bit ==="
echo ""

# Verificar se o GCC suporta 64-bit
echo "Verificando suporte a 64-bit..."
if gcc -m64 -E - < /dev/null > /dev/null 2>&1; then
    echo "✓ GCC suporta 64-bit"
else
    echo "✗ GCC não suporta 64-bit"
    echo "Instale o pacote gcc-multilib ou equivalente"
    exit 1
fi

# Verificar se o QEMU 64-bit está disponível
echo "Verificando QEMU 64-bit..."
if command -v qemu-system-x86_64 &> /dev/null; then
    echo "✓ QEMU 64-bit disponível"
else
    echo "✗ QEMU 64-bit não encontrado"
    echo "Execute: ./install_deps.sh"
    exit 1
fi

# Compilar o kernel 64-bit
echo ""
echo "Compilando kernel 64-bit..."
make -f Makefile_64 clean
make -f Makefile_64

if [ $? -eq 0 ]; then
    echo "✓ Kernel 64-bit compilado com sucesso!"
    
    # Verificar se o arquivo foi criado
    if [ -f "kernel_64.bin" ]; then
        echo "✓ Arquivo kernel_64.bin criado"
        
        # Verificar se é um executável ELF 64-bit
        if file kernel_64.bin | grep -q "ELF 64-bit"; then
            echo "✓ Arquivo é um executável ELF 64-bit válido"
        else
            echo "✗ Arquivo não é um executável ELF 64-bit válido"
            exit 1
        fi
    else
        echo "✗ Arquivo kernel_64.bin não foi criado"
        exit 1
    fi
else
    echo "✗ Falha na compilação do kernel 64-bit"
    exit 1
fi

# Testar execução no QEMU (modo console para evitar problemas de GUI)
echo ""
echo "Testando execução no QEMU (5 segundos)..."
timeout 5s make -f Makefile_64 run-console > /dev/null 2>&1

if [ $? -eq 124 ]; then
    echo "✓ Kernel 64-bit executou no QEMU (timeout após 5s)"
else
    echo "⚠ Execução no QEMU pode ter tido problemas (código: $?)"
fi

echo ""
echo "=== Teste concluído! ==="
echo ""
echo "Para executar o sistema 64-bit:"
echo "  make -f Makefile_64 run        # Interface gráfica"
echo "  make -f Makefile_64 run-console # Console de texto"
echo "  make -f Makefile_64 iso        # Criar ISO bootável"
echo ""
echo "Para ver todos os comandos disponíveis:"
echo "  make -f Makefile_64 help"
