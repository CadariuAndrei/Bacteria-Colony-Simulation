/**
 * We have an array with NELEM random values.
 * Compute a histogram with 16 categories where you count the array elements
 * according to their divisibility by 16.
 * Make this computation faster by parallelizing the counting.
 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#define NELEM 100000000

void generate_array(int *a, int n)
{
    srand(time(NULL));

    for (int i = 0; i < n; i++)
        a[i] = rand();
}

void count_histo16_serial(int *a, int n)
{
    
    long hist[16];
    for (int i = 0; i < 16; ++i)
        hist[i] = 0;

    double start, end;
    start = omp_get_wtime();

   
    for (long i = 0; i < NELEM; ++i)
    {
        hist[a[i] % 16]++;
    }

    end = omp_get_wtime() - start;

    printf("Serial Time for building histogram16 is %f\n", end);

    for (int i = 0; i < 16; i++)
    {
        printf("%d ", hist[i]);
    }
    printf("\n");
}

void count_histo16_parallel(int *a, int n)
{
   
    long hist[16];
    for (int i = 0; i < 16; ++i)
        hist[i] = 0;

    double start, end;
    start = omp_get_wtime();


    #pragma omp parallel for reduction(+ : hist[0:16])
    for (long i = 0; i < NELEM; ++i)
    {
        hist[a[i] % 16]++;
    }

    end = omp_get_wtime() - start;

    printf("Parallel Time for building histogram16 is %f\n", end);

    for (int i = 0; i < 16; i++)
    {
        printf("%d ", hist[i]);
    }
    printf("\n");
}

int main()
{

    int *a = (int *)malloc(NELEM * sizeof(int));
    if (a == NULL)
    {
        printf("Memory allocation failed!\n");
        return 1;
    }

    generate_array(a, NELEM);

    count_histo16_serial(a, NELEM);

    count_histo16_parallel(a, NELEM);

    return 0;
}