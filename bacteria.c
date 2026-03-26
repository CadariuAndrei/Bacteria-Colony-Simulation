#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<time.h>

//#define DEBUG

char *grid;
char *grid_parallel;
char *new_grid;
char *new_grid_parallel;
int N,M;
char *groundtruth;
int G;
int thread_count;

pthread_barrier_t barrier;

void read_bacteria(FILE *f)
{
    if(fscanf(f,"%d %d",&N,&M)!=2)
    {
        perror("Error reading\n");
        exit(-1);
    }

    grid = (char *)malloc(N * M * sizeof(char));
    if (!grid)
    {
        printf("Memory allocation error for grid\n");
        exit(1);
    }
    new_grid = (char *)malloc(N * M * sizeof(char));
    if (!new_grid)
    {
        printf("Memory allocation error for new grid\n");
        exit(1);
    }


    for(int i=0;i<N;i++)
    {
        for(int j=0;j<M;j++)
        {
            if(fscanf(f," %c",&grid[i*M+j])!=1)
            {
                perror("Error reading the grid\n");
                exit(-1);
            }
        }
    }
}

void print_bacteria_grid(void)
{
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<M;j++)
        {
            printf("%c",grid[i*M+j]);
        }
        printf("\n");
    }
}

void print_bacteria_grid_parallel(void)
{
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<M;j++)
        {
            printf("%c",grid_parallel[i*M+j]);
        }
        printf("\n");
    }
}

int equal_groundtruth(void)
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
        {
            if (grid_parallel[i*M+j] != groundtruth[i*M+j])
                return 0;
        }
    return 1;
}

void save_groundtruth(void)
{
    groundtruth = (char *)malloc(N * M * sizeof(char));
    if (!groundtruth)
    {
        printf("Memory allocation error for groundtruth result\n");
        exit(1);
    }

    for (int i = 0; i < N; i++)
        for (int j = 0; j < M; j++)
        {
            groundtruth[i * M + j] = grid[i * M + j];
        }
}

void swap_ptr(char **p1, char **p2)
{
    char *tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
}

void serial_bacteria()
{
    int i, j, time;

    for (time = 0; time < G; time++)
    {
#ifdef DEBUG
        printf("\nGeneration %d \n", time);
        print_bacteria_grid();
#endif
        for (i = 0; i < N ; i++) 
            for (j = 0; j < M; j++)
            {
               int count=0;
               if( i>0 && j>0 && grid[(i-1)*M+(j-1)]=='X') 
               {
                    count++;
               }
               if(i>0 && grid[(i-1)*M+(j)]=='X')
               {
                    count++;
               }
               if(i>0 && j<M-1 && grid[(i-1)*M+(j+1)]=='X')
               {
                    count++;
               }
               if(j>0 && grid[(i)*M+(j-1)]=='X')
               {
                    count++;
               }
               if(j<M-1 && grid[(i)*M+(j+1)]=='X')
               {
                    count++;
               }
               if(i<N-1 && j>0 && grid[(i+1)*M+(j-1)]=='X')
               {
                    count++;
               }
               if(i<N-1 && grid[(i+1)*M+(j)]=='X')
               {
                    count++;
               }
               if(i<N-1 && j<M-1 && grid[(i+1)*M+(j+1)]=='X')
               {
                    count++;
               }



               if(grid[i*M+j]=='X')
               {
                    if(count<2 || count>3)
                    {
                        new_grid[i*M+j]='.';
                    }
                    else
                    {
                        new_grid[i*M+j]='X';
                    }
               }
               else
               {
                    if(count==3)
                    {
                        new_grid[i*M+j]='X';
                    }
                    else
                    {
                         new_grid[i*M+j]='.';
                    }
               }


            }

        swap_ptr(&grid, &new_grid);
    }


}

void copy_grid_for_parallel(void)
{
    grid_parallel = (char *)malloc(N * M * sizeof(char));
    if (!grid_parallel)
    {
        printf("Memory allocation error for grid\n");
        exit(1);
    }
    new_grid_parallel = (char *)malloc(N * M * sizeof(char));
    if (!new_grid_parallel)
    {
        printf("Memory allocation error for new grid\n");
        exit(1);
    }

    for(int i=0;i<N;i++)
    {
        for(int j=0;j<M;j++)
        {
            grid_parallel[i*M+j]=grid[i*M+j];
        }
    }
}

