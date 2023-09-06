#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "lut.h"
#include "pgmfiles.h"
#include "diff2d.h"
#include <time.h>

//gcc -o fda pgmtolist.c pgmfiles.c diff2d.c main.c -lm
void main (int argc, char **argv) {
  char   row[80];
  float  **matrix;
  int i, j;
  FILE   *inimage, *outimage;
  long   imax;
  float  lambda;
  int result;
  eightBitPGMImage *PGMImage;

  FILE *resultfile;
  char fileName[] = "resultfile.txt";
  
  // calculate LUT with lambda = 15
  generateLut(15.0);

  /* ---- read image name  ---- */

  PGMImage = (eightBitPGMImage *) malloc(sizeof(eightBitPGMImage));
	
  if (!argv[1])
  {
    printf("name of input PGM image file (with extender): ");
    scanf("%s", PGMImage->fileName);
  }
  else
  {
    strcpy(PGMImage->fileName, argv[1]);
  }

  int read8BitImage_cstart = clock();
  result = read8bitPGM(PGMImage);
  int read8BitImage_cend = clock();
  resultfile = fopen(fileName, "w");
  fprintf(resultfile, "read8BitPGM clock diff: %3ld \n*-----------------------------*\n\n", read8BitImage_cend - read8BitImage_cstart);
  fclose(resultfile);

  if(result < 0) 
    {
      printPGMFileError(result);
      exit(result);
    }

  /* ---- allocate storage for matrix ---- */

  matrix = (float **) malloc (PGMImage->x * sizeof(float *));
  if (matrix == NULL)
    { 
      printf("not enough storage available\n");
      exit(1);
    }
  for (i=0; i<PGMImage->x; i++)
    {
      matrix[i] = (float *) malloc (PGMImage->y * sizeof(float));
      if (matrix[i] == NULL)
        {  
          printf("not enough storage available\n");
          exit(1);
        }
    }
  
  /* ---- read image data into matrix ---- */
  int copytoMatrix_cstart = clock();
  for (i=0; i<PGMImage->x; i++)
      for (j=0; j<PGMImage->y; j++)
        matrix[i][j] = (float) *(PGMImage->imageData + (i*PGMImage->y) + j); 
  int copytoMatrix_cend = clock();

  /* ---- process image ---- */
  printf("number of iterations: ");
  //~ gets(row);  sscanf(row, "%ld", &imax);
  scanf("%ld", &imax);
  int diff2d_cstart, diff2d_cend, diff2d_diff, sum = 0;
  float mean;
  resultfile = fopen(fileName, "a");
  fprintf(resultfile, "diff2d clock diff:\n");
  for (i=1; i<=imax; i++)
    {
      printf("iteration number: %3ld \n", i);
      diff2d_cstart = clock();
      diff2d (0.5, lambda, PGMImage->x, PGMImage->y, matrix);
      diff2d_cend = clock();
      fprintf(resultfile, "iteraction: %3ld -> %3ld \n", i, diff2d_cend - diff2d_cstart);
      sum += diff2d_cend - diff2d_cstart;
    }
  fprintf(resultfile, "mean: %f \n*-----------------------------*\n\n", (float)sum/imax);
  fclose(resultfile);

  /* copy the Result Image to PGM Image/File structure */
  resultfile = fopen(fileName, "a");
  int copyResultToPGM_cstart = clock();
  for (i=0; i<PGMImage->x; i++)
    for (j=0; j<PGMImage->y; j++)
      *(PGMImage->imageData + i*PGMImage->y + j) = (char) matrix[i][j];
  int copyResultToPGM_cend = clock();
  fprintf(resultfile, "copyResultToPGM clock diff: %3ld \n*-----------------------------*\n\n", copyResultToPGM_cend - copyResultToPGM_cstart);
  fclose(resultfile);

  /* ---- write image ---- */
  
  if (!argv[2])
  {
    printf("name of output PGM image file (with extender): ");
    scanf("%s", PGMImage->fileName);
  }
  else
  {
    strcpy(PGMImage->fileName, argv[2]);
  }

  resultfile = fopen(fileName, "a");
  int write8bitPGM_cstart = clock();
  write8bitPGM(PGMImage);
  int write8bitPGM_cend = clock();
  fprintf(resultfile, "write8bitPGM clock diff: %3ld \n*-----------------------------*\n\n", write8bitPGM_cend - write8bitPGM_cstart);
  fclose(resultfile);

  /* ---- disallocate storage ---- */
  
  for (i=0; i<PGMImage->x; i++)
    free(matrix[i]);
  free(matrix);

  free(PGMImage->imageData);
  free(PGMImage);
}
