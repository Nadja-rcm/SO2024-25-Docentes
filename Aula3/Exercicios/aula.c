#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define NUM_THREADS	5

void *PrintHello(void *threadid)
{
   long tid = (long)threadid;
   int tsleep = rand()%5;
   sleep(rand()%5);
   printf("Hello World! It's me, thread #%ld, slept %d!\n", tid, tsleep);
   pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
   pthread_t threads[NUM_THREADS];
   int rc;
   long t;
   for(t=0;t<NUM_THREADS;t++){
     printf("In main: creating thread %ld\n", t);
     rc = pthread_create(&threads[t], NULL, PrintHello, (void *)t);
     if (rc){
       printf("ERROR; return code from pthread_create() is %d\n", rc);
       exit(-1);
       }
     }

    printf("Goodbye cruel world!\n");

   /* Last thing that main() should do */
   pthread_exit(NULL);
}