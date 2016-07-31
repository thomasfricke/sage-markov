#ifndef __TOPOLOGY_H__
#define __TOPOLOGY_H__
#include <rand55.h>

extern size_t number_of_cells,sizes[];
extern cell * cells;
extern int topological_dimension;
extern size_t *topological_sizes;


inline size_t dimension(){
    return topological_dimension;
}

inline cell * random_neighbour(cell * source) {
/* draw dest cell, right or left neighbour */
      cell * dest;
      const size_t neighbour=rand55() % (2*topological_dimension);
      if(neighbour & 1){ /* bad example */
        dest=source+topological_sizes[neighbour/2];
        if(dest>=cells+number_of_cells) /* cyclic boundaries: right out, left in */
          dest-=number_of_cells;
      }
      else{
        dest=source-topological_sizes[neighbour/2];
        if(dest<cells) /* cyclic boundaries: left out, right in */
          dest+=number_of_cells;
      }
      return dest;
}

#endif 