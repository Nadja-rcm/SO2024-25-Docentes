#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define SUPPORT_PIPE "/tmp/support"
#define MAX_DISCIPLINAS 10
#define MAX_MSG_SIZE 256

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Uso: %s <student_id> <aluno_inicial> <num_alunos>\n", argv[0]);
        return 1;

    int student_id = atoi(argv[1]);
    int aluno_inicial = atoi(argv[2]);
    int num_alunos = atoi(argv[3]);

    char student_pipe[64];
    snprintf(student_pipe, sizeof(student_pipe), "/tmp/student_%d", student_id);
    mkfifo(student_pipe, 0666);

    printf("student %d: aluno inicial=%d, número de alunos=%d\n", student_id, aluno_inicial, num_alunos);

    srand(time(NULL) + student_id);
    for (int i = 0; i < num_alunos; i++) {
        int num_aluno = aluno_inicial + i;
        printf("student %d, aluno %d:", student_id, num_aluno);

        for (int j = 0; j < 5; j++) {
            int disciplina = rand() % MAX_DISCIPLINAS;

            char buffer[MAX_MSG_SIZE];
            snprintf(buffer, sizeof(buffer), "%d %d %s", num_aluno, disciplina, student_pipe);

            int support_fd = open(SUPPORT_PIPE, O_WRONLY);
            if (support_fd >= 0) {
                write(support_fd, buffer, strlen(buffer) + 1);
                close(support_fd);
            }

            int response_fd = open(student_pipe, O_RDONLY);
            if (response_fd >= 0) {
                char response[32];
                read(response_fd, response, sizeof(response));
                close(response_fd);

                printf(" %d/%s,", disciplina, response);
            }
        }
        printf("\n");
    }

    unlink(student_pipe);
    return 0;
}
}
