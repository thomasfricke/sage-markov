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

#include <sagemarkov.h>


lc_global lc_g;
cell * cells;
size_t number_of_cells;
double markov_time, timescale;

unsigned long seed;

int create_walk(size_t init_number_of_cells, unsigned long init_seed, double init_timescale){
  
  markov_time = 0;
  timescale = init_timescale;
  
  if(cells){
    return 1;
  }

  number_of_cells=init_number_of_cells;

  cells=(cell*) calloc(sizeof(cell), number_of_cells);
  seed=init_rand55(init_seed);
  lc_init(&lc_g,number_of_cells,NULL,timescale);

  for( size_t i=0; i<number_of_cells; i++){
    cells[i].lc_ev=NULL;
  }
 
  return 0;
}

int destroy_walk(){
  if(cells){
    lc_clear(&lc_g);
    free(cells);
    cells=NULL;
    number_of_cells=0;
    return 0;
  }
  return 1;
}

int run_walk(size_t nrun){
  if( lc_g.r < lc_g.eps ){
     printf("model not initialized, reactivity is 0 \n");
     return -1;
  }
  for(size_t i=0;  ( nrun==0 || i<nrun ) && lc_g.r > lc_g.eps ; i++){
    markov_time+=markov_step();
  }
  return 0;
}

size_t run_walk_until(double time){
  if( lc_g.r < lc_g.eps ){
     printf("model not initialized, reactivity is 0 \n");
     return -1;
  }
  size_t step=0;
  while (markov_time < time  && lc_g.r > lc_g.eps) {
    markov_time+=markov_step();
    step++;
  }
  return step;
}