#ifndef __TOPOLOGY_H__
#define __TOPOLOGY_H__

extern size_t number_of_cells;
extern cell * cells;

inline cell * random_neighbour(cell * source) {
/* draw dest cell, right or left neighbour */
      cell * dest;
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
      return dest;
}
#endif 