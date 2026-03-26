# Bacteria-Colony-Simulation
Problem description

Simulate the evolution of a  colony of bacteria that is grown on a culture surface of rectangular shape. The culture surface is a 2D grid and bacteria can grow in grid points.

Initially, bacteria are seeded in certain points of the grid. At each point of the grid there can be at most one bacterium. The bacteria colony evolves in time: bacteria can  multiply or can die.

Bacteria multiply  if there is a group of 3 bacteria around an empty grid point, a new bacterium will spawn in that point.

A bacterium dies if it is isolated (it has less than two bacteria around) or if it is suffocated (it has more than 3 bacteria around).

Multiplication and deaths of bacteria happen synchronously, and time passes discretely in successive generations. Every generation is a result of the preceding one, and the whole computation depends deterministically on the initial configuration.

Given the configuration of the initial bacterial seeding,  determine  the final configuration of the bacterial colony after a number of G generations.

Initial configurations are loaded from files. The first line contains the number of rows and columns. Starting with the second line, every character represents a grid point and is an “X” (bacterium) or  “.” (empty).

