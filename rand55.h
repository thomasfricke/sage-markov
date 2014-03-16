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
#ifndef _RAND55_H
#define _RAND55_H 
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long rand55_t;
/* RAND_SWITCH construct */
#define RAND_SWITCH_BEGIN(var,init){  rand55_t var=init;{{
#define RAND_SWITCH_END(error) }}error;rand55_exit:;}
#define RAND_SWITCH_BREAK goto rand55_exit
#define RAND_SWITCH_CASE(var,prob) }}{rand55_t rand55_p=prob; if(var>=rand55_p) var-=rand55_p ; else { 
#define RAND_SWITCH_DEFAULT }}{{

#define RAND_PROBE_BEGIN(var,init,first) {rand55_t var=init, rand55_last=first; if(var<rand55_last){ 
#define RAND_PROBE_NEXT(var,part) }else if((var-=rand55_last)<part){rand55_last=part

#define RAND_PROBE_END }}

#define RAND_PROBE_TAIL }else{


#define rand55_modpwr ((int)(sizeof(rand55_t)<<3)) 
#define rand55_K 55
#define rand55_J 24

#define rand55() ( (rand55_k)?(rand55_k)--:((rand55_k)=54),(rand55_j)?(rand55_j)--:((rand55_j)=54),rand55_s[(rand55_k)]=rand55_s[(rand55_k)]+rand55_s[(rand55_j)])

#ifdef NONDP
#error "randmin not implemented"
#define exp_rand55 rand_min
#else
#define exp_rand55() ( - log( 1.0 - drand55() ) )
#endif

#ifdef __TURBOC__
#define drand55() ldexp((double)(rand55()>>1),1-rand55_modpwr)
#else
#define drand55() ldexp((double)(rand55()),-rand55_modpwr)
#endif

#define lrand55(l) ((rand55()>>1)%(l))

unsigned long init_rand55(unsigned long);

extern unsigned long int rand55_sel;
extern short     rand55_j,rand55_0j;
extern short     rand55_k,rand55_0k; 
/* --> Knuth, Art of Computer-Programming, Vol. 2, p. 172 */
extern rand55_t rand55_s[rand55_K],rand55_0s[rand55_K];

extern long     rand55_alias;
#define alias_rand55(aliasprob, aliaselse, nalias) ( aliasprob[  _alias55 = ( rand55()&(nalias-1) )  ] > drand55()  ? _alias55 : aliaselse[_alias55] )

void     init_alias55(double *prob, double *aliasprob,
                        int *aliaselse, const int nalias);

double gauss_rand55(void);


#ifdef __cplusplus
} /* close extern "C" */
#endif

#endif
