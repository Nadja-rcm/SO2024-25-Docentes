#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define SUPPORT_PIPE "/tmp/support"
#define MAX_MSG_SIZE 80

// Função para ler do pipe até encontrar o '\0'
int read_pipe(int pipe, char *buffer, int max_buffer_size) {
    int i = 0;
    char ch;
    while (i < max_buffer_size - 1) {
        if (read(pipe, &ch, 1) <= 0) {
            return -1; // erro ou EOF
        }
        if (ch == '\0') {
            break;
        }
        buffer[i++] = ch;
    }
    buffer[i] = '\0';
    return i; // tamanho da mensagem lida
}

int main(int argc, char *argv[]) {
    int student_inicial, numero_students;
    char *support_pipename = SUPPORT_PIPE;

    // Verificar argumentos
    if ((argc < 3) || (argc > 4)) {
        printf("Uso: ./student <student_inicial> <num_students> [<support_pipe>]\n");
        return 1;
    }

    student_inicial = atoi(argv[1]);
    numero_students = atoi(argv[2]);

    if (argc == 4) {
        support_pipename = argv[3];
    }

    // Criar o pipe de leitura para este student ("/tmp/student_<student_inicial>")
    char *pipe_in_name = NULL;
    asprintf(&pipe_in_name, "/tmp/student_%d", student_inicial);
    if (mkfifo(pipe_in_name, 0666) < 0) {
        perror("Erro ao criar o named pipe de leitura");
        return 1;
    }

    int pipe_in = open(pipe_in_name, O_RDONLY | O_NONBLOCK);
    if (pipe_in < 0) {
        perror("Erro ao abrir o named pipe de leitura");
        return 1;
    }

    int pipe_out = open(support_pipename, O_WRONLY);
    if (pipe_out < 0) {
        perror("Erro ao abrir o named pipe de escrita");
        return 1;
    }

    // Criar a mensagem para enviar
    char *message = NULL;
    int msg_len = asprintf(&message, "%d %d %s", student_inicial, numero_students, pipe_in_name);
    // Enviar mensagem para o pipe ("/tmp/support")
    if (write(pipe_out, message, msg_len + 1) < 0) {
        perror("Erro ao enviar mensagem pelo pipe de escrita");
        return 1;
    }
    free(message);

    // Ler resposta via o pipe ("/tmp/student_<student_inicial>")
    char rcv_msg[MAX_MSG_SIZE];
    int rcv_msg_len = read_pipe(pipe_in, rcv_msg, MAX_MSG_SIZE);
    if (rcv_msg_len < 0) {
        perror("Erro ao ler a resposta do pipe de leitura");
        return 1;
    }

    // Imprimir a mensagem recebida
    printf("Student %d: alunos inscritos=%s\n", student_inicial, rcv_msg);

    // Fechar e remover o pipe de leitura
    close(pipe_in);
    unlink(pipe_in_name);
    free(pipe_in_name);

    return 0;
}
