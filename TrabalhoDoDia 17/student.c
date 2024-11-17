#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define MAX_MSG_SIZE 300
#define SUPPORT_PIPE "/tmp/support"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Parâmetros: %s <student_id> <num_students> <message>\n", argv[0]);
        return 1;
    }

    //atoi para converter os argumentos para inteiros

    int student_id = atoi(argv[1]);
    int num_student = atoi(argv[2]);
    char *message = argv[3];


    //snprintf(pra onde vai, size do buffer, "formato", o q sera formatado);
    char pipe_name[100];
    snprintf(pipe_name, sizeof(pipe_name), "%d", student_id);

    // Verifica se o pipe do estudante já existe e remove se necessário
    if (access(pipe_name, F_OK) == 0) {
        unlink(pipe_name);  // Remove o pipe se ele já existir
    }

    // Cria o pipe do estudante
    if (mkfifo(pipe_name, 0666) < 0) {
        perror("Erro ao criar o pipe");
        return 1;
    }

    printf("Pipe criado: %s\n", pipe_name);

    // Verifica se o pipe de suporte existe e cria se necessário 
    //No access recebe o caminho a ser verificado e o f_ok verifica se existe
    if (access(SUPPORT_PIPE, F_OK) != 0) {
        if (mkfifo(SUPPORT_PIPE, 0666) < 0) {
            perror("Erro ao cria o pipe do suporte");
            return 1;
        }
    }

    // Abre o pipe de suporte
    int support_pipe_fd = open(SUPPORT_PIPE, O_WRONLY);
    if (support_pipe_fd < 0) {
        perror("Não deu para abrir o pipe");
        return 1;
    }

    // Abre o pipe de suporte para enviar mensagem
    char buffer[MAX_MSG_SIZE];
    snprintf(buffer, sizeof(buffer), "%d %d %s\n", student_id, num_student, pipe_name); // Adicionar '\n' à mensagem
    if (write(support_pipe_fd, buffer, strlen(buffer) + 1) < 0) {
        perror("Não deu para escrever no pipe");
        close(support_pipe_fd);  // Fechar pipe antes de retornar
        return 1;
    }

    printf("Mensagem enviada: %s\n", buffer);

    // Abrir pipe para leitura
    //Abre o pipe exclusivo do estudante no modo de leitura para receber a resposta
    int pipe_fd = open(pipe_name, O_RDONLY);
    if (pipe_fd < 0) {
        perror("Erro ao abrir a resposta do aluno");
        unlink(pipe_name); // Remover pipe em caso de erro
        close(support_pipe_fd); // Fechar o pipe de suporte
        return 1;
    }

    // Ler resposta do suporte (se necessário)
    char response[MAX_MSG_SIZE];
    if (read(pipe_fd, response, sizeof(response)) < 0) {
        perror("Error ao ler a resposta do aluno");
    } else {
        printf("Resposta do aluno: %s\n", response);
    }

    // Fechar e remover pipes
    close(pipe_fd);
    unlink(pipe_name);
    close(support_pipe_fd);

    return 0;
}
