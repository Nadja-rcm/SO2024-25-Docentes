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
#define ADMIN_PIPE "/tmp/admin"
#define MAX_DISCIPLINAS 10
#define MAX_HORARIOS 10
#define MAX_VAGAS 10

typedef struct {
    int inscritos[MAX_VAGAS]; // IDs dos alunos inscritos
    int num_inscritos;        // Número de alunos inscritos
} Horario;

typedef struct {
    char nome[32];
    Horario horarios[MAX_HORARIOS];
} Disciplina;

typedef struct {
    int num_aluno;
    int disciplinas[5];
    int horarios[5];
} Inscricao;

Disciplina disciplinas[MAX_DISCIPLINAS];
pthread_mutex_t lock_disciplinas[MAX_DISCIPLINAS];

//Função para inicializar as disciplinas
void inicializar_disciplinas() {
    for (int i = 0; i < MAX_DISCIPLINAS; i++) {
        snprintf(disciplinas[i].nome, sizeof(disciplinas[i].nome), "Disciplina %d", i);
        for (int j = 0; j < MAX_HORARIOS; j++) {
            disciplinas[i].horarios[j].num_inscritos = 0;
        }
        pthread_mutex_init(&lock_disciplinas[i], NULL);
    }
}

//Função para inscrever um aluno em uma disciplina/horário
int inscrever_aluno(int num_aluno, int disciplina) {
    int horario_inscrito = -1;
    pthread_mutex_lock(&lock_disciplinas[disciplina]);

    for (int i = 0; i < MAX_HORARIOS; i++) {
        if (disciplinas[disciplina].horarios[i].num_inscritos < MAX_VAGAS) {
            disciplinas[disciplina].horarios[i].inscritos[disciplinas[disciplina].horarios[i].num_inscritos++] = num_aluno;
            horario_inscrito = i;
            break;
        }
    }
    pthread_mutex_unlock(&lock_disciplinas[disciplina]);
    return horario_inscrito;
}

// Thread para lidar com pedidos de students
void *student_thread(void *arg) {
    printf("Iniciando programa\n");
    int pipe_fd = open(SUPPORT_PIPE, O_RDONLY);
    printf("Iniciando programa\n");
    if (pipe_fd < 0) {
        perror("Erro ao abrir o pipe de suporte");
        return NULL;
    }

    char buf[MAX_MSG_SIZE];
    while (read(pipe_fd, buf, sizeof(buf)) > 0) {
        int num_aluno, disciplina;
        char response_pipe[64];
        sscanf(buf, "%d %d %s", &num_aluno, &disciplina, response_pipe);

        int horario = inscrever_aluno(num_aluno, disciplina);

        int resp_fd = open(response_pipe, O_WRONLY);
        if (resp_fd >= 0) {
            dprintf(resp_fd, "%d\n", horario);
            close(resp_fd);
        }
    }
    close(pipe_fd);
    return NULL;
}

// Thread para lidar com pedidos de admin
void *admin_thread(void *arg) {
    int pipe_fd = open(ADMIN_PIPE, O_RDONLY);
    if (pipe_fd < 0) {
        perror("Erro ao abrir o pipe admin");
        return NULL;
    }

    char buf[MAX_MSG_SIZE];
    while (read(pipe_fd, buf, sizeof(buf)) > 0) {
        char response_pipe[64];
        int codop;
        sscanf(buf, "%d %s", &codop, response_pipe);

        if (codop == 3) { // Terminar
            int resp_fd = open(response_pipe, O_WRONLY);
            if (resp_fd >= 0) {
                dprintf(resp_fd, "Ok\n");
                close(resp_fd);
            }
            break; 
        }
        // Implementar outras operações como gravar em arquivo, consultar horários, etc.
    }
    close(pipe_fd);
    return NULL;
}

int main() {


    inicializar_disciplinas();

    // Criação dos pipes
    mkfifo(SUPPORT_PIPE, 0666);
    mkfifo(ADMIN_PIPE, 0666);

    // Criação das threads
    pthread_t student_handler, admin_handler;
    pthread_create(&student_handler, NULL, student_thread, NULL);
    pthread_create(&admin_handler, NULL, admin_thread, NULL);

    // Aguardar threads
    pthread_join(student_handler, NULL);
    pthread_join(admin_handler, NULL);

    // Destruir mutexes
    for (int i = 0; i < MAX_DISCIPLINAS; i++) {
        pthread_mutex_destroy(&lock_disciplinas[i]);
    }

    unlink(SUPPORT_PIPE);
    unlink(ADMIN_PIPE);
    return 0;
}
