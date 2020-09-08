/*******************************************************************************
*    This file is part of Sage-Markov.
*
*    Sage-Markov is free software: you can redistribute it and/or modify
*    it under the terms of the GNU AFFERO GENERAL PUBLIC LICENSE as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    Sage-Markov is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU AFFERO GENERAL PUBLIC LICENSE for more details.

*    You should have received a copy of the GNU AFFERO GENERAL PUBLIC LICENSE
*    along with Sage-Markov.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef __MODEL_H__
#define __MODEL_H__
#include <logclass.h>
#include <diffusion/cells.h>
#include <topology.h>


extern lc_reactivity_t decay_rate, diffusion_rate;

inline  lc_reactivity_t reaction_reactivity(const cell* source){
    return decay_rate * source-> n ;
}

inline lc_reactivity_t diffusion_reactivity(const cell* source){
    return diffusion_rate * source->n;
}



inline lc_reactivity_t reactivity(size_t index) {
  return (decay_rate+diffusion_rate)* ( cells+index ) -> n;
}



inline void reaction_step(cell * source){
    source -> n--;
}

inline cell * diffusion_step(cell * source){
    source->n--;

    cell *dest = random_neighbour(source);
    dest->n++;

    return dest;
   }

#include <sagemarkov.h>

#endif