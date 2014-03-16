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

/**************************************************************************
*    File:
*        random.h
*
*    A number of macros and procedures generating random numbers 
*    according the most important distributions.
*    
*    Author:
*        Thomas Fricke
*        Inst. f. Theor. Physik
*        RWTH Aachen
*        Sommerfeldstrasse
*        D - W 5100 Aachen
*
*    Based on:
*        Donald E. Knuth
*        The Art of Computer-Programming, Vol. II
*        Seminumerical Algorithms
*        Addison-Wesley, Massachusetts
*
*    The initialization:
*
*    init_rand55(seed):
*                    procedure randomizing the generator by a seed.
*                    The seed determines the pseudo random number
*                    sequence totally, same seed results in same sequence.
*        !!!         If not called at first, before using any function or
*                    macro, the sequence of random numbers is always the same !
*                    If seed is 0L, system time or clock function is used
*                    to imitate real randomness. The function returns the
*                    seed to allow to reproduce the sequence.
*
*    The uniform distributions:
*
*    drand55()    :    macro generating a uniform distributed double random 
*                    number in the range [0..1), 32 or (if the compiler
*                    does not convert ulong-->float correctly, like
*                    Atari TC 2.03) 31 bits are relevant.
*    lrand55(l)    :    macro generating an uniform distributed unsigned long
*                    random number in the range 0..l-1. Avoid use because
*                    the %-division wastes time. If possible use
*                    (rand55() & mask) instead!
*    rand55()    :    macro generating an uniform distributed unsigned long
*                    random number in the range 0..MAX_ULONG. The direct
*                    way!
*
*    The Alias-Method by A. J. Walker
*
*     init_alias55(double *prob, double *aliasprob, 
*                    int *aliaselse, const int nalias):
*
*                    input are three arrays of size nalias, the double array
*                    prob contains the (normalized) probabilities 
*                    of the random numbers to generate. The i'th entry of
*                    aliasprob is changed to the probability of the
*                    alias-method to accept i directly.
*                    The aliaselse array is set up to the alias to generate
*                    if i is not accepted. The possible range of i 
*        !!!            is 0..nalias-1,    where nalias MUST BE A POWER OF 2,
*                    else you have to fill up your array by 0s!
*                    
*    alias_rand55(aliasprob, aliaselse, nalias) :
*                    macro, whose inputs are the arrays aliasprob and aliaselse
*                    set up by init_alias55(prob,aliasprob,aliaselse,nalias).
*                    It generates long integer random numbers according the
*                    probability distribution prob you have called with 
*                    the procedure init_alias55().
*
*    Some important distributions: 
*
*    exp_rand55():
*                    macro, generating exponentially distributed double
*                    random numbers in the range [0..infinity) with mean
*                    1.0.
*
*    gauss_rand55():    function, using a modified Marsaglia-algorithm to
*                    generate gaussian distributed random numbers with
*                    mean 0 and standard deviation 1.
*/
#ifndef _RANDOM55_H
#define _RANDOM55_H 
#include <math.h>

class Random55{
 private:
  short K;
  short J;

  static const  short K0 =55;
  static const  short J0 =24;

 public:
  typedef unsigned long T;
  static const  short modpwr   =(sizeof(T)<<3);

 private:
  T S[K0],S0[K0];
  


 public:
  inline T raw(){
    if(K)
      (K)--;
    else
      K=54;
    if(J)
      J--;
    else    
      J=54;
    S[K]=S[K]+S[J];
    return S[K];
  }
    
  inline double urn(){
    return ldexp((double)(raw()),-modpwr) ;
  }
    
  inline double exp(){
    return - log( 1.0 - urn() ) ;
  }

  inline T lurn(T l){
    return (raw()>>1)%(l);
  }

  double gauss();
    
  Random55(unsigned long=0);
  unsigned long seed;

  class Alias{
    public:
    double *aelse;
    long *alter, n,n1;
    Alias(const double * prob, long na);
    ~Alias(){
      delete aelse;
      delete alter;
    }
  };

  inline long alias(const Alias & a){
    long alias = raw()&a.n1;
    if( a.aelse[ alias   ] > urn() )
      return alias ;
    else
      return a.alter[alias];
  }
};

#endif

