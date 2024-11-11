#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int NUM_THREADS;

struct thread_data
{
  int thread_id;
  long int *matA;
  long int *matB;
  long int *matR;
  unsigned int linesMA, columnsMAlinesMB, columnsMB, linha_inicial, linha_final;
};

struct thread_read
{
  int thread_id;
  long int *matRd;
  char *filenameR;
  unsigned int lines,columns;
};

void printMatrix(long int *mat, unsigned int lines, unsigned int columns, char *fileName);

void *readMatrix(void *threadarg);

void *calculaParteMatriz(void *threadarg)
{
   register unsigned int i, j, cm;
   int taskid;
   unsigned int linesM1, columnsM1linesM2, columnsM2,linha_inicial,linha_final;
   long int *mat1;
   long int *mat2;
   long int *mat;
   long int soma;
   float p;
   struct thread_data *my_data;

   my_data = (struct thread_data *) threadarg;
   taskid = my_data->thread_id;
   mat1 = my_data->matA;
   mat2 = my_data->matB;
   mat = my_data->matR;
   linesM1 = my_data->linesMA;
   columnsM1linesM2 = my_data->columnsMAlinesMB;
   columnsM2 = my_data->columnsMB;
   linha_inicial = my_data->linha_inicial;
   linha_final = my_data->linha_final;
   
   for (i=linha_inicial;i<linha_final;i++)
   {
	for (j=0;j<columnsM2;j++)
	{
		soma=0;
		for (cm=0;cm<columnsM1linesM2;cm++)
		{
			soma=(soma+(mat1[i*columnsM1linesM2 + cm]*mat2[cm*columnsM2 + j]));
		}
		mat[i*columnsM2 + j]=soma;
	}
	p=(float)(i-linha_inicial)/(linha_final-linha_inicial)*100;
     	printf("\nThread %d Calculating --> %.2f%%", taskid, p);	
   }
   pthread_exit(NULL);
}

long int *multiplicationMatriz(long int *mat1, long int *mat2, unsigned int linesM1, unsigned int columnsM1linesM2, unsigned int columnsM2)
{
   pthread_t threads[NUM_THREADS];
   pthread_attr_t attr;
 
   register unsigned int linha_inicial, linha_final, contador_linhas;
   long int *mat = (long int *) malloc(sizeof(long int) * linesM1 * columnsM2);
   float p;
 
   long t;
   int rc,sum;
   void *status;
   struct thread_data passa_val[NUM_THREADS];
 
   if (mat == NULL)
   {
       perror("I cannot allocate memory\n");
       exit(EXIT_FAILURE);
       return NULL;
   }

   //cria threads para paralelizar o calculo do produto
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
 
   for(t=0;t<NUM_THREADS;t++) 
   {
	  if(t==0)
	  {
		linha_inicial=t*linesM1/NUM_THREADS;
	  	linha_final=(t+1)*(linesM1/NUM_THREADS);
		contador_linhas=linha_final;
	  }
	  else if (t!=(NUM_THREADS-1))
	  {
		linha_inicial=linha_final;
		linha_final=linha_inicial+contador_linhas;
	  }
	  else
	  {
		linha_inicial=linha_final;
		linha_final=linesM1;
	  }
	
	  passa_val[t].thread_id = t;
	  passa_val[t].matA = (long int *)mat1;
	  passa_val[t].matB = (long int *)mat2;
	  passa_val[t].matR = (long int *)mat;
	  passa_val[t].linesMA = linesM1;
	  passa_val[t].columnsMAlinesMB = columnsM1linesM2;
	  passa_val[t].columnsMB = columnsM2;
	  passa_val[t].linha_inicial = linha_inicial;
	  passa_val[t].linha_final = linha_final;
	  printf("\nCreating thread %ld", t);
	  rc = pthread_create(&threads[t], NULL, calculaParteMatriz, (void *) &passa_val[t]);
	  if (rc) 
	  {
	    printf("ERROR; return code from pthread_create() is %d\n", rc);
	    exit(-1);
	  }
   }
 
   //destroi os atributos e faz join das threads de calculo do produto
   pthread_attr_destroy(&attr);
   for(t=0; t<NUM_THREADS; t++) 
   {
        rc = pthread_join(threads[t], &status);
        if (rc) 
        {
           printf("ERROR; return code from pthread_join() is %d\n", rc);
           exit(-1);
        }
        printf("\nMultiplication: completed join with thread %ld having a status of %ld",t,(long)status);
   }
 
   return mat;
}

