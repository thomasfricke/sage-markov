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

#ifndef __RANDOMWALK_H___
#define __RANDOMWALK_H___

#include <logclass.h>
#include <model.h>

/*
 *  definition of the model 
 * 
 */
typedef struct CELL{
  LC_DERIVED;
  unsigned long n;
} cell;

extern cell * cells;
extern size_t number_of_cells;
extern lc_global lc_g;

inline lc_reactivity_t reactivity(size_t index) {
  return ( cells+index ) -> n;
}

inline double markov_step(){
    LC_DRAW(cell,source);
    double time_step=LC_TIME_STEP();
    source->n--;
    cell *dest;
    /* draw dest cell, right or left neighbour */
    if(rand55() & 1){ /* bad example */
      dest=source+1;
      if(dest==cells+number_of_cells) /* cyclic boundaries: right out, left in */
        dest=cells;
    }
    else{
      dest=source-1;
      if(dest<cells) /* cyclic boundaries: left out, right in */
        dest+=number_of_cells;
    }

    dest->n++;
    LC_UPDATE_DRAWN(source);

    LC_UPDATE(dest);
    return time_step;
}
#endif