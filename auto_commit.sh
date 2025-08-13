#!/bin/bash

# Script para commits automáticos no Git
# Executa sempre que houver mudanças nos arquivos

echo "=== Auto-Commit Git para Kernel-V ==="
echo ""

# Verifica se há mudanças
if [[ -n $(git status --porcelain) ]]; then
    echo "📝 Mudanças detectadas! Fazendo commit..."
    
    # Adiciona todos os arquivos
    git add .
    
    # Faz commit com timestamp
    TIMESTAMP=$(date "+%Y-%m-%d %H:%M:%S")
    git commit -m "Auto-commit: $TIMESTAMP - Mudanças no Kernel-V"
    
    echo "✅ Commit realizado com sucesso!"
    echo "📅 Timestamp: $TIMESTAMP"
    
    # Mostra status
    echo ""
    echo "📊 Status atual:"
    git status --short
else
    echo "✅ Nenhuma mudança detectada"
fi

echo ""
echo "=== Auto-Commit concluído ==="
