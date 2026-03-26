/* File:     mpi_trapez.c
 * Purpose:  Use MPI to implement a parallel version of the trapezoidal 
 *           rule.  This version accepts as inputs the endpoints of the
 *           interval and the number of trapezoids.
 *
 *  Adapts example from IPP2:  Section 3.3.2 
 *  Communication between processes is don with blocking send-receive
 * 
 *  Compile with: MSMPi anf Mingw in lab:  
 *  gcc -g mpi_trapez.c -I "c:\Program Files (x86)\Microsoft SDKs\MPI\Include" -L "c:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" -lmsmpi -o mpi_trapez.exe
 */


#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/* Get the input values */
void Get_input(int my_rank, int comm_sz, double* a, double* b, int* n);

/* Calculate local integral  */
double integral(double a, double b, int n);    

/* Function we're integrating */
double f(double x); 

double start, stop; /* Measuring time */

int main(void) {
   /* Every process will have its own copy of these variables */
   int my_rank, comm_sz;
   int  n, local_n;   
   double a, b, h, local_a, local_b;
   double local_result, total_result;

   MPI_Init(NULL, NULL);

   /* Get my process rank */
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

   /* Find out how many processes are being used */
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

   /* Every process will get input values for a, b, n */
   Get_input(my_rank, comm_sz, &a, &b, &n);

   if (n % comm_sz !=0) {
       if (my_rank==0) {
		printf("Number of trapezes n must be divisible by number of processes\n");
		fflush(stdout);
		}
        MPI_Finalize();
	exit(1);
   } 

   h = (b-a)/n;          /* h is the same for all processes */
   
   /* Every process will handle local_n trapezes */
   local_n = n/comm_sz;  

   /* Determine local start point a and local end point b for current process */
   local_a = a + my_rank*local_n*h;
   local_b = local_a + local_n*h;

   /* Compute a local result for the interval local_a to local_b */
   local_result = integral(local_a, local_b, local_n);

   /* Add up the integrals calculated by each process */
   /* Process 0 receives and sums up local results from all other processes*/
   MPI_Reduce(&local_result,&total_result,1,MPI_DOUBLE,MPI_SUM,0,MPI_COMM_WORLD);
   /* Print the result */
   if (my_rank == 0) {
      stop=MPI_Wtime(); /* end measuring time */
      printf("time =%f\n", stop-start);
      printf("With n = %d trapezoids, our estimate\n", n);
      printf("of the integral from %f to %f = %.15e\n",
          a, b, total_result);
   }

   /* Shut down MPI */
   MPI_Finalize();

   return 0;
} /*  main  */

/*------------------------------------------------------------------
 * Function:     Get_input
 * Purpose:      Get the user input:  the left and right endpoints
 *               and the number of trapezoids. 
		           Only process 0 reads from stdin, then it sends data by
                 messages to all other processes 
 */
void Get_input(int my_rank, int comm_sz, double* a, double* b, int* n) {

   if (my_rank == 0) {
      printf("Enter a, b, and n\n");
      fflush(stdout);
      scanf("%lf %lf %d", a, b, n);
      start=MPI_Wtime();   /* start measuring time */
   }
     
     MPI_Bcast(a,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
     MPI_Bcast(b,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
     MPI_Bcast(n,1,MPI_INT,0,MPI_COMM_WORLD);
}/* Get_input */


/*------------------------------------------------------------------
 * Function:     integral
 * Purpose:      Serial function for estimating a definite integral 
 *               using the trapezoidal rule
 */
double integral(double a , double b, int  n) {
  double result;
  double h = (b - a) / n;
  int i;
  result = (f(a) + f(b)) / 2.0;
   for (i = 1; i <= n - 1; i++) {
	result += f(a + i * h);
	}
   result = result * h;
   return result;  
} /*  integral  */


/*------------------------------------------------------------------
 * Function:    f
 * Purpose:     Compute value of function to be integrated
 * Input args:  x
 */
double f(double x) {
   return x*x;
} /* f */