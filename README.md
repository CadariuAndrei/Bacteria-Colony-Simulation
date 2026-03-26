# Bacteria-Colony-Simulation 🦠

BACTERIA COLONY SIMUlation | C & PTHREADS

A high-performance 2D cellular automaton that simulates bacteria evolution. This project compares Serial vs. Parallel execution to analyze speedup and scalability in multi-threaded environments.


OVERVIEW

The simulation evolves a grid of bacteria based on deterministic survival and multiplication rules (similar to Conway's Game of Life). The core focus of this project is efficient workload distribution using POSIX Threads (pthreads).


EVOLUTION RULES

-Birth: An empty cell spawns a bacterium if it has exactly 3 neighbors.

-Survival: A bacterium lives if it has 2 or 3 neighbors.

-Death: Occurs due to isolation (<2 neighbors) or suffocation (>3 neighbors).


TECHNICAL FEATURES

-Parallelization: Workload is partitioned among N threads to process grid rows concurrently.

-Synchronization: Implements barrier synchronization to ensure consistent generations.

-Automated Validation: Built-in verification tool that checks if the parallel output matches the serial "ground truth.

-Benchmarking: High-resolution timing to calculate Execution Time and Speedup.

-Modes: -DEBUG: Real-time visual evolution of the grid.
        -NORMAL: Optimized for performance measurements.


In our case for the executable b.exe:

COMPILATION : use  gcc

in our case :    gcc -Wall -o b bacteria.c

EXECUTION 

./b <input_file> <generations> <thread_count>








