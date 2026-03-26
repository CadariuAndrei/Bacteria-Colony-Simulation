/*
 * Measures speedup for matrix-vector multiplication
 * Uses and adapts code samples from [Pacheco]
 * For a given size m, n generates random matrix A and vector x
 * and measures runtime of serial and parallel algorithm
 * in order to compute speedup.
 * The parallel solution assumes that m is divisible by the number of threads!
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

/* If DEBUG defined, prints arrays*/
// #define DEBUG

/* maximum range for element values */
#define MAXRANGE 5000

/* Global variables */
int thread_count;
int n;
double *x;
double *y;
double *z;
double *z_serial;

/* Serial functions */
void Usage(char *prog_name);
void Generate_vector(char *prompt, double x[], int n);
void Print_vector(char *title, double y[], double n);
int Equal_vectors(double y[], double z[], double n);
void sum_two_squares_two_vectors(double x[], double y[], double z[], int n);

/* Parallel functions */
void *Pth_sum_two_squares_two_vectors(void *rank);
void sum_two_squares_two_vectors_parallel(double x[], double y[], double z[], int n);

/*------------------------------------------------------------------*/
int main(int argc, char *argv[])
{

   if (argc != 2)
      Usage(argv[0]);
   thread_count = atoi(argv[1]);

   printf("Enter n\n");
   scanf("%d", &n);

   if (n % thread_count != 0)
   {
      printf("n is not divisible by thread_count!\n");
      exit(1);
   }

  
   x = malloc(n * sizeof(double));
   y = malloc(n * sizeof(double));
   z = malloc(n * sizeof(double));
   z_serial = malloc(n * sizeof(double));


   Generate_vector("Generate the vector", x, n);
#ifdef DEBUG
   Print_vector("Vector is", x, n);
#endif
   Generate_vector("Generate the vector", y, n);
#ifdef DEBUG
   Print_vector("Vector is", y, n);
#endif

   struct timespec start, finish;
   double elapsed_serial, elapsed_parallel;
   printf("Serial: ");
   clock_gettime(CLOCK_MONOTONIC, &start); // measure wall clock time!

   sum_two_squares_two_vectors(x, y, z_serial, n);

   clock_gettime(CLOCK_MONOTONIC, &finish);

   elapsed_serial = (finish.tv_sec - start.tv_sec);
   elapsed_serial += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

   printf("time =%lf \n", elapsed_serial);
#ifdef DEBUG
   Print_vector("Serial Result", z_serial, n);
#endif

   printf("Parallel: ");

   clock_gettime(CLOCK_MONOTONIC, &start); // measure wall clock time!

   sum_two_squares_two_vectors_parallel(x,y,z,n); 

   clock_gettime(CLOCK_MONOTONIC, &finish);

   elapsed_parallel = (finish.tv_sec - start.tv_sec);
   elapsed_parallel += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;

   printf("time =%lf \n", elapsed_parallel);

#ifdef DEBUG
   Print_vector("Paralell Result", z, n);
#endif

   if (!Equal_vectors(z, z_serial, n))
      printf("Error! Serial and Parallel result vectors not equal! \n");

   printf("Number of parallel threads was %d\n", thread_count);
   printf("Measured Speedup=%f\n ", elapsed_serial / elapsed_parallel);


   free(x);
   free(y);
   free(z);
   free(z_serial);

   return 0;
} /* main */

/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   print a message showing what the command line should
 *            be, and terminate
 * In arg :   prog_name
 */
void Usage(char *prog_name)
{
   fprintf(stderr, "usage: %s <thread_count>\n", prog_name);
   exit(0);
} /* Usage */



/*------------------------------------------------------------------
 * Function:        Generate_vector
 * Purpose:         Generate random the vector x
 * In arg:          prompt, n
 * Out arg:         x
 */
void Generate_vector(char *prompt, double x[], int n)
{
   int i;

   printf("%s\n", prompt);
   for (i = 0; i < n; i++)
      // scanf("%lf", &x[i]);
      x[i] = rand() % MAXRANGE;
} /* Generate_vector */

int Equal_vectors(double y[], double z[], double n)
{
   int i;
   for (i = 0; i < n; i++)
      if (y[i] != z[i])
         return 0;
   return 1;
}

/*-------------------------------------------------------------------
 * Function:   sum_two_squares_two_vectors
 * Purpose:     compute the result in z array
 * In args:    x: the first computed vector
 *             y: the second computed vector
 *             n: the size of arrays
 * Out args:   z: the result vector
 */
void sum_two_squares_two_vectors(
    double x[] /* in  */,
    double y[] /* in  */,
    double z[] /* out */,
    int n /* in  */)
{
   int i;

   for (i = 0; i < n; i++)
   {
      z[i]=x[i]*x[i] + y[i]*y[i];
      
   }
} /* sum_two_squares_two_vectors */

/*------------------------------------------------------------------
 * Function:       Pth_sum_two_squares_two_vectors
 * Purpose:        function for sum_two_squares_two_vectors_parallel function
 * In arg:         rank
 * Global in vars:  x , y, n, thread_count
 * Global out var: z
 */
void *Pth_sum_two_squares_two_vectors(void *rank)
{
   int my_rank = *(int *)rank;
   int i;
   int local_n = n / thread_count;
   int my_first_index = my_rank * local_n;
   int my_last_index = (my_rank + 1) * local_n - 1;

   for (i = my_first_index; i <= my_last_index; i++)
   {
      z[i]=x[i]*x[i] + y[i]*y[i];
      
   }

   return NULL;
} /* Pth_sum_two_squares_two_vectors */

/*-------------------------------------------------------------------
 * Function:   sum_two_squares_two_vectors_parallel
 * Purpose:    sum of two squares of two vectors, in parallel
 * In args:    x: the first computed vector
 *             y: the second computed vector
 *             n: the size of arrays
 * Out args:   z: the result vector
 */
void sum_two_squares_two_vectors_parallel(
    double x[] /* in  */,
    double y[] /* in  */,
    double z[] /* out */,
    int n /* in  */)
{
   int thread;
   pthread_t *thread_handles;
   thread_handles = malloc(thread_count * sizeof(pthread_t));
   int *tid;
   tid = malloc(thread_count * sizeof(int));

   for (thread = 0; thread < thread_count; thread++)
   {
      tid[thread] = thread;
      pthread_create(&thread_handles[thread], NULL,
                     Pth_sum_two_squares_two_vectors, &tid[thread]);
   }

   for (thread = 0; thread < thread_count; thread++)
      pthread_join(thread_handles[thread], NULL);

   free(thread_handles);
   free(tid);
} /* sum_two_squares_two_vectors_parallel */



/*------------------------------------------------------------------
 * Function:    Print_vector
 * Purpose:     Print a vector
 * In args:     title, y, n
 */
void Print_vector(char *title, double y[], double n)
{
   int i;

   printf("%s\n", title);
   for (i = 0; i < n; i++)
      printf("%4.1f ", y[i]);
   printf("\n");
} /* Print_vector */