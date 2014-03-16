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

#include <rand55.h>
#include <logclass.h>

typedef struct CELL{
  LC_DERIVED;
  unsigned long n;
} cell;

#define LC_REACTIVITY(c) ((c)->n)

int main(int argc, char **argv){


  unsigned long an=1000, an2=an/2;
  unsigned long wn=(argc>1?atoi(argv[1]):100000);
  unsigned long nrun=(argc>2?atoi(argv[2]):1000000);
  unsigned long k;
  double e1,e2; /* first and second moment */
  int ok;

  lc_reorg_t i;
  cell c[an];
  double t=0.0;
  LC_GLOBAL_DEF;

	init_rand55(999999999UL);

	LC_MAIN_BEGIN(16*an/15,1);

  printf("==========================================================================\n"
	 "testcase: %s\n",argv[0]);
  printf("lc_reactivity_t   : %s\n",lc_types[LC_REACTIVITY_TYPE]);
  printf("lc_reactivity_max : " LC_FORMAT "\n",lc_reactivity_max);
  printf("lc_reactivity_min : " LC_FORMAT "\n",lc_reactivity_min);
	printf("lc_num_classes    :  %d \n",lc_g.num_classes);
  printf("random walkers    : %lu \n",wn);
  printf("steps             : %lu \n",nrun);



  for(i=0; i<an; i++){
    c[i].n=0;
    c[i].lc_ev=NULL;
  }

  c[an2].n=wn;
  c[an2].lc_ev=lc_enter(&lc_g,c+an2,c[an2].n);

  for(i=0; nrun==0 || i<nrun; i++){

    LC_DRAW(cell,source);

    t+=LC_TIME_STEP();
    source->n--;
    cell *dest;
    /* draw dest cell, right or left neighbour */
    if(rand55() & 1){ /* bad example */
      dest=source+1;
      if(dest==c+an) /* cyclic boundaries: right out, left in */
	dest=c;
    }
    else{
      dest=source-1;
      if(dest<c) /* cyclic boundaries: left out, right in */
	dest+=an;
    }

    dest->n++;
    LC_UPDATE_DRAWN(source);

    LC_UPDATE(dest);
  }

  e1=e2=0;
  for(k=0; k<an; k++){
    e1+=c[k].n*k;
    e2+=c[k].n*pow(k,2);
  }
  e1/=wn;
  e2/=wn;
  ok=fabs(e1/an-0.5)<2*(e2-e1*e1)/(wn*t);
  printf("%lf %lf %lg %lg %s\n",e1/an,(e2-e1*e1)/t,
	 e1/an-0.5, (e2-e1*e1)/(wn*t),ok?"OK":"WRONG");

  LC_MAIN_END;
  return ok?0:1;
}


