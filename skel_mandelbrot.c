/**
 * Compute the Area of the Mandelbrot Set
 *
 */
#include <stdio.h>
#include <omp.h>

#define NPOINTS 1600
#define MAXITER 1000

struct complex
{
    double r;
    double i;
};

void compute_serial(double *area, double *error)
{
    int numinside = 0, numoutside = 0;
    for (int i = 0; i < NPOINTS; i++)
        for (int j = 0; j < NPOINTS; j++)
        {
            
            struct complex c;
            c.r = -2.0 + 2.5 * (double)(i) / (double)(NPOINTS);
            c.i = 1.125 * (double)(j) / (double)(NPOINTS);
            struct complex z;
            z = c; 
            for (int iter = 0; iter < MAXITER; iter++)
            {
                double temp = (z.r * z.r) - (z.i * z.i) + c.r;
                z.i = z.r * z.i * 2 + c.i;
                z.r = temp;
                if ((z.r * z.r + z.i * z.i) > 4.0)
                { 
                    numoutside++;
                    break;
                }
            }
        }
    numinside = NPOINTS * NPOINTS - numoutside;
    *area = 2.0 * 2.5 * 1.125 * (double)(numinside) / (double)(NPOINTS * NPOINTS);
    *error = *area / (double)NPOINTS;
}



void compute_parallel_v1(double *area, double *error,int thread_count)
{
    int numinside = 0, numoutside = 0;
    #pragma omp parallel for num_threads(thread_count) schedule(static,1) reduction(+ : numoutside)
    for (int i = 0; i < NPOINTS; i++)
        for (int j = 0; j < NPOINTS; j++)
        {
        
            struct complex c;
            c.r = -2.0 + 2.5 * (double)(i) / (double)(NPOINTS);
            c.i = 1.125 * (double)(j) / (double)(NPOINTS);
            struct complex z;
            z = c; 
            for (int iter = 0; iter < MAXITER; iter++)
            {
                double temp = (z.r * z.r) - (z.i * z.i) + c.r;
                z.i = z.r * z.i * 2 + c.i;
                z.r = temp;
                if ((z.r * z.r + z.i * z.i) > 4.0)
                { 
                    numoutside++;
                    break;
                }
            }
        }
    numinside = NPOINTS * NPOINTS - numoutside;
    *area = 2.0 * 2.5 * 1.125 * (double)(numinside) / (double)(NPOINTS * NPOINTS);
    *error = *area / (double)NPOINTS;
}



void compute_parallel_v2(double *area, double *error,int thread_count)
{
    int numinside = 0, numoutside = 0;
    #pragma omp parallel for num_threads(thread_count) schedule(dynamic,1) reduction(+ : numoutside)
    for (int i = 0; i < NPOINTS; i++)
        for (int j = 0; j < NPOINTS; j++)
        {
          
            struct complex c;
            c.r = -2.0 + 2.5 * (double)(i) / (double)(NPOINTS);
            c.i = 1.125 * (double)(j) / (double)(NPOINTS);
            struct complex z;
            z = c; 
            for (int iter = 0; iter < MAXITER; iter++)
            {
                double temp = (z.r * z.r) - (z.i * z.i) + c.r;
                z.i = z.r * z.i * 2 + c.i;
                z.r = temp;
                if ((z.r * z.r + z.i * z.i) > 4.0)
                { 
                    numoutside++;
                    break;
                }
            }
        }
    numinside = NPOINTS * NPOINTS - numoutside;
    *area = 2.0 * 2.5 * 1.125 * (double)(numinside) / (double)(NPOINTS * NPOINTS);
    *error = *area / (double)NPOINTS;
}



void compute_parallel_v3(double *area, double *error,int thread_count)
{
    int numinside = 0, numoutside = 0;
    #pragma omp parallel for num_threads(thread_count) schedule(static,20) reduction(+ : numoutside)
    for (int i = 0; i < NPOINTS; i++)
        for (int j = 0; j < NPOINTS; j++)
        {
         
            struct complex c;
            c.r = -2.0 + 2.5 * (double)(i) / (double)(NPOINTS);
            c.i = 1.125 * (double)(j) / (double)(NPOINTS);
            struct complex z;
            z = c; 
            for (int iter = 0; iter < MAXITER; iter++)
            {
                double temp = (z.r * z.r) - (z.i * z.i) + c.r;
                z.i = z.r * z.i * 2 + c.i;
                z.r = temp;
                if ((z.r * z.r + z.i * z.i) > 4.0)
                { 
                    numoutside++;
                    break;
                }
            }
        }
    numinside = NPOINTS * NPOINTS - numoutside;
    *area = 2.0 * 2.5 * 1.125 * (double)(numinside) / (double)(NPOINTS * NPOINTS);
    *error = *area / (double)NPOINTS;
}



void compute_parallel_v4(double *area, double *error,int thread_count)
{
    int numinside = 0, numoutside = 0;
    #pragma omp parallel for num_threads(thread_count) schedule(dynamic,20) reduction(+ : numoutside)
    for (int i = 0; i < NPOINTS; i++)
        for (int j = 0; j < NPOINTS; j++)
        {
           
            struct complex c;
            c.r = -2.0 + 2.5 * (double)(i) / (double)(NPOINTS);
            c.i = 1.125 * (double)(j) / (double)(NPOINTS);
            struct complex z;
            z = c; 
            for (int iter = 0; iter < MAXITER; iter++)
            {
                double temp = (z.r * z.r) - (z.i * z.i) + c.r;
                z.i = z.r * z.i * 2 + c.i;
                z.r = temp;
                if ((z.r * z.r + z.i * z.i) > 4.0)
                {
                    numoutside++;
                    break;
                }
            }
        }
    numinside = NPOINTS * NPOINTS - numoutside;
    *area = 2.0 * 2.5 * 1.125 * (double)(numinside) / (double)(NPOINTS * NPOINTS);
    *error = *area / (double)NPOINTS;
}


int main()
{
    double area, error;
    double start, time;

    printf("Serial version:...");
    start = omp_get_wtime();
    compute_serial(&area, &error);
    time = omp_get_wtime() - start;
    printf("Serial :  area=%f  error=%f   time=%f \n", area, error, time);

    printf("Parallel version 1:...");
    start = omp_get_wtime();
    compute_parallel_v1(&area, &error,8);
    time = omp_get_wtime() - start;
    printf("Serial :  area=%f  error=%f   time=%f \n", area, error, time);

    printf("Parallel version 2:...");
    start = omp_get_wtime();
    compute_parallel_v2(&area, &error,8);
    time = omp_get_wtime() - start;
    printf("Serial :  area=%f  error=%f   time=%f \n", area, error, time);

    printf("Parallel version 3:...");
    start = omp_get_wtime();
    compute_parallel_v3(&area, &error,8);
    time = omp_get_wtime() - start;
    printf("Serial :  area=%f  error=%f   time=%f \n", area, error, time);

    printf("Parallel version 4:...");
    start = omp_get_wtime();
    compute_parallel_v4(&area, &error,8);
    time = omp_get_wtime() - start;
    printf("Serial :  area=%f  error=%f   time=%f \n", area, error, time);

}