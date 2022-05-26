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

lc_reactivity_t decay_rate, diffusion_rate;

lc_reactivity_t reaction_reactivity(const cell* source);


lc_reactivity_t diffusion_reactivity(const cell* source);


lc_reactivity_t reactivity(size_t index);

lc_reactivity_t LC_REACTIVITY( const cell* source );

void reaction_step(cell * source);

cell * diffusion_step(cell * source);

#endif
