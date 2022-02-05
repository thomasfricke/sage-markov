#ifndef __TOPOLOGY_H__
#define __TOPOLOGY_H__

size_t number_of_cells, sizes[];
cell * cells;
int topological_dimension;
size_t *topological_sizes;

int dimension();

size_t topological_volume();

cell * random_neighbour(cell * source);

#endif 