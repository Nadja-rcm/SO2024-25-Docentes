#!/bin/bash

#PIPE_NAME=$1

#if [ -z "$PIPE_NAME" ]; then
   # echo "Pipe $PIPE_NAME esta vazio "
   # exit 1
#fi

#if [ ! -p "$PIPE_NAME" ]; then
    #echo "Pipe $PIPE_NAME não encontrado, Saindo."
    #exit 1
#fi
   # echo "Pipe encontrado: $PIPE_NAME"

#while true; do
    #if read -r pedido ; then

        #if [ "$pedido" == "quit" ]; then
          #  echo "Pedidos terminados"
           # break
        #fi

        #echo "Tratando o $pedido"
        #sleep $((RANDOM % 5 + 1))

    #fi
#done < "$PIPE_NAME"

#!/bin/bash
# Simulate the support agent reading and processing requests

if [[ $# -eq 1 ]]; then
    pipe=$1
else
    pipe="/tmp/support_pipe"
fi

while read request; do
    #echo "Support Agent: request= $request"
    if [[ $request == "quit" ]]; then
        echo "Support Agent: quit"
        exit
    fi
    echo "Support Agent processing: $request"

    sleep $((1 + $RANDOM % 5))  # Simulate time spent handling the request
done < $pipe

echo "Support Agent: read request terminated"
