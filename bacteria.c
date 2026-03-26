#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

char *grid;
char *new_grid;
char *local_grid;
char *local_new_grid;
char *groundtruth;
int N, M;
int G;
int my_rank, comm_sz;

void read_bacteria(FILE *f)
{
    if(fscanf(f,"%d %d",&N,&M)!=2)
    {
        fprintf(stderr,"Error reading\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    grid = (char *)malloc(N * M * sizeof(char));
    if (!grid)
    {
        fprintf(stderr,"Memory allocation error for grid\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    new_grid = (char *)malloc(N * M * sizeof(char));
    if (!new_grid)
    {
        fprintf(stderr,"Memory allocation error for new grid\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    for(int i=0;i<N;i++)
    {
        for(int j=0;j<M;j++)
        {
            if(fscanf(f," %c",&grid[i*M+j])!=1)
            {
                fprintf(stderr,"Error reading the grid\n");
                MPI_Abort(MPI_COMM_WORLD, 1);
            }
        }
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
    for(int time = 0; time < G; time++) 
    {
        for(int i = 0; i < N; i++) 
        {
            for(int j = 0; j < M; j++) 
            {
                int count = 0;
                
                if ((i > 0) && (j > 0) && (grid[(i-1)*M+(j-1)] == 'X')) count++;
                if ((i > 0) && (grid[(i-1)*M+(j)] == 'X')) count++;
                if ((i > 0) && (j < M-1) && (grid[(i-1)*M+(j+1)] == 'X')) count++;
                if ((j > 0) && (grid[(i)*M+(j-1)] == 'X')) count++;
                if ((j < M-1) && (grid[(i)*M+(j+1)] == 'X')) count++;
                if ((i < N-1) && (j > 0) && (grid[(i+1)*M+(j-1)] == 'X')) count++;
                if ((i < N-1) && (grid[(i+1)*M+(j)] == 'X')) count++;
                if ((i < N-1) && (j < M-1) && (grid[(i+1)*M+(j+1)] == 'X')) count++;
                
                if (grid[i*M+j] == 'X') 
                {
                    new_grid[i*M+j] = (count < 2 || count > 3) ? '.' : 'X';
                } else 
                {
                    new_grid[i*M+j] = (count == 3) ? 'X' : '.';
                }
            }
        }
        swap_ptr(&grid, &new_grid);
    }
}

void parallel_bacteria_1d(int debug_mode) 
{
    int local_n = N / comm_sz;
    int rest = N % comm_sz;
    
    int my_rows;
    if (my_rank < rest) 
    {
        my_rows = local_n + 1; 
    } 
    else 
    {
        my_rows = local_n;
    }
    
    // Alocăm cu ghost rows (index 0 și my_rows+1)
    local_grid = (char *)malloc((my_rows + 2) * M * sizeof(char));
    local_new_grid = (char *)malloc((my_rows + 2) * M * sizeof(char));
    
    if (!local_grid || !local_new_grid) 
    {
        fprintf(stderr, "Memory allocation error for local grids\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    int *sendelements = NULL;
    int *displacement_data_per_process = NULL;
    
    if (my_rank == 0) 
    {
        sendelements = (int *)malloc(comm_sz * sizeof(int));
        displacement_data_per_process = (int *)malloc(comm_sz * sizeof(int));
        
        for (int i = 0; i < comm_sz; i++) 
        {
            int rows = local_n + (i < rest ? 1 : 0);
            sendelements[i] = rows * M;
            displacement_data_per_process[i] = (i * local_n + (i < rest ? i : rest)) * M;
        }
    }
    
    // Distribuim datele (în local_grid începând de la index M, adică rândul 1)
    MPI_Scatterv(grid, sendelements, displacement_data_per_process, MPI_CHAR,
                 &local_grid[M], my_rows * M, MPI_CHAR, 0, MPI_COMM_WORLD);
  
    for (int time = 0; time < G; time++) 
    {
        MPI_Request requests[4];
        MPI_Status statuses[4];
        int request_count = 0;
        
        // Schimb cu procesul de sus (my_rank - 1)
        if (my_rank > 0) 
        {
            // Trimitem primul rând real către sus
            MPI_Isend(&local_grid[M], M, MPI_CHAR, my_rank - 1, 0, 
                     MPI_COMM_WORLD, &requests[request_count++]);
            // Primim ghost row de sus
            MPI_Irecv(&local_grid[0], M, MPI_CHAR, my_rank - 1, 1, 
                     MPI_COMM_WORLD, &requests[request_count++]);
        } 
        else 
        {
            // Procesul 0: ghost row de sus este gol (boundary condition)
            memset(&local_grid[0], '.', M);
        }
        
        // Schimb cu procesul de jos (my_rank + 1)
        if (my_rank < comm_sz - 1) 
        {
            // Trimitem ultimul rând real către jos
            MPI_Isend(&local_grid[my_rows * M], M, MPI_CHAR, my_rank + 1, 1, 
                     MPI_COMM_WORLD, &requests[request_count++]);
            // Primim ghost row de jos
            MPI_Irecv(&local_grid[(my_rows + 1) * M], M, MPI_CHAR, my_rank + 1, 0, 
                     MPI_COMM_WORLD, &requests[request_count++]);
        } 
        else 
        {
            // Ultimul proces: ghost row de jos este gol (boundary condition)
            memset(&local_grid[(my_rows + 1) * M], '.', M);
        }
        
        // Așteptăm finalizarea comunicațiilor
        MPI_Waitall(request_count, requests, statuses);

        // Calculăm noua generație pentru rândurile locale
        // Indexul în local_grid: 0 = ghost sus, 1..my_rows = date reale, my_rows+1 = ghost jos
        for(int i = 1; i <= my_rows; i++) 
        {
            for(int j = 0; j < M; j++) 
            {
                int count = 0;
                
                // Calculăm vecinii - trebuie să respectăm boundary-urile coloanelor
                // Rândul de deasupra (i-1)
                if (j > 0 && local_grid[(i-1)*M+(j-1)] == 'X') count++;
                if (local_grid[(i-1)*M+(j)] == 'X') count++;
                if (j < M-1 && local_grid[(i-1)*M+(j+1)] == 'X') count++;
                
                // Același rând (i)
                if (j > 0 && local_grid[i*M+(j-1)] == 'X') count++;
                // Nu contăm celula curentă
                if (j < M-1 && local_grid[i*M+(j+1)] == 'X') count++;
                
                // Rândul de dedesubt (i+1)
                if (j > 0 && local_grid[(i+1)*M+(j-1)] == 'X') count++;
                if (local_grid[(i+1)*M+(j)] == 'X') count++;
                if (j < M-1 && local_grid[(i+1)*M+(j+1)] == 'X') count++;
                
                // Aplicăm regulile Game of Life
                if (local_grid[i*M+j] == 'X') 
                {
                    // Celulă vie: supraviețuiește dacă are 2-3 vecini
                    local_new_grid[i*M+j] = (count < 2 || count > 3) ? '.' : 'X';
                } 
                else 
                {
                    // Celulă moartă: devine vie dacă are exact 3 vecini
                    local_new_grid[i*M+j] = (count == 3) ? 'X' : '.';
                }
            }
        }
        
        // Swap grid-urile pentru următoarea iterație
        swap_ptr(&local_grid, &local_new_grid);
    }
    
    // Adunăm rezultatele finale
    MPI_Gatherv(&local_grid[M], my_rows * M, MPI_CHAR,
                grid, sendelements, displacement_data_per_process, MPI_CHAR,
                0, MPI_COMM_WORLD);
    
    if (my_rank == 0) 
    {
        free(sendelements);
        free(displacement_data_per_process);
    }
    
    free(local_grid);
    free(local_new_grid);
}

void save_groundtruth() 
{
    groundtruth = (char *)malloc(N * M * sizeof(char));
    if (!groundtruth) 
    {
        fprintf(stderr, "Memory allocation error for groundtruth\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    
    for(int i = 0; i < N*M; i++) 
    {
        groundtruth[i] = grid[i];
    }
}

void write_bacteria_file(const char *filepath) 
{
    FILE *g = fopen(filepath, "w");
    if(g == NULL) 
    {
        fprintf(stderr, "Error opening output file\n");
        return;
    }
    
    for(int i = 0; i < N; i++) 
    {
        for(int j = 0; j < M; j++) 
        {
            fprintf(g, "%c", grid[i*M+j]);
        }
        fprintf(g, "\n");
    }
    
    fclose(g);
}

int equal_groundtruth() 
{
    for(int i = 0; i < N*M; i++) 
    {
        if(groundtruth[i] != grid[i]) 
        {
            return 0;
        }
    }
    return 1;
}

void Usage(char *prog_name) 
{
    fprintf(stderr, "usage: mpirun -n <num_procs> %s <input_file> <generations>\n", prog_name);
}

int main(int argc, char **argv) 
{
    double start_serial, end_serial, elapsed_serial;
    double start_parallel, end_parallel, elapsed_parallel;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    
    if(argc != 3) 
    {
        if (my_rank == 0) 
        {
             Usage(argv[0]);
        }
        MPI_Finalize();
        return 0;
    }
    
    G = atoi(argv[2]);
    
    if (my_rank == 0) 
    {
        FILE *f = fopen(argv[1], "r");
        if(f == NULL)
         {
            fprintf(stderr, "Error opening input file\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        
        read_bacteria(f);
        fclose(f);
        
        // SALVEAZĂ GRILA INIȚIALĂ
        char *initial_grid = (char *)malloc(N * M * sizeof(char));
        if (!initial_grid)
        {
            fprintf(stderr,"Memory allocation error for initial grid\n");
            MPI_Abort(MPI_COMM_WORLD, 1);
        }
        for(int i = 0; i < N*M; i++) 
        {
            initial_grid[i] = grid[i];
        }
        
        printf("\n=== SERIAL VERSION ===\n");
        start_serial = MPI_Wtime();
        serial_bacteria();
        end_serial = MPI_Wtime();
        elapsed_serial = end_serial - start_serial;
        printf("Serial time: %lf seconds\n", elapsed_serial);
        
        save_groundtruth();
        
        char output_filepath[256];
        strcpy(output_filepath, argv[1]);
        char *p = strchr(output_filepath, '.');
        if(p) *p = '\0';
        strcat(output_filepath, "_serial_out.txt");
        write_bacteria_file(output_filepath);
        printf("Serial output: %s\n\n", output_filepath);
        
        // RESTAUREAZĂ GRILA INIȚIALĂ pentru versiunea paralelă
        for(int i = 0; i < N*M; i++) 
        {
            grid[i] = initial_grid[i];
        }
        free(initial_grid);
    }
    
    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&M, 1, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Bcast(&G, 1, MPI_INT, 0, MPI_COMM_WORLD);
    
    if (my_rank != 0) 
    {
        grid = (char *)malloc(N * M * sizeof(char));
        new_grid = (char *)malloc(N * M * sizeof(char));
    }
    
    if (my_rank == 0) 
    {
        printf("=== PARALLEL VERSION (1D Decomposition) ===\n");
        printf("Running with %d processes...\n", comm_sz);
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    start_parallel = MPI_Wtime();
    
    parallel_bacteria_1d(0);
    
    MPI_Barrier(MPI_COMM_WORLD);
    end_parallel = MPI_Wtime();
    elapsed_parallel = end_parallel - start_parallel;
    
    if (my_rank == 0) 
    {
        printf("Parallel time: %lf seconds\n", elapsed_parallel);
        
        if (!equal_groundtruth()) 
        {
            printf("!!! ERROR: Parallel version produces different result!\n");
        } else 
        {
            printf("SUCCESS: Parallel version produces same result\n");
        }
        
        char output_filepath[256];
        strcpy(output_filepath, argv[1]);
        char *p = strchr(output_filepath, '.');
        if(p) *p = '\0';
        strcat(output_filepath, "_parallel_1d_out.txt");
        write_bacteria_file(output_filepath);
        printf("Parallel output: %s\n\n", output_filepath);
        
        printf("=== PERFORMANCE ===\n");
        printf("Speedup: %.4f\n", elapsed_serial / elapsed_parallel);
        printf("Efficiency: %.4f\n", (elapsed_serial / elapsed_parallel) / comm_sz);
        
        free(groundtruth);
    }
    
    free(grid);
    free(new_grid);
    
    MPI_Finalize();
    return 0;
}