#ifndef __CELLS_H_
#define __CELLS_H_

/*
 *  definition of the model 
 * 
 */
typedef struct CELL{
  LC_DERIVED;
  unsigned long n;
} cell;

extern cell * cells;

#endif 