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
#include "rand55.h"
#include "gauss55.h"


short rand55_0j=24,rand55_j=24;
short rand55_0k=55, rand55_k=55;
rand55_t rand55_s[rand55_K];
rand55_t rand55_0s[rand55_K];

int 	rand55_g_else[rand55_N_GAUSS];
double 	rand55_g_prob[rand55_N_GAUSS]={
                     /* 0 */ 0.098959421736187371033 ,
                     /* 1 */ 0.096667029200712301735 ,
                     /* 2 */ 0.092963773467442227819 ,
                     /* 3 */ 0.088016331691074869444 ,
                     /* 4 */ 0.082040242137593758515 ,
                     /* 5 */ 0.075284358038701101234 ,
                     /* 6 */ 0.068013749594635876088 ,
                     /* 7 */ 0.060492681129785837449 ,
                     /* 8 */ 0.052969161443924865725 ,
                     /* 9 */ 0.045662271347255476248 ,
                     /* 10 */ 0.038753066364573299289 ,
                     /* 11 */ 0.032379398916472931904 ,
                     /* 12 */ 0.026634567032646265749 ,
                     /* 13 */ 0.021569329706627878609 ,
                     /* 14 */ 0.017196568956672974511 ,
                     /* 15 */ 0.013497741628297012988 ,
                     /* 16 */ 0.010430246314084651488 ,
                    /* 17 */ 0.0079349129589168539375 ,
                    /* 18 */ 0.0059429752074784507281 ,
                    /* 19 */ 0.0043820751233921343406 ,
                    /* 20 */ 0.0031810453992078579569 ,
                    /* 21 */ 0.0022733906253977631925 ,
                    /* 22 */ 0.0015995300776808890400 ,
                    /* 23 */ 0.0011079621029845017939 ,
                    /* 24 */ 0.00075556450879968902418 ,
                    /* 25 */ 0.00050726201432494194641 ,
                    /* 26 */ 0.00033527971837259440686 ,
                    /* 27 */ 0.00021817067376144001640 ,
                    /* 28 */ 0.00013976538055804120485 ,
                   /* 29 */ 0.000088148920591861347578 ,
                   /* 30 */ 0.000054732909441153024206 ,
                               /* 31 */ 0 ,
                     /* 32 */ 0.000517027924038414925 ,
                     /* 33 */ 0.001269172504909360787 ,
                     /* 34 */ 0.001963108512157912451 ,
                     /* 35 */ 0.002569057511503749086 ,
                     /* 36 */ 0.003063777216978969585 ,
                     /* 37 */ 0.003431995304963564736 ,
                     /* 38 */ 0.003667049454079271332 ,
                     /* 39 */ 0.003770716712321310951 ,
                     /* 40 */ 0.003752312145371527875 ,
                     /* 41 */ 0.003627215592651717602 ,
                     /* 42 */ 0.003415036266465776201 ,
                     /* 43 */ 0.003137643248482375976 ,
                     /* 44 */ 0.002817276674630770811 ,
                     /* 45 */ 0.002474915396177036001 ,
                     /* 46 */ 0.002129021240438456239 ,
                     /* 47 */ 0.001794718005867322702 ,
                     /* 48 */ 0.001483404685376137742 ,
                    /* 49 */ 0.0012027546278913649225 ,
                    /* 50 */ 0.0009570199111342322119 ,
                    /* 51 */ 0.0007475444165323186994 ,
                    /* 52 */ 0.0005733885263192951231 ,
                    /* 53 */ 0.0004319796305182405475 ,
                    /* 54 */ 0.0003197214125362211100 ,
                    /* 55 */ 0.0002325168136473125161 ,
                    /* 56 */ 0.00016618095624363590582 ,
                    /* 57 */ 0.00011673849911038808359 ,
                    /* 58 */ 0.00008061345785880506314 ,
                    /* 59 */ 0.00005472807671764453360 ,
                    /* 60 */ 0.00003653132575053733515 ,
                   /* 61 */ 0.000023977960769002442422 ,
                   /* 62 */ 0.000015476961458262025794 ,
                         /* 63 */ 0.340743E-14    };


void printdat(){
  int i;
  /*	init gaussian random numbers */
  init_alias55(rand55_g_prob, rand55_g_prob,
	       rand55_g_else, rand55_N_GAUSS);
  printf("#include \"rand55.h\"\n",rand55_j) ;
  printf("#include \"gauss55.h\"\n",rand55_j) ;
  printf("\nshort rand55_0j,rand55_j = %d;\n",rand55_j) ;
  printf("\nshort rand55_0k,rand55_k = %d;\n",rand55_k) ;
  printf("\nunsigned long rand55_s[rand55_K]={");
  for(i=0; i<rand55_K; i++)
    {
      if(i)
	printf(",\n\t");

      printf("%luUL",rand55_s[i]);

    }
  printf("};\n");

  printf("\nunsigned long rand55_0s[rand55_K]={");
  for(i=0; i<rand55_K; i++)
    {
      if(i)
	printf(",\n\t");

      printf("%luUL",rand55_s[i]);

    }
  printf("};\n");
  
  printf("\ndouble rand55_g_prob[rand55_N_GAUSS]={");
  for(i=0; i<rand55_N_GAUSS; i++)
    {
      if(i)
	printf(",\n\t");
      
      printf("%.20le",rand55_g_prob[i]);
      
    }
  printf("};\n");
  printf("\nint rand55_g_else[rand55_N_GAUSS]={");
  for(i=0; i<rand55_N_GAUSS; i++)
    {
      if(i)
	printf(",\n\t");
      
      printf("%d",rand55_g_else[i]);
      
    }
  printf("};\n");
}


int main(int argc, char **arglist)
{
  init_rand55(9999999L);
  printdat();
  return 0;
}
