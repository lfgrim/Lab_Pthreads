#include <stdio.h>
#include <stdlib.h>

void printMatrix(long int *mat, unsigned int lines, unsigned int columns, char *fileName);

long int *readMatrix(unsigned int lines, unsigned int columns, char *fileName);

long int *multiplicationMatriz(long int *mat1, long int *mat2, unsigned int linesM1, unsigned int columnsM1linesM2, unsigned int columnsM2)
{
 register unsigned int i, j, cm;

 long int *mat = (long int *) malloc(sizeof(long int) * linesM1 * columnsM2);
 long int soma;
 float p;

 if (mat == NULL)
 {
     perror("I cannot allocate memory\n");
     exit(EXIT_FAILURE);
     return NULL;
 }
 
 for (i=0;i<linesM1;i++)
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
	p=(float)i/linesM1*100;
     	printf("\nCalculo --> %.2f%%", p);	
 }
 return mat;
}

int main (int argc, char **argv)
{
 unsigned int linesM1, columnsM1linesM2, columnsM2;
 char *fileNameInputM1;
 char *fileNameInputM2;
 char *fileNameOutput;
 long int *mat1;
 long int *mat2;
 long int *matRes;

 switch(argc)
 {
     case 0:
     case 1:
     case 2:
     case 4:
     case 5:
     case 6:
         fprintf(stderr, "Uso:\n\t%s <nº of lines Matrix 1> <nº of columns Matrix 1 and Lines Matrix 2> <nº of columns Matrix 2> <input file Matrix 1> <input file Matrix 2> <output file>\n", argv[0]);
         exit(EXIT_FAILURE);
         break;
     case 7:
         linesM1 = atoi(argv[1]);
         columnsM1linesM2 = atoi(argv[2]);
	 columnsM2 = atoi(argv[3]);
         fileNameInputM1 = argv[4];
	 fileNameInputM2 = argv[5];
	 fileNameOutput = argv[6];

         fprintf(stdout, "Multiplicating %d, %d Matrix 1 with %d, %d Matrix 2, without values limits to %s\n", linesM1, columnsM1linesM2, columnsM1linesM2, columnsM2, fileNameOutput);
         break;
 }

 mat1 = readMatrix(linesM1, columnsM1linesM2, fileNameInputM1);
 mat2 = readMatrix(columnsM1linesM2, columnsM2, fileNameInputM2);
 matRes = multiplicationMatriz(mat1, mat2, linesM1, columnsM1linesM2, columnsM2);
 printMatrix(matRes, linesM1, columnsM2, fileNameOutput);
 free(mat1);
 free(mat2);
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
      printf("\nSaída %s --> %.2f%%", fileName, p);
  }

  if (fileName != NULL)
     fclose(arq);
  return;
}

long int *readMatrix(unsigned int lines, unsigned int columns, char *fileName)
{
 register unsigned int i, j;
 long int *mat;
 float p;
 FILE *arq;

 if (fileName == NULL)
 {
        fprintf(stderr,"File name cannot be null\n");
        exit(EXIT_FAILURE);
 }
 
 arq = fopen(fileName,"r");
 if (arq == NULL)
 {
   perror("I cannot open file to read\n");
   exit(EXIT_FAILURE);
 }

 mat = (long int *) malloc (sizeof(long int) * lines * columns);

 if (mat == NULL)
 {
    perror("I cannot allocate memory\n");
    exit(EXIT_FAILURE);
 }

 for (i=0; i<lines; i++)
 {
     for(j=0; j<columns; j++)
     {
         fscanf(arq, "%ld", &mat[i * columns + j]);
     }
     p=(float)i/lines*100;
     printf("\nLeitura %s --> %.2f%%", fileName, p);	
 }
 fclose(arq);
 return mat;
}
