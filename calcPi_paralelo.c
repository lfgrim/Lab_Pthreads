#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static long num_steps = 1000000;
double step, sum = 0.0;
int NUM_THREADS;

struct thread_data
{
  long thread_id;
  long step_inicio;
  long step_fim;
};

pthread_mutex_t mutexsum;

void *CalculaPartePI(void *threadarg)
{
   long step_inicial, step_final, tid;
   int i;
   double x, mysum = 0.0;
   struct thread_data *my_data;

   my_data = (struct thread_data *) threadarg;
   tid = my_data->thread_id;
   step_inicial = my_data->step_inicio;
   step_final = my_data->step_fim;
   
   printf("\nThread %ld starting...",tid);
   
   for (i=step_inicial; i<step_final; i++)
   {
     x = (i+0.5)*step;
     mysum = mysum + 4.0/(1.0+x*x);
   }
   //implementa mutex na variável global sum
   pthread_mutex_lock (&mutexsum);
   sum = sum + mysum;
   pthread_mutex_unlock (&mutexsum);
   
   printf("Thread %ld done.\n",tid);
   pthread_exit(NULL);
}


void chama_e_junta_threads()
{
  pthread_t thread[NUM_THREADS];
  pthread_attr_t attr;
  long t; 
  long step_inicial, step_final, contador_steps;
  int rc;
  void *status;
  struct thread_data passa_val[NUM_THREADS];
  
  pthread_mutex_init(&mutexsum, NULL);
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  
  for(t=0; t<NUM_THREADS; t++) 
  {
	if(t==0)
	{
		step_inicial=t*num_steps/NUM_THREADS;
		step_final=(t+1)*(num_steps/NUM_THREADS);
		contador_steps=step_final;
	}
	else if (t!=(NUM_THREADS-1))
	{
		step_inicial=step_final;
		step_final=step_inicial+contador_steps;
	}
	else
	{
		step_inicial=step_final;
		step_final=num_steps;
	}
	passa_val[t].thread_id = t;
	passa_val[t].step_inicio = step_inicial;
	passa_val[t].step_fim = step_final;
	printf("\nMain: creating thread %ld", t);
	rc = pthread_create(&thread[t], &attr, CalculaPartePI, (void *) &passa_val[t]); 
	if (rc) 
	{
		printf("ERROR; return code from pthread_create() is %d\n", rc);
		exit(-1);
	}
  }

  /* Free attribute and wait for the other threads */
  pthread_attr_destroy(&attr);
  for(t=0; t<NUM_THREADS; t++) 
  {
      rc = pthread_join(thread[t], &status);
      if (rc) 
      {
         printf("ERROR; return code from pthread_join() is %d\n", rc);
         exit(-1);
      }
      printf("\nMain: completed join with thread %ld having a status of %ld\n",t,(long)status);
   }
}

int main (int argc, char **argv)
{
  double pi;
  
  switch(argc)
  {
     case 0:
     case 1:
         fprintf(stderr, "Uso:\n\t%s <nº de threads (1..8)>\n", argv[0]); 
         exit(EXIT_FAILURE);
         break;
     case 2:
	 NUM_THREADS = atoi(argv[1]);
         fprintf(stdout, "Calculando PI\n");
         break;
  }
  
  step = 1.0/(double) num_steps;
  
  chama_e_junta_threads();
  
  pi = step * sum;
  printf("Valor de pi: %.30f\n", pi);
  
  pthread_mutex_destroy(&mutexsum);
  pthread_exit(NULL);
  
  return 0;
}
