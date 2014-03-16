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

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "rand55.h"
#include "gauss55.h"

double 	_gauss_reject55[rand55_N_GAUSSHALF]={
  /* 0 */ .79788456080286535588 ,
	  /* 1 */ .79167537388949896826 ,
	  /* 2 */ .77333623360569841388 ,
	  /* 3 */ .74371018773953782255 ,
	  /* 4 */ .70413065352859895555 ,
	  /* 5 */ .65632193710075006812 ,
	  /* 6 */ .60227486430960880987 ,
	  /* 7 */ .54410999675708700870 ,
	  /* 8 */ .48394144903828669959 ,
	  /* 9 */ .42375329155139892580 ,
	  /* 10 */ .36529817077804380998 ,
	  /* 11 */ .31002453091658639431 ,
	  /* 12 */ .25903519133178345523 ,
	  /* 13 */ .21307653626117012599 ,
	  /* 14 */ .17255463765302302887 ,
	  /* 15 */ .13757255165338379609 ,
	  /* 16 */ .10798193302637610390 ,
	  /* 17 */ 0.083441970512677211900 ,
	  /* 18 */ 0.063479303671334831500 ,
	  /* 19 */ 0.047543801659827605825 ,
	  /* 20 */ 0.035056600987137074725 ,
	  /* 21 */ 0.025448363193662863655 ,
	  /* 22 */ 0.018187125003182105540 ,
	  /* 23 */ 0.012796240621447112320 ,
	  /* 24 */ 0.0088636968238760143511 ,
	  /* 25 */ 0.0060445160703975121934 ,
	  /* 26 */ 0.0040580961145995355713 ,
	  /* 27 */ 0.0026822377469807552549 ,
	  /* 28 */ 0.0017453653900915201312 ,
	  /* 29 */ 0.0011181230444643296388 ,
	  /* 30 */ 0.00070519136473489078062 ,
	  /* 31 */ 0.00043786327552922419365 };



#define	aliaselse _g55_else
#define	aliasprob _g55_prob

/*	gauss55 is a version of MARSAGLIAS algorithm to obtain
*	gaussian random numbers, see Knuth II pp. 118 for details.
*/

double gauss_rand55(void)
{
  register	alias55, sign;

  alias55 = (  ( (int)rand55() ) & (127) );

  sign = alias55 & 64;
  alias55 &= 63;

  if( rand55_g_prob[alias55] < drand55() )
    alias55=rand55_g_else[alias55];
  if(  alias55 < rand55_N_GAUSSHALF )
    if( sign )
      return (alias55+drand55())*0.125;
    else
      return -(alias55+drand55())*0.125;
  else
    {
      double x, y, dy;

      alias55-=rand55_N_GAUSSHALF;
      dy = _gauss_reject55[alias55]
	- _gauss_reject55[alias55+1];
      do{
	x=(alias55+drand55()) * 0.125;
	y= dy * drand55();
      }
      while ( (*_gauss_reject55) * exp(-x*x*0.5)
	      -	_gauss_reject55[alias55+1] < y);
      if(	sign	)	return -x;
      else		return  x;
    }
}

/* Knuth II, S. 172 */
unsigned long init_rand55 ( unsigned long seed55 )
{
  long j, k, i, ii ;


  rand55_k=rand55_0k;
  rand55_j=rand55_0j;
  memcpy(rand55_s,rand55_0s, sizeof(rand55_s));

  if(seed55==0)
#ifdef __TURBOC__
    {
      extern unsigned long clock(void);
      seed55=clock() | 1;
    }
#else
  {
    seed55=time(NULL) | 1;
  }
#endif

  if(seed55<100000L)
    {
      fprintf(stderr,"init_rand55: WARNING seed55=%lu is small\n",seed55);
      seed55=9999999L;
    }
  /*
    rand55 initialisieren
  */
  rand55_s[ rand55_K - 1 ] = j = seed55 ;
  k = 1 ;
  for ( i = 0 ; i < ( rand55_K - 1 ) ; i++ )  {
    ii      = ( 21 * i ) % rand55_K ;
    rand55_s[ii] = k ;
    k       = j - k ;
    j       = rand55_s[ii] ;
  }

  /*
    Warmlaufen
  */
  rand55_j = rand55_J ;
  rand55_k = rand55_K ;
  for ( i = 1000000L ; i ; i-- )
    rand55() ;

  return seed55;
}

#define NOELSE (-1)

void init_alias55(double *prob, double *aliasprob, int *aliaselse, const int nalias)
{
  int i, j;
  int	maxi, mini;
  double	maxprob, minprob;

  for( j = 0 ; j < nalias  ; j ++ ) {
    aliaselse[j] = NOELSE;
    aliasprob[j] = prob[j];
  }

  for( j = nalias ; j  ; j -- ) {
    maxprob	=	-1.0;
    minprob =	 2.0;
    /*	determine max and min of not
     *	yet deleted pairs
     */
    for( i = 0 ; i < nalias ; i++) {
      if( aliaselse[i] == NOELSE )  {
	if( aliasprob[i] > maxprob ) {
	  maxprob = aliasprob[i];
	  maxi = i;
	}

	if( aliasprob[i] < minprob ) {
	  minprob = aliasprob[i];
	  mini = i;
	}
      }
    }
    /*	compute probability of jumping to
     *	aliaselse
     */
    aliaselse[mini]	=  maxi;
    aliasprob[maxi]	+=
      aliasprob[mini] - 1.0/nalias;
    aliasprob[mini] *= nalias;
#ifdef TESTXXX
    for( i = 0 ; i < nalias ; i++)
      printf("# %2d->%2d a=%lf p=%lf\n",
	     i, aliaselse[i], aliasprob[i],prob[i]);
    getch();
#endif
  }

}
#undef NOELSE
#undef N_GAUSS
#undef N_GAUSSHALF

