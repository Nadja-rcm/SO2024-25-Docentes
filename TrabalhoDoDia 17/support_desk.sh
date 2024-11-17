#!/bin/bash

# Verifica se os argumentos foram fornecidos
if [ $# -lt 3 ]; then
  echo "Uso: $0 <NALUN> <NDISCIP> <NSTUD>"
  exit 1
fi

NALUN=$1       # Número total de alunos
NDISCIP=$2     # Número de disciplinas
NSTUD=$3       # Número de processos student
SUPPORT_PIPE="/tmp/suporte"

# Cria o named pipe (se não existir)
if [ ! -p "$SUPPORT_PIPE" ]; then
  mkfifo "$SUPPORT_PIPE"
fi

# Calcula o número de horários baseado em NALUN (assumindo 10 lugares por horário)
NLUG=10
NHOR=$(( (NALUN + NLUG - 1) / NLUG ))  # Número de horários, arredondando para cima

# Executa o agente de suporte
./support_agent $NALUN &
./student 1 10 Hello &
./student 2 15 Hello &
./student 3 25 Hello &
