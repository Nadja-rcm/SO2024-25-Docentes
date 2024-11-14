#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/stat.h>
#include <errno.h>

#define SUPPORT_PIPE "/tmp/support"
#define MAX_MSG_SIZE 80
#define NDISCIP 5
#define NHOR 3
#define NLUG 10

// Estrutura para armazenar informações das disciplinas e horários
typedef struct {
    int vagas[NHOR];
} Disciplina;

Disciplina disciplinas[NDISCIP];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

// Função para processar pedidos e inscrever alunos
void *process_request(void *arg) {
    int pipe_in_fd = *(int *)arg;
    free(arg);
    
    char buffer[MAX_MSG_SIZE];
    while (1) {
        int len = read(pipe_in_fd, buffer, MAX_MSG_SIZE - 1);
        if (len <= 0) continue;
        
        buffer[len] = '\0';

        // Extrair informações da mensagem recebida
        int aluno_inicial, num_alunos;
        char response_pipe[MAX_MSG_SIZE];
        sscanf(buffer, "%d %d %s", &aluno_inicial, &num_alunos, response_pipe);

        int alunos_inscritos = 0;

        // Lock para garantir exclusão mútua ao acessar as vagas
        pthread_mutex_lock(&lock);
        for (int d = 0; d < NDISCIP && alunos_inscritos < num_alunos; d++) {
            for (int h = 0; h < NHOR && alunos_inscritos < num_alunos; h++) {
                if (disciplinas[d].vagas[h] > 0) {
                    int vagas = (num_alunos - alunos_inscritos) < disciplinas[d].vagas[h]
                        ? (num_alunos - alunos_inscritos) : disciplinas[d].vagas[h];
                    disciplinas[d].vagas[h] -= vagas;
                    alunos_inscritos += vagas;
                }
            }
        }
        pthread_mutex_unlock(&lock);

        // Enviar resposta para o student
        int response_fd = open(response_pipe, O_WRONLY);
        if (response_fd >= 0) {
            char response_msg[MAX_MSG_SIZE];
            snprintf(response_msg, MAX_MSG_SIZE, "%d", alunos_inscritos);
            write(response_fd, response_msg, strlen(response_msg) + 1);
            close(response_fd);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: ./support_agent <numero_de_alunos>\n");
        return 1;
    }
    
    int num_alunos = atoi(argv[1]);

    // Inicializar vagas das disciplinas
    for (int i = 0; i < NDISCIP; i++) {
        for (int j = 0; j < NHOR; j++) {
            disciplinas[i].vagas[j] = NLUG;
        }
    }

    // Criar o pipe principal para receber os pedidos
    mkfifo(SUPPORT_PIPE, 0666);
    int pipe_in_fd = open(SUPPORT_PIPE, O_RDONLY);
    if (pipe_in_fd < 0) {
        perror("Erro ao abrir o pipe principal");
        return 1;
    }

    // Criar threads para processar os pedidos
    pthread_t threads[10];
    for (int i = 0; i < 10; i++) {
        int *arg = malloc(sizeof(int));
        *arg = pipe_in_fd;
        pthread_create(&threads[i], NULL, process_request, arg);
    }

    // Aguardar todos os alunos serem inscritos
    while (num_alunos > 0) {
        sleep(1);
        pthread_mutex_lock(&lock);
        int total_vagas = 0;
        for (int d = 0; d < NDISCIP; d++) {
            for (int h = 0; h < NHOR; h++) {
                total_vagas += disciplinas[d].vagas[h];
            }
        }
        if (total_vagas == 0) {
            num_alunos = 0;
        }
        pthread_mutex_unlock(&lock);
    }

    // Fechar e remover o pipe principal
    close(pipe_in_fd);
    unlink(SUPPORT_PIPE);

    return 0;
}
