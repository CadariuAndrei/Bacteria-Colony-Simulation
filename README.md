# Bacteria-Colony-Simulation 🦠

Bacteria Colony Simulation | C & Pthreads

A high-performance 2D cellular automaton that simulates bacteria evolution. This project compares Serial vs. Parallel execution to analyze speedup and scalability in multi-threaded environments.

Overview

The simulation evolves a grid of bacteria based on deterministic survival and multiplication rules (similar to Conway's Game of Life). The core focus of this project is efficient workload distribution using POSIX Threads (pthreads).

Evolution Rules

-Birth: An empty cell spawns a bacterium if it has exactly 3 neighbors.
-Survival: A bacterium lives if it has 2 or 3 neighbors.
-Death: Occurs due to isolation (<2 neighbors) or suffocation (>3 neighbors).
