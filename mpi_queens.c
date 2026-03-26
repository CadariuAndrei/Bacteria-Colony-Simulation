/*

Compile with:
gcc -g mpi_queens.c -I "c:\Program Files (x86)\Microsoft SDKs\MPI\Include" -L "c:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64" -lmsmpi -o mpi_queens.exe

*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#define WORK_TAG 1
#define FINAL_TAG 2
#define SOLUTION_TAG 3

// #define DEBUG

/* checks if a queen can be placed at row=depth and col */
int safe(int *pos, int depth, int col)
{
    for (int i = 0; i < depth; i++)
    {
        if (pos[i] == col ||
            abs(pos[i] - col) == abs(depth - i))
            return 0;
    }
    return 1;
}

/* Backtracking N-queens from a given partial configuration 
   Returns 1 if solution found, 0 otherwise.
   Stores first solution in pos array */
int solve_first(int *pos, int depth, int N)
{
    // queens are already placed in all rows until depth-1
    // pos[i] = the column of the queen from row i

    if (depth == N)
        return 1; // found a solution

    // for row=depth, explore all columns col where a queen can be placed
    for (int col = 0; col < N; col++)
    {
        if (safe(pos, depth, col))
        {
            pos[depth] = col;
            if (solve_first(pos, depth + 1, N))
                return 1; // found solution, propagate up
        }
    }
    return 0; // no solution found
}

/* Recursive generator for pos arrays after expanding k levels.
Executes the first k levels of the solution backtracking algo.
Fills out the array tasks, tasks[i] contains pos used as start.
The number of tasks will be in count. */
void gen_k_tasks_rec(int **tasks, int *count, 
                     int *pos, int depth, int k, int N)
{
    if (depth == k)
    {  
        int *task = malloc(k * sizeof(int));
        for (int i = 0; i < k; i++)
            task[i] = pos[i];
        tasks[(*count)++] = task; // add a new task to tasks array
        return;
    }
    for (int col = 0; col < N; col++)
    {
        if (safe(pos, depth, col))
        {
            pos[depth] = col;
            gen_k_tasks_rec(tasks, count,  pos, depth + 1, k, N);
        }
    }
}

/* Generates tasks from level k.
Returns an array of "tasks". Each "task" is a pos 
array containing the positions of the first k queens. */

int** generate_k_row_tasks( int N, int k, int *count)
{
    int capacity = 1;
    for (int i = 1; i <= k; i++)
        capacity = capacity * N; // capacity = N^k
    int **tasks = malloc(capacity * sizeof(int *)); // an array of pos arrays
    
    *count = 0; // actual number of nodes=tasks

    int *pos = calloc(N, sizeof(int)); // init pos vector

    gen_k_tasks_rec(tasks, count, pos, 0, k, N);

    free(pos);
    
    return tasks;
}

void print_solution(int *pos, int N)
{
    printf("First solution found:\n");
    for (int i = 0; i < N; i++)
    {
        printf("Row %d: Queen at column %d\n", i, pos[i]);
    }
    printf("\nBoard visualization:\n");
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
        {
            if (pos[i] == j)
                printf("Q ");
            else
                printf(". ");
        }
        printf("\n");
    }
}

