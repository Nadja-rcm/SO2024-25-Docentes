/*#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "ERRO %s \n", argv[0]);
        return 1;
    }

    char *pipe_name = argv[1];
    char *mensagem = argv[2];

    int fd = open(pipe_name, O_WRONLY);
    if (fd == -1) {
        perror("Erro ao abrir o pipe");
        return 1;
    }

    char buf[80]; 
    strcpy(buf, mensagem);
    strcat(buf, "\n");
    
    printf("Enviar %s", buf);
    write(fd, buf, strlen(buf));

    close(fd);

    return 0;

    
}*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define BSIZE 80
#define NOMEFIFO "/tmp/support_pipe"

int main(int argc, char const *argv[])
{
    //Codigo de resolução
    char const *nome_pipe;
    int fd;
    char buf[BSIZE];

    if (argc == 3) {
        nome_pipe = argv[2];
    } else {
        nome_pipe = NOMEFIFO;
    }
    if ((fd = open (nome_pipe, O_WRONLY)) < 0) {
        perror ("open");
        exit(1);
    }
    strcpy (buf, argv[1]);
    strcat (buf, "\n");
    //printf ("student: %s, buf=%s", argv[1], buf);
    write (fd, buf, strlen(buf)+1);
    return 0;
}


