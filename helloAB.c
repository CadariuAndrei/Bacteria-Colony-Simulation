/* File:  
 *    helloAB.c
 *
 * Purpose:
 *    Illustrate basic use of pthreads:  creates two threads,
 *    one repeatedly prints AAA, the other prints BBB, while 
 *    parent thread prints main
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h> 


/* Thread function A */
void *HelloA(void* dummy) {
   for (int i=0; i<200; i++)  
      printf("AAAA");

   return NULL;
}  



/* Thread function B */
void *HelloB(void* dummy) {
   for (int i=0; i<200; i++)  
      printf("BBBB");

   return NULL;
}  


void *HelloC(void *dummy)
{
   for(int i=0;i<200;i++)
   {
      printf("CCCCC");
   }
   return NULL;
}


int main(int argc, char* argv[]) {
  

   pthread_t thread_handleA, thread_handleB, thread_handleC;

   pthread_create(&thread_handleA, NULL, HelloA, NULL);  

   pthread_create(&thread_handleB, NULL, HelloB, NULL);  

   pthread_create(&thread_handleC, NULL, HelloC, NULL);

   
   for (int i=0; i<200; i++)
   	printf("main");

  
   pthread_join(thread_handleA, NULL); 

   pthread_join(thread_handleB, NULL); 

   pthread_join(thread_handleC, NULL);

   return 0;
}  
