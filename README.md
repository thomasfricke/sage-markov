Markov is an optimized simulation tool written in C for arbitrary 
Reaction-Diffusion-Systems (RDS).

It simulates the Gillespie algorithm in the thermodynamical limit, 
and it has been proven [2] that there is no way to do a faster, 
exact simulation of the trajectory of the RDS.

A RDS is a lattice of reaction cells. The diffusion and reaction 
have to be chosen, that a diffusion step to a neighbour cell 
is much more probable than a reaction step in a cell.

The algorithm selects the next step by dividing the cells into reactivity 
classes based on the dual logarithm of the reaction probability.

This guarantees only a few classes and a relative homogeneous 
reaction probability inside the class, where a von-Neuman rejection 
algorithm is used to select a cell with an 75% efficiency.

Inside a cell standard Gillespie can be used.

First selecting the classe, then the cell inside the class t

[1]  http://en.wikipedia.org/wiki/Gillespie_algorithm

[2]  http://link.springer.com/article/10.1007/BF01309430#page-1

[3]  http://www.worldscientific.com/doi/abs/10.1142/S0129183195000216

I have split the simulation into the random generator, the standard algorithm
and domain specific code.

pseudo random generator:
     createtable.c 
     gauss55.h
     rand55.h 
     random55.h 
     rand55.c
     rand55static.c

common algorithm:
     logclass.c 
     logclass.h
     logclassevent.h
     logclasstest.c  

glue code to Sage:
    sagemarkov.h
    sagemarkov.c
    markovian.spyx     

model specific, a simple 1-dimensional random walk without reactions:
    model.h
    randomwalk.h

call sage in the directory sage-markov of the checkout

sage
Sage Version 6.1.1, Release Date: 2014-02-04                       

sage: %attach "markovian.spyx"
   Compiling ./markovian.spyx...
   100 cells empty initialized
sage: m.plot()
<picture pops up>
sage: 