int main (int argc, char **argv)
{
   pthread_t thread_rd[2];
   pthread_attr_t attr_rd;
   unsigned int linesM1, columnsM1linesM2, columnsM2;
   char *fileNameInputM1;
   char *fileNameInputM2;
   char *fileNameOutput;
   long int *mat1;
   long int *mat2;
   long int *matRes;
   int rc, tr;
   void *statusR;
   struct thread_read val_read[2];

   switch(argc)
   {
     case 0:
     case 1:
     case 2:
     case 4:
     case 5:
     case 6:
     case 7:
         fprintf(stderr, "Use:\n\t%s <nº of lines Matrix 1> <nº of columns Matrix 1 and Lines Matrix 2> <nº of columns Matrix 2> <input file Matrix 1> <input file Matrix 2> <output file> <number of threads (1..8)>\n", argv[0]); 
         exit(EXIT_FAILURE);
         break;
     case 8:
         linesM1 = atoi(argv[1]);
         columnsM1linesM2 = atoi(argv[2]);
	 columnsM2 = atoi(argv[3]);
         fileNameInputM1 = argv[4];
	 fileNameInputM2 = argv[5];
	 fileNameOutput = argv[6];
	 NUM_THREADS = atoi(argv[7]);

         fprintf(stdout, "Multiplicating %d %d Matrix 1 with %d %d Matrix 2, to %s\n", linesM1, columnsM1linesM2, columnsM1linesM2, columnsM2, fileNameOutput);
         break;
   }
   mat1 = (long int *) malloc (sizeof(long int) * linesM1 * columnsM1linesM2);
   if (mat1 == NULL)
   {
      perror("I cannot allocate memory\n");
      exit(EXIT_FAILURE);
   }
 
   mat2 = (long int *) malloc (sizeof(long int) * columnsM1linesM2 * columnsM2);
   if (mat2 == NULL)
   {
      perror("I cannot allocate memory\n");
      exit(EXIT_FAILURE);
   } 

   pthread_attr_init(&attr_rd);
   pthread_attr_setdetachstate(&attr_rd, PTHREAD_CREATE_JOINABLE);

   //cria 2 threads para paralelizar a leitura das matrizes, 1 para cada.
   val_read[0].thread_id = 0;
   val_read[0].matRd = (long int *)mat1;
   val_read[0].filenameR = (char *)fileNameInputM1;
   val_read[0].lines = linesM1;
   val_read[0].columns = columnsM1linesM2;
 
   rc = pthread_create(&thread_rd[0], NULL, readMatrix, (void *) &val_read[0]);
   if (rc) 
   {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
   }
 
   val_read[1].thread_id = 1;
   val_read[1].matRd = (long int *)mat2;
   val_read[1].filenameR = (char *)fileNameInputM2;
   val_read[1].lines = columnsM1linesM2;
   val_read[1].columns = columnsM2;
 
   rc = pthread_create(&thread_rd[1], NULL, readMatrix, (void *) &val_read[1]);
   if (rc) 
   {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
   }

   //destroi os atributos e faz join das threads de leitura das matrizes de entrada
   pthread_attr_destroy(&attr_rd);
   for(tr=0; tr<2; tr++) 
   {
        rc = pthread_join(thread_rd[tr], &statusR);
        if (rc) 
        {
           printf("ERROR; return code from pthread_join() is %d\n", rc);
           exit(-1);
        }
        printf("\nReading: completed join with thread %d having a status of %ld",tr,(long)statusR);
   }

   matRes = multiplicationMatriz(mat1, mat2, linesM1, columnsM1linesM2, columnsM2);
   printMatrix(matRes, linesM1, columnsM2, fileNameOutput);
   free(mat1);
   free(mat2);
   free(matRes);
   pthread_exit(NULL);
   return 0;
}

void printMatrix(long int *mat, unsigned int lines, unsigned int columns, char *fileName)
{
   register unsigned int i, j;
   register long int value;
   float p;
   FILE *arq;

   if (fileName == NULL)
        arq = stdout;
   else
   {
      arq = fopen(fileName, "w");
      if (arq == NULL)
      {
        perror("I cannot open output file.\n");
        exit(EXIT_FAILURE);
      }
   }

   for (i=0; i<lines; i++)
   {
      for(j=0; j<columns; j++)
      {
          value = mat[i*columns + j];
          if (value >= 0)
            fprintf(arq, " %ld  ",value);
          else
            fprintf(arq, "%ld  ", value);
      }
      fprintf(arq, "\n");
      p=(float)i/lines*100;
      printf("\nWriting %s --> %.2f%%", fileName, p);
   }

   if (fileName != NULL)
     fclose(arq);
   return;
}

void *readMatrix(void *threadarg)
{
   unsigned int lines, columns; 
   char *fileNameRd;
   register unsigned int i, j;
   long int *mat;
   float p;
   int taskid;
   FILE *arq;
   struct thread_read *my_read;

   my_read = (struct thread_read *) threadarg;
   taskid = my_read->thread_id;
   mat = my_read->matRd;
   fileNameRd = my_read->filenameR;
   lines = my_read->lines;
   columns = my_read->columns;

   if (fileNameRd == NULL)
   {
        fprintf(stderr,"File name cannot be null\n");
        exit(EXIT_FAILURE);
   }
 
   arq = fopen(fileNameRd,"r");
   if (arq == NULL)
   {
      perror("I cannot open file to read\n");
      exit(EXIT_FAILURE);
   }

   for (i=0; i<lines; i++)
   {
       for(j=0; j<columns; j++)
       {
           fscanf(arq, "%ld", &mat[i * columns + j]);
       }
       p=(float)i/lines*100;
       printf("\nThread %d Reading %s --> %.2f%%", taskid, fileNameRd, p);	
   }
   fclose(arq);
   pthread_exit(NULL);
}
