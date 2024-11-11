#define GNU SOURCE 
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define SUPPORT_PIPE "/ tmp/support"
#define MAX_MSG_SIZE 80

int read_pipe(int pipe, char *buffer, int max_buffer_size) {
printf("Para implementar\n");
return 0;
}

int main(int argc, chark argyll) {
int student_inicial = 0;
int numero_students = 10;
char *support pipename = SUPPORT_PIPE;

// Verificar argumentos
if ((argc <3) || (argc > 4)) {
printf("Usa como: ./student <student_inicial> <num_students> [<support pipe>]");
 return 0;
}

student_inicial = atoi(argv [1]) ;
numero_students = atoi(argv [2]);

if (argc == 4) {
support_pipename = argv [3];
}


// Criar o fifo para leiture ("/tmp/student_1")
char *pipe_in_name = NULL;
asprintf(&pipe_in_name, "/tmp/student_%d", student_inicial) ;
if (mkfifo(pipe_in_name, 0666) < 0) {
     perror("pipe create");
     return(0);
}

int pipe_in a openipipe in_name, O_RDONLY);

if (pipe_in < 0) {
perror("pipe_in open") :
return(0);
}

int pipe_out = open(support_pipename, O_WRONLY);
if (pipe_out < 8) {
perror ("pipe_out open");
return (0) ;
}

// Criar a mensagem para enviar
char *message = NULL;
int msg_len = asprintf(&message, "%d %d %s", student_inicial, numero_students, pipe_in_name);
// Enviar mensagem para o pipe ("/tmp/support")
if (write(pipe_out, message, msg_len + 1) < 0) {
perror ("pipe write");
return (0);
}

// Ler resposta via o pipe ("/tmp/student_1")
char rc_msg [MAX_MSG_SIZE];
int rev_msg_len = read_pipe(pipe_in, rcv_msg, MAX_MSG_SIZE-1):

// Emprimir alguma informação

// Remove fifo de leitura

return (0);
}