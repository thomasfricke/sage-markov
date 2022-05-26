#ifndef __MODEL_C__
#define __MODEL_C__
#include <diffusion/model.h>
#include <randomwalk.h>
#include <topology.h>

lc_reactivity_t decay_rate=0, diffusion_rate=1;

lc_reactivity_t reaction_reactivity(const cell* source){
    return decay_rate * source-> n ;
};

lc_reactivity_t diffusion_reactivity(const cell* source){
    return diffusion_rate * source->n;
};

lc_reactivity_t reactivity(size_t index) {
    return LC_REACTIVITY(cells+index);
}

lc_reactivity_t LC_REACTIVITY(const cell* source) {
    return (decay_rate+diffusion_rate) * source -> n;
}

void reaction_step(cell * source){
    source -> n--;
};

cell * diffusion_step(cell * source){
    source->n--;

    cell *dest = random_neighbour(source);
    dest->n++;

    return dest;
   };

#endif
