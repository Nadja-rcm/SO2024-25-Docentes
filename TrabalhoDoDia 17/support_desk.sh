#!/bin/bash

# Verificar se os argumentos foram passados
if [ "$#" -ne 4 ]; then
    echo "Uso: ./support_desk.sh <NALUN> <NDISCIP> <NLUG> <NSTUD>"
    exit 1
fi

NALUN=$1
NDISCIP=$2
NLUG=$3
NSTUD=$4

# Calcular o número de horários por disciplina
NHOR=$(( (NALUN + NLUG - 1) / NLUG ))

# Criar o named pipe principal
SUPPORT_PIPE="/tmp/support"
if [ ! -p "$SUPPORT_PIPE" ]; then
    mkfifo "$SUPPORT_PIPE"
fi

# Iniciar o support_agent em background
./support_agent "$NALUN" &

# Executar processos student em background
ALUNOS_POR_STUDENT=$(( NALUN / NSTUD ))
for ((i=0; i<NSTUD; i++)); do
    ALUNO_INICIAL=$(( i * ALUNOS_POR_STUDENT ))
    ./student "$ALUNO_INICIAL" "$ALUNOS_POR_STUDENT" "$SUPPORT_PIPE" &
done

# Esperar todos os processos terminarem
wait

# Limpeza: Remover o named pipe principal
rm -f "$SUPPORT_PIPE"
