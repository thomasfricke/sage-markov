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
#include <topology.h>


extern cell * cells;
extern lc_global lc_g;

extern lc_reactivity_t decay_rate, diffusion_rate;

inline lc_reactivity_t reactivity(size_t index) {
  return (decay_rate+diffusion_rate)* ( cells+index ) -> n;
}

inline double markov_step(){
    LC_DRAW(cell,source);
    double time_step=LC_TIME_STEP();
   
    if( drand55()* ( decay_rate + diffusion_rate ) < decay_rate ){
       // reaction step
      source -> n--;
      LC_UPDATE_DRAWN(source);
      return time_step;
    } else {
      // diffusion step
      source->n--;
      cell *dest = random_neighbour(source);
      dest->n++;
      LC_UPDATE_DRAWN(source);

      LC_UPDATE(dest);
      return time_step;
    }
}
#endif