int main(int argc, char *argv[])
{
    int rank, size;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (argc != 3)
    {
        if (rank == 0)
            printf("arguments: N k\n");
        MPI_Finalize();
        return 1;
    }

    int N = atoi(argv[1]);
    int k = atoi(argv[2]);
    if (k < 1 || k > N)
    {
        if (rank == 0)
            printf("k must be between 1 and N.\n");
        MPI_Finalize();
        return 1;
    }

    double start = MPI_Wtime();
    if (rank == 0)
    {
        /* ---------------- MASTER ---------------- */

        int **tasks;
        int num_tasks;
        tasks = generate_k_row_tasks( N, k, &num_tasks);
#ifdef DEBUG
        printf("generated %d tasks  \n", num_tasks);
        fflush(stdout);
#endif
        int next = 0;
        int solution_found = 0;
        int *solution = malloc(N * sizeof(int));

        int num_workers = size - 1;
        int initial_sent = 0;

        // Give each worker one initial task (up to num_tasks) 
        for (int w = 1; w <= num_workers && next < num_tasks; w++)
        {
            MPI_Send(tasks[next], k, MPI_INT, w, WORK_TAG, MPI_COMM_WORLD);
            next++;
            initial_sent++;
        }

        // If there are more workers than tasks, tell the extra workers to exit
        for (int w = 1 + initial_sent; w <= num_workers; w++)
        {
            int dummy = 0;
            MPI_Send(&dummy, 1, MPI_INT, w, FINAL_TAG, MPI_COMM_WORLD);
        }

        int active_workers = initial_sent;

        while (active_workers > 0 && !solution_found)
        {
            int result;
            int worker;

            MPI_Recv(&result, 1, MPI_INT,
                     MPI_ANY_SOURCE, MPI_ANY_TAG,
                     MPI_COMM_WORLD, &status);

            worker = status.MPI_SOURCE;

            if (result == 1) // solution found
            {
                // Receive the complete solution from worker
                MPI_Recv(solution, N, MPI_INT, worker, SOLUTION_TAG,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                solution_found = 1;
#ifdef DEBUG
                printf("Master: Solution received from worker %d\n", worker);
                fflush(stdout);
#endif
                // Send termination signal to all workers
                for (int w = 1; w <= num_workers; w++)
                {
                    int dummy = 0;
                    MPI_Send(&dummy, 1, MPI_INT, w, FINAL_TAG, MPI_COMM_WORLD);
                }
                break;
            }
            else
            {
                // No solution in this task, continue
                if (next < num_tasks)
                {
                    MPI_Send(tasks[next], k, MPI_INT, worker, WORK_TAG, MPI_COMM_WORLD);
                    next++;
                }
                else
                {
                    MPI_Send(&next, 1, MPI_INT, worker, FINAL_TAG, MPI_COMM_WORLD);
                    active_workers--;
                }
            }
        }

        double end = MPI_Wtime();
        
        if (solution_found)
        {
            printf("Time: %.6f seconds\n", end - start);
            printf("N = %d, k = %d\n\n", N, k);
            print_solution(solution, N);
        }
        else
        {
            printf("No solution found for %d-Queens.\n", N);
        }

        free(solution);
        
        /* free all tasks */
        for (int i = 0; i < num_tasks; i++)
            free(tasks[i]);
        free(tasks);
    }
    else
    {
        /* ---------------- WORKER ---------------- */
        while (1)
        {
            int *task = malloc(N * sizeof(int)); // only first k used in receive, but will need N to complete it to pos array later
            MPI_Recv(task, N, MPI_INT, 0, MPI_ANY_TAG,
                     MPI_COMM_WORLD, &status);

            if (status.MPI_TAG == FINAL_TAG)
            {
#ifdef DEBUG
                printf("worker %d exits \n", rank);
                fflush(stdout);
#endif
                free(task);
                break;
            }
#ifdef DEBUG
            printf("worker %d solves subproblem \n", rank);
            fflush(stdout);
#endif
            double task_start = MPI_Wtime();

            int result = solve_first(task, k, N);

            double task_end = MPI_Wtime();
            double task_time = task_end - task_start;
            
            if (result == 1)
            {
#ifdef DEBUG
                printf("worker %d FOUND SOLUTION in %lf seconds\n", rank, task_time);
                fflush(stdout);
#endif
                // Send result indicating solution found
                MPI_Send(&result, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                // Send the complete solution
                MPI_Send(task, N, MPI_INT, 0, SOLUTION_TAG, MPI_COMM_WORLD);
                free(task);
                
                // Wait for termination signal
                int dummy;
                MPI_Recv(&dummy, 1, MPI_INT, 0, FINAL_TAG,
                         MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                break;
            }
            else
            {
#ifdef DEBUG
                printf("worker %d finished a task (no solution) in %lf \n", rank, task_time);
                fflush(stdout);
#endif
                MPI_Send(&result, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
                free(task);
            }
        }
    }

    MPI_Finalize();
    return 0;
}