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

#ifndef __SAGEMARKOV_H___
#define __SAGEMARKOV_H___

#define MODEL "diffusion/model.h"

#include <logclass.h>
#include MODEL
#include <randomwalk.h>
#include <topology.h>

extern double markov_time, timescale;

unsigned long seed;

inline void update_reactivity(size_t index){
 ( cells+index) -> lc_ev = lc_enter( &lc_g, cells+index, reactivity( index ) );
}

inline lc_reactivity_t global_reactivity(){
   return lc_g.r;
}
#endif