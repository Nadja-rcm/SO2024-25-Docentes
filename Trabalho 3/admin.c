#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h> // Cabeçalho necessário para mkfifo()

#define ADMIN_PIPE "/tmp/admin"
#define MAX_MSG_SIZE 256

void menu() {
    printf("1. Pedir horários de um aluno\n");
    printf("2. Gravar em ficheiro\n");
    printf("3. Terminar support_agent\n");
    printf("Escolha: ");
}

int main() {
    char response_pipe[64] = "/tmp/admin_resp";
    mkfifo(response_pipe, 0666);

    while (1) {
        menu();
        int escolha;
        scanf("%d", &escolha);

        char buffer[MAX_MSG_SIZE];
        int admin_fd = open(ADMIN_PIPE, O_WRONLY);

        if (admin_fd < 0) {
            perror("Erro ao abrir o pipe admin");
            unlink(response_pipe);
            return 1;
        }

        if (escolha == 1) {
            int num_aluno;
            printf("Número do aluno: ");
            scanf("%d", &num_aluno);

            snprintf(buffer, sizeof(buffer), "%d %d %s", 1, num_aluno, response_pipe);
        } else if (escolha == 2) {
            char ficheiro[64];
            printf("Nome do ficheiro: ");
            scanf("%s", ficheiro);

            snprintf(buffer, sizeof(buffer), "%d %s %s", 2, ficheiro, response_pipe);
        } else if (escolha == 3) {
            snprintf(buffer, sizeof(buffer), "%d %s", 3, response_pipe);
        } else {
            printf("Opção inválida\n");
            close(admin_fd);
            continue;
        }

        write(admin_fd, buffer, strlen(buffer) + 1);
        close(admin_fd);

        int resp_fd = open(response_pipe, O_RDONLY);
        if (resp_fd >= 0) {
            char response[MAX_MSG_SIZE];
            read(resp_fd, response, sizeof(response));
            printf("Resposta: %s\n", response);
            close(resp_fd);
        } else {
            perror("Erro ao abrir o pipe de resposta");
        }

        if (escolha == 3) break;
    }

    unlink(response_pipe);
    return 0;
}
