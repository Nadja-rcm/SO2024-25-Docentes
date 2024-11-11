/**
 * Sistemas Operativos
 * Aplicação Thread Unica
 * @file: single_thread.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
// Adicionando a biblioteca para medir o tempo
#define N_DISCIPLINAS 3
#define N_SECRETARIOS 10
#define N_INSC_PER_PRODUCER 5000000
// Memoria global para as disciplinas
long disciplinas[N_DISCIPLINAS];
/**
 * @brief Este função inscreve um aluno em 3 N_DISCIPLINAS
 * @return void
 */
void inscrever_cadeiras(void)
{
    for (int i = 0; i < N_DISCIPLINAS; i++)
    {
        disciplinas[i] += 1;
    }
}
/**
 * @brief Este função faz as inscrições de um numero de alunos de cada vez
 * @param void *data Um ponteiro genérico para o numero de inscrições a fazer
 * @return void
 */
void *producer(void *data)
{
    int no_inscricoes = *((int *)data);
    for (int i = 0; i < no_inscricoes; i++)
    {
        inscrever_cadeiras();
    }
    return NULL;
}
/**
 * Programa principal
 *
 */
int main(int argc, char const *argv[])
{
    int insc_per_producer = N_INSC_PER_PRODUCER;
    struct timespec start_time, end_time;
    if (argc >= 2)
    {
        sscanf(argv[1], "%d", &insc_per_producer);
        if (insc_per_producer <= 0 || insc_per_producer > N_INSC_PER_PRODUCER)
            insc_per_producer = N_INSC_PER_PRODUCER;
    }

    // Marcando o tempo de início (tempo de parede)
    clock_gettime(CLOCK_MONOTONIC, &start_time);
    for (int i = 0; i < N_SECRETARIOS; i++)
    {
        producer(&insc_per_producer);
    }
    // Marcando o tempo de fim (tempo de parede)
    clock_gettime(CLOCK_MONOTONIC, &end_time);
    for (int i = 0; i < N_DISCIPLINAS; i++)
    {
        printf("Total inscritos na disciplina %d: %ld \n", i, disciplinas[i]);
    }
    // Calculando o tempo total de execução
    double time_taken = (end_time.tv_sec - start_time.tv_sec) +
                        (end_time.tv_nsec - start_time.tv_nsec) / 1e9;
    printf("O programa demorou %f segundos a executar.\n", time_taken);
    return 0;
}