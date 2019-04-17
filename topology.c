#include <stddef.h>
#include <stdarg.h>

int topological_dimension;
size_t *topological_sizes;

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