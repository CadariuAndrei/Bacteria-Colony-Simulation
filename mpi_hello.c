/* File:       
 *    mpi_hello.c

Compile with:
gcc -g mpi_hello.c -I "c:\Program Files (x86)\Microsoft SDKs\MPI\Include" -L "c:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" -lmsmpi -o mpi_hello.exe
 
*/
#include <stdio.h>
#include <string.h>  /* For strlen             */
#include "mpi.h"    /* For MPI functions, etc */ 

#define MAX_STRING   100

int main(void) {
   char       greeting[MAX_STRING];  /* String storing message */
   int        comm_sz;               /* Number of processes    */
   int        my_rank;               /* My process rank        */

   int v[2]={10,11};
   int suma;

   /* Start up MPI */
   MPI_Init(NULL, NULL); 

   /* Get the number of processes */
   MPI_Comm_size(MPI_COMM_WORLD, &comm_sz); 

   /* Get my rank among all the processes */
   MPI_Comm_rank(MPI_COMM_WORLD, &my_rank); 

   if(my_rank==0)
   {
    printf("Procesul 0 tirmite valorile %d si %d catre procesul 1\n",v[0],v[1]);
    MPI_Send(v,2,MPI_INT,1,0,MPI_COMM_WORLD);

    MPI_Recv(&suma,1,MPI_INT,1,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    printf("Procesul 0 a primit suma %d\n",suma);
   }

   else if(my_rank==1)
   {
    MPI_Recv(v,2,MPI_INT,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
    printf("Procesul 1 a primit valorile %d si %d de la procesul 0\n",v[0],v[1]);

    suma=v[0]+v[1];

    printf("Procesul 1 a facut suma %d si a trimis-o la procesul 0\n",suma);
    MPI_Send(&suma,1,MPI_INT,0,0,MPI_COMM_WORLD);
   }



   /* Shut down MPI */
   MPI_Finalize(); 

   return 0;
}  /* main */