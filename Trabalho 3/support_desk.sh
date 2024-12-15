
#!/bin/bash

# Limpa os pipes e arquivos antigos (se existirem)
cleanup() {
    echo "Limpando pipes e processos antigos..."
    rm -f pipe_admin pipe_desk
    pkill -f admin 2>/dev/null
    pkill -f desk 2>/dev/null
}

# Função para criar os pipes necessários
create_pipes() {
    echo "Criando pipes necessários..."
    mkfifo pipe_admin
    mkfifo pipe_desk
}

# Função para iniciar os processos
start_processes() {
    echo "Iniciando processos..."
    
    # Inicia o admin e desk em segundo plano
    ./admin pipe_admin pipe_desk &
    ADMIN_PID=$!
    
    ./desk pipe_admin pipe_desk &
    DESK_PID=$!
    
    echo "Admin iniciado com PID: $ADMIN_PID"
    echo "Desk iniciado com PID: $DESK_PID"
}

# Menu de controle do suporte
menu() {
    while true; do
        echo "------ Suporte Desk ------"
        echo "1) Enviar mensagem para o Admin"
        echo "2) Enviar mensagem para o Desk"
        echo "3) Exibir logs"
        echo "4) Encerrar"
        echo "--------------------------"
        read -rp "Escolha uma opção: " opcao
        
        case $opcao in
            1)
                read -rp "Digite a mensagem para o Admin: " mensagem
                echo "$mensagem" > pipe_admin
                ;;
            2)
                read -rp "Digite a mensagem para o Desk: " mensagem
                echo "$mensagem" > pipe_desk
                ;;
            3)
                echo "---- Logs ----"
                cat logs.txt 2>/dev/null || echo "Sem logs disponíveis."
                ;;
            4)
                echo "Encerrando suporte..."
                cleanup
                exit 0
                ;;
            *)
                echo "Opção inválida. Tente novamente."
                ;;
        esac
    done
}

# Controle de execução do script
if [[ $1 == "start" ]]; then
    cleanup
    create_pipes
    start_processes
    menu
else
    echo "Uso: $0 start"
    exit 1
fi
./support_agent &
./student 1 100 5 &