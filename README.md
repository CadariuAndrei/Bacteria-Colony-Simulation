# Bacteria-Colony-Simulation
PROBLEM DESCRIPTION

Simulate the evolution of a  colony of bacteria that is grown on a culture surface of rectangular shape. The culture surface is a 2D grid and bacteria can grow in grid points.

Initially, bacteria are seeded in certain points of the grid. At each point of the grid there can be at most one bacterium. The bacteria colony evolves in time: bacteria can  multiply or can die.

Bacteria multiply  if there is a group of 3 bacteria around an empty grid point, a new bacterium will spawn in that point.

A bacterium dies if it is isolated (it has less than two bacteria around) or if it is suffocated (it has more than 3 bacteria around).

Multiplication and deaths of bacteria happen synchronously, and time passes discretely in successive generations. Every generation is a result of the preceding one, and the whole computation depends deterministically on the initial configuration.

Given the configuration of the initial bacterial seeding,  determine  the final configuration of the bacterial colony after a number of G generations.

Initial configurations are loaded from files. The first line contains the number of rows and columns. Starting with the second line, every character represents a grid point and is an “X” (bacterium) or  “.” (empty).



REQUIREMENTS

Implement a program that determines the final configuration both in serial and parallel .

The name of the input file, the number of generations and the number of threads for the parallel version are given as command line arguments.

The parallel version will use pthreads. 

Implement an automatic verification method to compare that the serial and parallel versions produce the same result.  

The program must provide 2 modes of running: the interactive (DEBUG) mode, when the evolution of the grid is printed on screen after each generation, and the normal mode (without printing) for performance measurements.

The final configuration will be saved in files. If input file was  f.txt, then output files are following the name convention f_serial_out.txt and f_parallel_out.txt

Only the final result is saved – intermediate configurations for all generations are not saved in files.

Measure serial and parallel runtime and compute the speedup.  The measured runtime does NOT include reading initial configuration from file and writing the final configuration in a file.

Repeat measurements for different sizes of the  input grid and different number of parallel threads and draw the graph of speedup as a function of number of threads for different sized of the input grid. Provide a meaningful discussion of your experimental results.
