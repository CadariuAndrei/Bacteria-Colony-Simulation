#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>


int is_prime(int num)
{
    if (num <= 1)
        return 0; 
    for (int i = 2; i * i <= num; i++)
    {
        if (num % i == 0)
            return 0; 
    }
    return 1; 
}

void generate_array(int a[], int n)
{
   
    srand(time(NULL));
    for (int i = 0; i < n; i++)
    {
        a[i] = rand();
    }
}

void print_array(int a[], int n)
{
    for (int i = 0; i < n; i++)
        printf("%d ", a[i]);
}


int count_prime_serial(int a[], int n)
{
    int count = 0;
    for (int i = 0; i < n; i++)
    {
        if (is_prime(a[i]))
            count++;
    }
    return count;
}

int count_prime_parallel_v1(int a[],int n,int thread_count)
{
    int global_count=0;

    #pragma omp parallel num_threads(thread_count)
    {
        int i,local_n;
        int local_count=0;
        int rest;
        int my_rank=omp_get_thread_num();

        local_n=n/thread_count;
        
        if(my_rank==thread_count-1)
        {
            rest=n%thread_count;
        }
        else
        {
            rest=0;
        }
        
        int index_start=my_rank*local_n;
        int index_final=index_start+local_n+rest;
        

        for(i=index_start;i<index_final;i++)
        {
            if(is_prime(a[i]))
            {
                local_count++;
            }
        }

        #pragma omp critical 
        {
            global_count+=local_count;
        }

    }

    return global_count;
}


int count_prime_parallel_v2(int a[],int n,int thread_count)
{
    int count=0;

    #pragma omp parallel num_threads(thread_count) reduction(+ : count)
    {
        int i,local_n;
        int rest;
        int my_rank=omp_get_thread_num();

        local_n=n/thread_count;
        
        if(my_rank==thread_count-1)
        {
            rest=n%thread_count;
        }
        else
        {
            rest=0;
        }
        
        int index_start=my_rank*local_n;
        int index_final=index_start+local_n+rest;
        

        for(i=index_start;i<index_final;i++)
        {
            if(is_prime(a[i]))
            {
                count++;
            }
        }

    }

    return count;
}


int count_prime_parallel_v3(int a[],int n,int thread_count)
{
    int count=0;
    int i;
    #pragma omp parallel for num_threads(thread_count) reduction (+ : count)
        for( i=0;i<n;i++)
        {
            if(is_prime(a[i]))
            {
                count++;
            }
        }
        return count;
}



int main()
{
    int n, count = 0;

    printf("Enter the size of the array: ");
    scanf("%d", &n);

    int *arr = (int *)malloc(n * sizeof(int));
    if (arr == NULL)
    {
        printf("Memory allocation failed!\n");
        return 1;
    }

    generate_array(arr, n);

    double start, time;
    start = omp_get_wtime();
    count = count_prime_serial(arr, n);
    time = omp_get_wtime() - start;
    printf("\nTotal prime numbers in the array: %d\n", count);
    printf("Serial time =%f\n", time);

    start = omp_get_wtime();
    count = count_prime_parallel_v1(arr, n,8);
    time = omp_get_wtime() - start;
    printf("\nTotal prime numbers in the array: %d\n", count);
    printf("Parallel V1 time =%f\n", time);

    start = omp_get_wtime();
    count = count_prime_parallel_v2(arr, n,8);
    time = omp_get_wtime() - start;
    printf("\nTotal prime numbers in the array: %d\n", count);
    printf("Parallel V2 time =%f\n", time);

    start = omp_get_wtime();
    count = count_prime_parallel_v3(arr, n,8);
    time = omp_get_wtime() - start;
    printf("\nTotal prime numbers in the array: %d\n", count);
    printf("Parallel V3 time =%f\n", time);

    free(arr);

    return 0;
}