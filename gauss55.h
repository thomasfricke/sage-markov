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

#ifndef _GAUSS55_H_
#define _GAUSS55_H_
#define rand55_N_GAUSS 64
#define rand55_N_GAUSSHALF 32
#include "rand55.h"

extern short rand55_j, rand55_0j ;
extern short rand55_k, rand55_0k ;


extern int 	rand55_g_else[rand55_N_GAUSS];
extern double 	rand55_g_prob[rand55_N_GAUSS];
extern double 	rand55_gauss_reject[rand55_N_GAUSSHALF];

extern rand55_t rand55_s [ rand55_K ], rand55_0s [ rand55_K ] ;


#endif