void *Pth_parallel_bacteria(void *rank)
{
   int i,j,time;
   int my_rank = *(int *)rank;
   int local_n = N / thread_count;
   int my_first_row = my_rank * local_n;
   int my_last_row = (my_rank + 1) * local_n - 1;

   if(my_rank==thread_count-1)
   {
     my_last_row=N-1;
   }

   for (time = 0; time < G; time++)
    {
        for (i = my_first_row; i <=my_last_row ; i++) 
        {
            for (j = 0; j < M; j++)
            {
               int count=0;
               if( i>0 && j>0 && grid_parallel[(i-1)*M+(j-1)]=='X') 
               {
                    count++;
               }
               if(i>0 && grid_parallel[(i-1)*M+(j)]=='X')
               {
                    count++;
               }
               if(i>0 && j<M-1 && grid_parallel[(i-1)*M+(j+1)]=='X')
               {
                    count++;
               }
               if(j>0 && grid_parallel[(i)*M+(j-1)]=='X')
               {
                    count++;
               }
               if(j<M-1 && grid_parallel[(i)*M+(j+1)]=='X')
               {
                    count++;
               }
               if(i<N-1 && j>0 && grid_parallel[(i+1)*M+(j-1)]=='X')
               {
                    count++;
               }
               if(i<N-1 && grid_parallel[(i+1)*M+(j)]=='X')
               {
                    count++;
               }
               if(i<N-1 && j<M-1 && grid_parallel[(i+1)*M+(j+1)]=='X')
               {
                    count++;
               }



               if(grid_parallel[i*M+j]=='X')
               {
                    if(count<2 || count>3)
                    {
                        new_grid_parallel[i*M+j]='.';
                    }
                    else
                    {
                        new_grid_parallel[i*M+j]='X';
                    }
               }
               else
               {
                    if(count==3)
                    {
                        new_grid_parallel[i*M+j]='X';
                    }
                    else
                    {
                         new_grid_parallel[i*M+j]='.';
                    }
               }

            }
        }
        pthread_barrier_wait(&barrier);
        if(my_rank==0)
        {
            swap_ptr(&grid_parallel, &new_grid_parallel);
        }
        pthread_barrier_wait(&barrier);
    }
   return NULL;
}

void parallel_bacteria()
{
   int thread;
   pthread_t *thread_handles;
   thread_handles = malloc(thread_count * sizeof(pthread_t));
   int *tid;
   tid = malloc(thread_count * sizeof(int));

   pthread_barrier_init(&barrier,NULL,thread_count);

   for (thread = 0; thread < thread_count; thread++)
   {
      tid[thread] = thread;
      pthread_create(&thread_handles[thread], NULL,
                     Pth_parallel_bacteria, &tid[thread]);
   }

   for (thread = 0; thread < thread_count; thread++)
      pthread_join(thread_handles[thread], NULL);

   pthread_barrier_destroy(&barrier);
   free(thread_handles);
   free(tid);
} 

void write_bacteria_file(const char *filepath)
{
    FILE *g=fopen(filepath,"w");
    if(g==NULL)
    {
        perror("Error opening the file\n");
        exit(-1);
    }

    for(int i=0;i<N;i++)
    {
        for(int j=0;j<M;j++)
        {
            fprintf(g,"%c",grid[i*M+j]);
        }
        fprintf(g,"\n");
    }

    if(fclose(g))
    {
        perror("Error closing the file\n");
        exit(-1);
    }
}

void write_bacteria_file_parallel(const char *filepath)
{
    FILE *g=fopen(filepath,"w");
    if(g==NULL)
    {
        perror("Error opening the file\n");
        exit(-1);
    }

    for(int i=0;i<N;i++)
    {
        for(int j=0;j<M;j++)
        {
            fprintf(g,"%c",grid_parallel[i*M+j]);
        }
        fprintf(g,"\n");
    }

    if(fclose(g))
    {
        perror("Error closing the file\n");
        exit(-1);
    }
}

void Usage(char *prog_name)
{
   fprintf(stderr, "Usage: %s <input_file> <generations> <thread_count>\n", prog_name);
   exit(0);
}

int main(int argc,char **argv)
{

    struct timespec start, finish;
    double elapsed_serial, elapsed_parallel;
    if(argc!=4)
    {
      Usage(argv[0]);
    }

    FILE *f=fopen(argv[1],"r");
    if(f==NULL)
    {
        perror("Error opening the file\n");
        exit(-1);
    }

    G=atoi(argv[2]);
    thread_count=atoi(argv[3]);



    read_bacteria(f);
    copy_grid_for_parallel();



    printf("\nRunning serial version ...\n");

    clock_gettime(CLOCK_MONOTONIC, &start);
    serial_bacteria();
    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed_serial = (finish.tv_sec - start.tv_sec);
    elapsed_serial += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("time =%lf \n", elapsed_serial);

    save_groundtruth();

    char input_filepath[256];
    char output_filepath[256];
    strcpy(input_filepath,argv[1]);
    char *p=strchr(input_filepath,'.');
    if(p)
    {
        *p='\0';
    }
    strcpy(output_filepath,input_filepath);
    strcat(output_filepath,"_serial_out.txt");
    write_bacteria_file(output_filepath);
    printf("Serial output:%s\n",output_filepath);
    printf("\n");


    printf("Running parallel version with %d threads...\n",thread_count);

    clock_gettime(CLOCK_MONOTONIC, &start);
    parallel_bacteria();
    clock_gettime(CLOCK_MONOTONIC, &finish);

    elapsed_parallel = (finish.tv_sec - start.tv_sec);
    elapsed_parallel += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("time =%lf \n", elapsed_parallel);


    if (!equal_groundtruth())
        printf("!!! Parallel version produces a different result! \n");
    else
        printf("Parallel version produced the same result \n");

    strcpy(output_filepath,input_filepath);
    strcat(output_filepath,"_parallel_out.txt");
    write_bacteria_file_parallel(output_filepath);
    printf("Parallel output:%s\n",output_filepath);
    printf("\n");


    printf("Speedup %lf \n",elapsed_serial/elapsed_parallel);
    if(fclose(f))
    {
        perror("Error closing the file\n");
        exit(-1);
    }

    free(grid);
    free(new_grid);
    free(grid_parallel);
    free(new_grid_parallel);
    free(groundtruth);
 
    return 0;
}
