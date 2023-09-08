#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include "diff2d.h"
#include "lut.h"

/*--------------------------------------------------------------------------*/

float **g;                                    /* work copy of f */

void upperHalf(float **f)
{
    float qC, qN, qNE, qE, qSE, qS, qSW, qW, qNW;
    for (int i = 1; i <= 127; i++)
    {
        for (int j = 1; j <= 255; j++)
        {
            qN = LUT[(int)fabs(g[i][j] - g[i][j + 1])];
            qNE = LUT[(int)fabs(g[i][j] - g[i + 1][j + 1])];
            qE = LUT[(int)fabs(g[i][j] - g[i + 1][j])];
            qSE = LUT[(int)fabs(g[i][j] - g[i + 1][j + 1])];
            qS = LUT[(int)fabs(g[i][j] - g[i][j - 1])];
            qSW = LUT[(int)fabs(g[i][j] - g[i - 1][j - 1])];
            qW = LUT[(int)fabs(g[i][j] - g[i - 1][j])];
            qNW = LUT[(int)fabs(g[i][j] - g[i - 1][j + 1])];
            qC = 1.0 - qN - qNE - qE - qSE - qS - qSW - qW - qNW;

            f[i - 1][j - 1] = qNW * g[i - 1][j + 1] + qN * g[i][j + 1] + qNE * g[i + 1][j + 1] +
                              qW * g[i - 1][j] + qC * g[i][j] + qE * g[i + 1][j] +
                              qSW * g[i - 1][j - 1] + qS * g[i][j - 1] + qSE * g[i + 1][j - 1];
        }
    }
}

void lowerHalf(float **f)
{
    float qC, qN, qNE, qE, qSE, qS, qSW, qW, qNW;
    for (int i = 128; i <= 255; i++)
    {
        for (int j = 1; j <= 255; j++)
        {
            qN = LUT[(int)fabs(g[i][j] - g[i][j + 1])];
            qNE = LUT[(int)fabs(g[i][j] - g[i + 1][j + 1])];
            qE = LUT[(int)fabs(g[i][j] - g[i + 1][j])];
            qSE = LUT[(int)fabs(g[i][j] - g[i + 1][j + 1])];
            qS = LUT[(int)fabs(g[i][j] - g[i][j - 1])];
            qSW = LUT[(int)fabs(g[i][j] - g[i - 1][j - 1])];
            qW = LUT[(int)fabs(g[i][j] - g[i - 1][j])];
            qNW = LUT[(int)fabs(g[i][j] - g[i - 1][j + 1])];
            qC = 1.0 - qN - qNE - qE - qSE - qS - qSW - qW - qNW;

            f[i - 1][j - 1] = qNW * g[i - 1][j + 1] + qN * g[i][j + 1] + qNE * g[i + 1][j + 1] +
                              qW * g[i - 1][j] + qC * g[i][j] + qE * g[i + 1][j] +
                              qSW * g[i - 1][j - 1] + qS * g[i][j - 1] + qSE * g[i + 1][j - 1];
        }
    }
}


void diff2d

    (float ht,     /* time step size, >0, e.g. 0.5 */
     float lambda, /* contrast parameter */
     long nx,      /* image dimension in x direction */
     long ny,      /* image dimension in y direction */
     float **f)    /* input: original image ;  output: smoothed */

/*--------------------------------------------------------------------------*/
/*                                                                          */
/*             NONLINEAR TWO DIMENSIONAL DIFFUSION FILTERING                */
/*                                                                          */
/*                       (Joachim Weickert, 7/1994)                         */
/*                                                                          */
/*--------------------------------------------------------------------------*/

/* Explicit scheme with 9-point stencil and exponential stabilization.      */
/* Conservative, conditionally consistent to the discrete integration       */
/* model, unconditionally stable, preserves maximum-minimum principle.      */

{

    long i, j;                                    /* loop variables */
    float qC, qN, qNE, qE, qSE, qS, qSW, qW, qNW; /* weights */

    /* ---- allocate storage for g ---- */

    g = (float **)malloc((nx + 2) * sizeof(float *));
    if (g == NULL)
    {
        printf("not enough storage available\n");
        exit(1);
    }
    for (i = 0; i <= nx + 1; i++)
    {
        g[i] = (float *)malloc((ny + 2) * sizeof(float));
        if (g[i] == NULL)
        {
            printf("not enough storage available\n");
            exit(1);
        }
    }

    /* ---- copy f into g ---- */

    for (i = 1; i <= nx; i++)
        for (j = 1; j <= ny; j++)
            g[i][j] = f[i - 1][j - 1];

    /* ---- create dummy boundaries ---- */

    for (i = 1; i <= nx; i++)
    {
        g[i][0] = g[i][1];
        g[i][ny + 1] = g[i][ny];
    }

    for (j = 0; j <= ny + 1; j++)
    {
        g[0][j] = g[1][j];
        g[nx + 1][j] = g[nx][j];
    }

    /* ---- diffusive averaging ---- */

    pthread_t upperHalf_tid, lowerHalf_tid;
    pthread_create(&upperHalf_tid, NULL, (void *)upperHalf, f);
    pthread_create(&lowerHalf_tid, NULL, (void *)lowerHalf, f);
    pthread_join(upperHalf_tid, NULL);
    pthread_join(lowerHalf_tid, NULL);

    /* ---- disallocate storage for g ---- */

    for (i = 0; i <= nx + 1; i++)
        free(g[i]);
    free(g);

    return;

} /* diff */

/*--------------------------------------------------------------------------*/
