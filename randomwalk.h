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

extern cell * cells;
extern lc_global lc_g;

inline double markov_step(){
    LC_DRAW(cell,source);
    double time_step=LC_TIME_STEP();

    lc_reactivity_t reaction  = reaction_reactivity(source);
    lc_reactivity_t diffusion = diffusion_reactivity(source);

    if( drand55()* ( reaction + diffusion ) < reaction ){
       // reaction step
      reaction_step(source);

      LC_UPDATE_DRAWN(source);

    } else {
      // diffusion step
      cell * dest = diffusion_step(source);

      LC_UPDATE_DRAWN(source);

      LC_UPDATE(dest);

    }
    return time_step;
}
#endif