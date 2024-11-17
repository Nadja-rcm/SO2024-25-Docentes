#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>

#define MAX_MSG_SIZE 256
#define SUPPORT_PIPE "/tmp/support"
#define MAX_DISCIPLINAS 5

typedef struct {
    char nome[32];
    int inscritos;
    pthread_mutex_t lock; //para proteger o acesso ao número de inscritos
} Disciplina;

Disciplina disciplinas[MAX_DISCIPLINAS];

// Mutexes globais
pthread_mutex_t pipe_lock = PTHREAD_MUTEX_INITIALIZER;  // Para proteger o pipe, o thread lê do pipe por vez
pthread_mutex_t alunos_lock = PTHREAD_MUTEX_INITIALIZER;  // Para proteger a contagem de alunos

int alunos_inscritos = 0;

// Função para inicializar as disciplinas
void inicializar_disciplinas() {
    for (int i = 0; i < MAX_DISCIPLINAS; i++) {
        snprintf(disciplinas[i].nome, sizeof(disciplinas[i].nome), "Disciplina %d", i + 1);
        disciplinas[i].inscritos = 0;
        pthread_mutex_init(&(disciplinas[i].lock), NULL);
    }
}

// Função para inscrever alunos nas disciplinas
void inscrever_alunos(int aluno_inicial, int num_alunos) {
    for (int i = 0; i < MAX_DISCIPLINAS; i++) {
        pthread_mutex_lock(&(disciplinas[i].lock));
        disciplinas[i].inscritos += num_alunos;
        printf("Inscrição concluida: %s - Total inscritos: %d\n", disciplinas[i].nome, disciplinas[i].inscritos);
        pthread_mutex_unlock(&(disciplinas[i].lock));
    }
}

// Função para lidar com cada thread de suporte
void *support_thread(void *arg) {

    // Abrir o pipe para leitura
    int pipe_fd = open(SUPPORT_PIPE, O_RDONLY);
    if (pipe_fd < 0) {
        perror("Não deu para abrir o pipe de suporte ");
        return NULL;
    }

    char buf[MAX_MSG_SIZE];
    int bytes_read;

    while (1) {
        pthread_mutex_lock(&pipe_lock);
        bytes_read = read(pipe_fd, buf, MAX_MSG_SIZE);
        pthread_mutex_unlock(&pipe_lock);

        if (bytes_read > 0) {
            printf("Mensagem recebida: %s\n", buf);

            int aluno_inicial, num_alunos;
            char resp_pipe_name[255];
            sscanf(buf, "%d %d %s", &aluno_inicial, &num_alunos, resp_pipe_name);

            // Inscrever alunos nas disciplinas
            inscrever_alunos(aluno_inicial, num_alunos);

            // Atualizar o total de alunos inscritos
            pthread_mutex_lock(&alunos_lock);
            alunos_inscritos += num_alunos;
            pthread_mutex_unlock(&alunos_lock);

            // Enviar resposta ao aluno
            int student_pipe_fd = open(resp_pipe_name, O_WRONLY);
            if (student_pipe_fd < 0) {
                perror("Não deu para abrir o pipe do aluno");
                continue;
            }

            char response[MAX_MSG_SIZE];
            int resp_len = sprintf(response, "%d alunos inscritos", num_alunos);
            if (write(student_pipe_fd, response, resp_len + 1) < 0) {
                perror("Error writing to student pipe");
            } else {
                printf("Sent response to student: %s\n", response);
            }

            close(student_pipe_fd);
        } else if (bytes_read < 0) {
            perror("Error reading from support pipe");
        }
    }

    // Fechar o pipe de leitura
    close(pipe_fd);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <total_students>\n", argv[0]);
        return 1;
    }

    int total_students = atoi(argv[1]);
    printf("Support agent is running and waiting for messages...\n");

    inicializar_disciplinas();

    int num_threads = 5;
    pthread_t threads[num_threads];

    // Iniciar threads de suporte
    for (int i = 0; i < num_threads; i++) {
        pthread_create(&(threads[i]), NULL, support_thread, NULL);
    }

    // Esperar que todos os alunos sejam inscritos
    while (1) {
        pthread_mutex_lock(&alunos_lock);
        if (alunos_inscritos >= total_students) {
            pthread_mutex_unlock(&alunos_lock);
            break;
        }
        pthread_mutex_unlock(&alunos_lock);
        usleep(1000);  // Dormir um pouco
    }

    // Cancelar threads
    for (int i = 0; i < num_threads; i++) {
        pthread_cancel(threads[i]);
    }

    // Liberar recursos
    for (int i = 0; i < MAX_DISCIPLINAS; i++) {
        pthread_mutex_destroy(&(disciplinas[i].lock));
    }

    pthread_mutex_destroy(&pipe_lock);
    pthread_mutex_destroy(&alunos_lock);

    return 0;
}
