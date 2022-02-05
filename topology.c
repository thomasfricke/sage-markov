#include <stddef.h>
#include <stdarg.h>

int topological_dimension;
size_t *topological_sizes;

int dimension(){
    return topological_dimension;
};

size_t topological_volume(){
    return topological_sizes[topological_dimension];
};

cell * random_neighbour(cell * source) {
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
};


size_t create_topology(const int dimension, const size_t edge){

    topological_dimension=dimension;
    topological_sizes=calloc(topological_dimension+1,sizeof(size_t));

    size_t d_shift=1;
    for(size_t d=0; d<=topological_dimension; d++){
        topological_sizes[d]=d_shift;
        d_shift*=edge;
    }
    return d_shift;
}
