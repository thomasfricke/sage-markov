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

/*

   THE LC PACKAGE

   Function definitions of the dual-logarithmic-classes code.

   Idea and first coding      : Thomas Fricke

   Last modification          : September 23, 1994


*/

#include <string.h>

/* #define debug_printf(x...) printf(x) */
#define debug_printf(x...)

#define CEND NULL

char *lc_types[]={0,
                  "double",
                  "unsigned long",
                  "long double",
                  "unsigned long long"};

/* ========================================================================= */
/*                                                                           */
/*                                  Contents                                 */
/*                                                                           */
/* ========================================================================= */

/* This file consists of two parts:

   - Public function definitions:
       lc_init
       lc_clear
       lc_enter
       lc_rand
       lc_knownchange
       lc_safechange
     which constitute the user interface of the lc package

   - Private function definitions:
       lc_delete
       lc_getclass
       lc_reorg
       lc_store
       lc_unknownchange
      which are internal to lc and should not be called by the user
*/


/* ========================================================================= */
/*                                                                           */
/*                                  Includes                                 */
/*                                                                           */
/* ========================================================================= */

#include <rand55.h>
#include <logclass.h>   /* contains all further includes, definition of

preprocessor commands, type definitions and
function prototypes                          */


#ifdef __sparc__
#define  memmove(s1, s2, n) bcopy((char*) s2, (char*) s1, n)
#endif


static void lc_print_queue(lc_global * lc_g, char * s){
	lc_class * class_i;
	printf("%s\n",s);
	for(class_i=lc_g->first; class_i; class_i=class_i->next){
		printf("%d.r=%f  #%d, p=%d n=%d\n",
			class_i-lc_g->cbeg,class_i->r,class_i->top-class_i->bot,
			class_i->prev-lc_g->cbeg,class_i->next-lc_g->cbeg);
	}
	printf("------------\n");
}

static void lc_check_queue(lc_global * lc_g, char * s){
	lc_class * class_i;
	double r=0;
	if(lc_g->first && lc_g->first->prev){
		printf("first prev error r==%lf\n",lc_g->first->prev->r);
		lc_print_queue(lc_g,s);
		exit(1);
	}
		
	for(class_i=lc_g->first; class_i; class_i=class_i->next){
		if( ( class_i->prev && class_i->prev->next!=class_i) ||
			( class_i->next && class_i->next->prev!=class_i) ){
				lc_print_queue(lc_g,s);
				exit(1);
			}
		r+=class_i->r;	
	}
	
	double error=abs(lc_g->r-r)/r;
	static double rel_error=0;
	
	if(error>rel_error){
		rel_error=error;
		printf("integrity rel error %g\n",rel_error);	
		//lc_print_queue(lc_g,s);
		//exit(1);
	}

}
#define LC_CHECK_QUEUE(lc,s) lc_check_queue((lc),(s))

/* ========================================================================= */
/*                                                                           */
/*                              Public functions                             */
/*                                                                           */
/* ========================================================================= */

void lc_init(lc_global *lc, size_t num_leds, void (*event_moved)(lc_event *),double timescale)

/* - allocates memory for class and LC event descriptors
   - initializes elements of lc_global data structure
   - sets seed for random generator

   Called by: user-program
*/

{ int    i, classlow, classhigh, classn;

    frexp(lc_reactivity_max,&classhigh);
    frexp(lc_reactivity_min,&classlow);
    classlow--; /* one spare extra */

    classn = classhigh - classlow;         /* number of classes required */
    if(classlow>classhigh) {
        fprintf(stderr,
                "Error initialising class limits: lc_reactivity_max : "LC_FORMAT\
                " lc_reactivity_min: "LC_FORMAT" classhigh: %d classlow: %d\n",
                lc_reactivity_max,lc_reactivity_min,classhigh,classlow);
        fflush(NULL);
        exit(1);
    }

    /*
    ** the num_leds parameter is not really needed
    ** It is allowed to set num_leds=0 or 1, it will be increased up to a
    ** reasonable value
    */
    if(num_leds<1)
        num_leds=1;

    while( num_leds<2*classn)
        num_leds*=2;

    /* allocate memory for class descriptors, one extra as upper limit */
    lc->cbeg=calloc(classn+1,sizeof(lc_class));
    lc->cend=lc->cbeg+classn; /* get pointer to upper limit */

	lc->first=NULL;
    /* allocate memory for event descriptors, no delimiter needed */
    lc->cbeg->top=lc->cbeg->bot=calloc(num_leds,sizeof(lc_event));
	lc->cbeg->next=lc->cbeg->prev=NULL;

    /* set pointers from class descriptors to event array, all classes are
       empty, the same amount of memory is reserved for each class, build
       doubly linked list of class descriptors                                    */
    for (i=1; i<classn; i++) {
        lc->cbeg[i].top = lc->cbeg[i].bot =   /* bot == top means empty  */
                              lc->cbeg->bot+(num_leds/classn)*i;     /* each class same portion */
        lc->cbeg[i].next = lc->cbeg[i].prev =NULL;
    }

    /* set up empty, limiting class at upper end of class descriptor array */
    lc->cbeg[classn].bot = lc->cbeg[classn].top = lc->cbeg->bot+num_leds;
    lc->cbeg[classn].next = 	lc->cbeg[classn].prev = NULL;
    /*
       enter "linking" function into lc, "tell cell that event moved"
       is default
    */
    if ( event_moved )
        lc->event_moved=event_moved;
    else
        lc->event_moved=lc_tell_cell_that_event_moved;

    lc->max_events  = num_leds;   /* initialize remaining elements of lc */
    lc->min_class   = classlow;
    lc->num_classes = classn;
    lc->number_of_reorgs = 0;
    lc->number_of_checks = 0;
    lc->r=0;

#ifdef LC_ROUND_OFF_ERRORS

    lc->eps=ldexp(1,classlow+2);
#endif
    /*
    ** remember timescale
    */

    lc->time_scale=timescale;

}  /* end -- lc_init */

/*--------------------------------------------------------------------------*/

void lc_clear(lc_global *lc) /* if lc not longer needed */

/* - returns memory allocated by lc_init() to OS

   Called by: user-program
*/

{
    free(lc->cbeg->bot);   /* free event descriptors first, then class descs. */
    free(lc->cbeg);        /* i.e. reverse allocation order                   */
}

/*--------------------------------------------------------------------------*/

lc_event  *lc_enter(lc_global *lc, void *ued, lc_reactivity_t r)

/* - assigns a led to the event represented by ued and places it in a
     class according to its reactivity r; sets up link led->ued and
     stores reactivity in led

  Called by: user-program, lc_safechange
  Calls    : lc_getclass, lc_store
*/

{ lc_class *events_class;

    events_class = lc_getclass(lc, r);          /* determine event's class */
    return lc_store(lc, events_class, ued, r);  /* get a led and return it */
}

/*--------------------------------------------------------------------------*/

lc_event *lc_rand(lc_global *lc, lc_class **re_class_ptr)

/* - select one event at random as described by Fricke
   - as a side effect, lc_rand exchanges adjacent elements of the linked
     list of class descriptors if they are no longer in descending order
     of class reactivity, so the list remains partially sorted

   Called by: user-program
*/

{ lc_reactivity_t    r, sum_r;          /* limit for linear selection of class          */
    lc_event *re;        /* ptr to led of event selected                 */
    lc_class *re_class,  /* ptr to descriptor of class of event selected */
    **preds_next,  /* used in sorting linked list                  */
    *old_class;
    size_t    class_size; /* number of events in class selected           */
    int       ci;         /* class index */

    /* Select a class by linear selection */
    sum_r=0;
    /* draw random selection limit, uniformly distributed in 0 <= r < lc_r */
    r = (lc_reactivity_t)(((double)lc->r) *drand55());

    /* search linked list of classes reactivites of classes searched from r
       until r <= 0, sort along the way */

    re_class  = lc->first;     /* start at top of list, not at lc->cbeg    */
    preds_next= &(lc->first);  /* if first class in list is exchange, store
    				 ptr to new top's descriptor in lc->first */

    while( re_class )  /* empty class at end of array AND list */
    {

        if ( re_class->next && re_class->next->r > re_class->r) /* next class has higher react. */
        {                                   /* exchange descriptors in list */
//						if(re_class->r==0)
//							printf("skip empty class");
            old_class        = re_class;         /* save ptr to current class    */
            re_class      = re_class->next;   /* make next class current      */
            old_class->next  = re_class->next;   /* update link next->next's next*/
            re_class->next= old_class;           /* update link current->next    */

		//				printf("%d old %x->%x->%x re %x->%x->%x\n",__LINE__,old_class->prev,old_class,old_class->next,
		//								re_class->prev,re_class,re_class->next);

            *preds_next   = re_class;  /* update link current's predecessor ->
            					current, preds_next points to pred.'s
            					next element or lc->first          */
						{
							int xit=0;
							re_class->prev=old_class->prev;
							old_class->prev=re_class;
							if(old_class->next)
								old_class->next->prev=old_class;
			//				printf("%d old %x->%x->%x re %x->%x->%x\n",__LINE__,old_class->prev,old_class,old_class->next,
//										 re_class->prev,re_class,re_class->next);
							LC_CHECK_QUEUE(lc,"lc_rand");

						}
        } /* end -- if ( re_class-> ... */
        sum_r+=re_class->r;
#if 0

        if( r < re_class->r) /* selection limit is reached */
            break; /* from while ( re_class != lc->cend ) */
        else
            r-=re_class->r;/* count down r               */
#endif

        if(sum_r>r)
            break;
        /* current class becomes predecessor, current->next becomes current */
        preds_next = &(re_class->next); /* get ptr to pred's next element   */
        re_class   = re_class->next;    /* step to next class in list       */

    }  /* end -- while ( re_class != lc->cend ) */

 	if(!re_class->next)
 		lc->r=sum_r;
 		
    class_size = re_class->top - re_class->bot; /* ->top is NOT a valid led */
    ci         = ( re_class - lc->cbeg ) + lc->min_class;
    /*             ^^^^^^^^^^^^^^^^^^^^     ^^^^^^^^^^^^^^
          pointers               integer
       index is given by position in array of class descs. plus offset */

    /* Select an event from re_class by von Neumann rejection, upper bound
       is maximum reactivity represented by re_class, i.e. 2^ci            */
    do {
        /* draw any event from re_class, uniformly distributed */
        re = re_class->bot+lrand55(class_size);
    } while ( ldexp(drand55(), ci) > re->r );   /* test rejection criterium */
    /* using ldexp here is faster than calculating x=2^ci before entering the
       loop and multiplying drand55()*x every time; besides, average acceptance
       is 75% */

    /* return */
    *re_class_ptr = re_class;   /* store pointer to re's class' descriptor */
    return re;                  /* pointer to the event's led              */

} /* end -- lc_rand */

/*--------------------------------------------------------------------------*/

lc_event *lc_knownchange(lc_global *lc, lc_class *events_class,
                         lc_event *led, void *ued, lc_reactivity_t r)

/* - move the event described by led/ued whose reactivity r has changed
     to the appropriate class if its current class is known;
   - update event's reactivity, class reactivity, total reactivity
   - reorganize memory if necessary

   Called by: user-program, lc_safechange, lc_unknownchange
   Calls    : lc_delete, lc_getclass, lc_store
*/

{ lc_class *events_new_class;

    /* determine class according to NEW reactivity */
    events_new_class = lc_getclass(lc, r);

    if ( events_new_class == events_class )  /* event remains in old class */
    {                                      /* just update reactivities   */
        events_class->r += (r - led->r);     /* class reactivity           */
        lc->r          += (r - led->r);     /* total     "                */
        led->r           = r;                /* event's reactivity         */
#ifdef LC_ROUND_OFF_ERRORS
        /* check for accumulated round-off errors and set to 0 */
        if ( events_class->r < lc->eps ) {
            lc->r -= events_class->r;
            events_class->r = 0;
        }
        if ( lc->r < lc->eps )
            lc->r = 0;
#endif

        return led;    /* unchanged pointer to led */
    } else {
        lc_delete(lc,events_class,led);     /* remove event from old class */
        if(r==0){
        		LC_CHECK_QUEUE(lc,"lc_known r==0");
            return NULL;
        } else {
            led=lc_store(lc,events_new_class,ued,r);  /* put it in new class,  */

            /* get new led           */
			LC_CHECK_QUEUE(lc,"lc_known r!=0");
            return led;   /* pointer to new led */
        }
    }
}   /* end -- lc_knownchange */

/*--------------------------------------------------------------------------*/

lc_event *lc_safechange(lc_global *lc, lc_event *led, void *ued, lc_reactivity_t r)

/* - move or enter the event described by led/ued whose NEW reactivity is r
     to the appropriate class
   - update event's reactivity, class reactivity, total reactivity
   - reorganize memory if necessary

   Called by: user-program
   Calls    : lc_enter, lc_knownchange
*/

{
    if ( led )  {    /* the event has a led already */
        if(r==0) {
            lc_delete(lc,lc_getclass(lc, led->r),led);
            return NULL;
        } else
            return lc_knownchange(lc, lc_getclass(lc, led->r), led, ued ,r);
    } else            /* the event has to be assigned a led */
        return lc_enter(lc, ued, r);
}


/* ========================================================================= */
/*                                                                           */
/*        Private functions: users should keep their HANDS OFF !!!           */
/*                                                                           */
/* ========================================================================= */

void lc_delete(lc_global *lc, lc_class *events_class, lc_event *led)

/* - remove event from its class, reduce class and total reactivity
     accordingly

   Called by: lc_knownchange
   Calls    : lc->event_moved
*/

{ lc_reactivity_t ci;      /* class index */

    if ( !events_class || !led )   /* just to be on the safe side */
        return;

    events_class->r -= led->r;  /* reduce class and */
    lc->r          -= led->r;  /* total reactivity */

    /* remove led from class' array of leds F[] */
    events_class->top--; /* ->top, i.e. the delimiting, invalid led just beyond
    			  the last valid led in the array, goes down by one
    			  as array is kept contiguous */

   LC_CHECK_QUEUE(lc,"before delete");
   	if ( events_class->top == events_class->bot ) {
   		//events_class->r = 0;
   		if(events_class==lc->first){
   			lc->first=lc->first->next;
   			if(lc->first)
   				lc->first->prev=NULL;
   			else
   				lc->r=0;
   			LC_CHECK_QUEUE(lc,"after delete first");
   		} else{
	     //   lc->r -= events_class->r;
	      
	 		if(events_class->next)
	 			events_class->next->prev=events_class->prev;
	 		if(events_class->prev)
	 			events_class->prev->next=events_class->next;
	 		
	   	}
    }
 	LC_CHECK_QUEUE(lc,"after delete");
    
    if ( led != events_class->top )   /* if led is not at the top of the array,
        				       we got a gap inside */
    {
        *led = *(events_class->top);  /* fill the gap by the led at top */
        lc->event_moved(led); /* update the link ued->led for the event moved */
    }

}  /* end -- lc_delete */

/*--------------------------------------------------------------------------*/

void lc_tell_cell_that_event_moved(lc_event *lce) {
    ((lc_base*)(lce->ued))->lc_ev=lce;
    /*
       when the event has to be moved in memory,
       the cell is noticed this way
    */
}



/*--------------------------------------------------------------------------*/

lc_class *lc_getclass(lc_global *lc, lc_reactivity_t r)

/* - determines the class that comprises the reactivity r

   Called by: lc_enter, lc_knownchange, lc_unknownchange
*/
{ int    ci;    /* class index */

    /* calculate class index */
    errno = 0;       /* reset before use                               */
    frexp(r,&ci);    /* class index is defined by 2^(ci-1) <= r < 2^ci */
    ci -= lc->min_class;  /* subtract offset to get class-array index */

    /* if ok, return pointer to class descriptor */
    return lc->cbeg + ci;

} /* end -- lc_getclass */

/*--------------------------------------------------------------------------*/

void lc_reorg(lc_global *lc)

/* This function is called if all leds allocated for a class have been
   assigned to events and lc_store requests another led in that class.
   Then, lc_reorg re-distributes the lc->max_events leds allocated by
   lc_init to the classes proportional to their current size.  In the
   end, the order of leds currently assigned to events is not changed,
   just the "free" leds have been shifted around.  All links are main-
   tained properly.
   To do so, lc_rand employs the following strategy:
     0. Recalculate class and total reactivites to reduce round-off errors.
     1. Move all assigned leds downward in memory, so they occupy a
        contiguous are in memory between lc->cbeg->bot and
	lc->cbeg[lc->num_classes-1]->top and all "free" leds occupy
	memory from there to lc->cend.
     2. Move the leds for each class upward to their new location, beginning
        with the highest class.
     3. Update the links ued->led for all events using lc->event_moved.

   Called by: lc_store
   Calls    : lc->event_moved
*/

{ size_t
    class_size,   /* no. of assigned leds in class        */
    new_size,     /* no. of leds to be allocated to class */
    tot_needed,   /* total number of leds needed          */
    prop_needed;  /* proposede number of needed leds      */

    lc_event *led;
    lc_class *cd;
    int       ci;           /* class index                   */
    size_t    shift=0;

    /* 0. recalculate reactivities */
    for ( cd = lc->cbeg, lc->r = 0; cd < lc->cend; cd++ ) {
        for ( led = cd->bot, cd->r = 0; led < cd->top; led++ )
            cd->r += led->r;
        lc->r += cd->r;
    }

    for(tot_needed = 2*lc->num_classes , ci=0; ci< lc->num_classes; ci++)
        tot_needed += lc->cbeg[ci].top - lc->cbeg[ci].bot;
    debug_printf("tot %d\n",tot_needed);

    led=lc->cbeg->bot;

    prop_needed=tot_needed;
    /*
    **  0. Compute sizes and shifts
    */
    if ( tot_needed * 2> lc->max_events ) {    /* out of memory !!! */
        /*
        ** increase prop_needed until it is more than two times bigger than
        ** the tot_needed real number of cells
        */
        prop_needed=lc->max_events;
        while(tot_needed*2>prop_needed)
            prop_needed*=2;

        /*
        ** recycle and allocate new memory
        */
        lc->cbeg->bot=realloc(led,prop_needed*sizeof(lc_event));
        /*
        ** for testing purpose
        */
        /* 		lc->cbeg->bot=calloc(prop_needed,sizeof(lc_event)); */
        /* 		memmove(lc->cbeg->bot,led,lc->max_events*sizeof(lc_event)); */
        /* 		free(led); */
        debug_printf("needed %d expanding from %d to %d\n",tot_needed,lc->max_events,prop_needed);

        /*
        ** how much do we have to shift the top and bot(tom) pointers of each class?
        */
        shift=lc->cbeg->bot -led;
        /*
        ** Lowest class here
        */
        lc->cbeg->top+=shift;

        if(! lc->cbeg->bot) {
            fprintf(stderr, "Reorganization error: run out of memory while reorganizing event descriptors.\n");
            fflush(NULL);
            exit(1);
        }
        /*
        ** Move the class end cend to stretch the classes below
        ** cend is empty and not crunched and stretched below
        */

				lc->cend->bot+=shift+(prop_needed-lc->max_events);
        lc->cend->top+=shift+(prop_needed-lc->max_events);

        /*
        ** max_events becomes the proposed needed value
        */
        lc->max_events=prop_needed;
    }

    ci=0;
    debug_printf("%2d: n=%2d s=%2d r=%5d b=%X t=%X x=%d4\n",
                 ci,new_size,class_size,lc->cbeg[ci].r,lc->cbeg[ci].bot,lc->cbeg[ci].top,lc->cbeg[ci].bot-lc->cbeg[0].bot);
    /* 1. move assigned leds downward, calculate no. of leds needed */
    /*   add a little extra and reserve 2 leds for classes currently empty */
    /* needed for lowest class */
    for ( ci = 1; ci < lc->num_classes; ci++) {
        /*
        ** shift the top and bot pointers
        */
        lc->cbeg[ci].bot+=shift;
        lc->cbeg[ci].top+=shift;

        class_size = lc->cbeg[ci].top - lc->cbeg[ci].bot;
        /* move assigned leds of class ci down using memove(to, from, bytes) */
        memmove(lc->cbeg[ci-1].top, lc->cbeg[ci].bot,
                class_size * sizeof(lc_event));
        /* update pointers from class descriptors to class' array of leds
           making use of the contiguous packing of assigned leds          */
        lc->cbeg[ci].bot = lc->cbeg[ci-1].top;
        lc->cbeg[ci].top = lc->cbeg[ci].bot + class_size;
        debug_printf("%2d: sh=%2d s=%2d r=%5d b=%X t=%X x=%4d\n",
                     ci,shift,class_size,lc->cbeg[ci].r,lc->cbeg[ci].bot,lc->cbeg[ci].top,lc->cbeg[ci].bot-lc->cbeg[0].bot);
    }  /* end -- for ( ci = 1; ... */


    /* 2. move leds up to the new locations class by class, starting from top */
    for ( ci = lc->num_classes - 1; ci ; ci--)   {
        class_size = lc->cbeg[ci].top - lc->cbeg[ci].bot;
        new_size   = (size_t) floor((class_size+2.0) *
                                    (lc->max_events / (lc_reactivity_t) prop_needed));

        /* bottom of class ci's led-array is reached from bottom of class ci+1
        (has been moved already) moving down by new_size spaces for leds,
        top of ci points to first free led in class' array */
        lc->cbeg[ci].bot = lc->cbeg[ci+1].bot - new_size;
        lc->cbeg[ci].top = lc->cbeg[ci].bot + class_size;

        /* finally move leds to the right place */
        memmove(lc->cbeg[ci].bot, lc->cbeg[ci-1].top,
                class_size * sizeof(lc_event));
        debug_printf("%2d: n=%2d s=%2d r=%5d b=%X t=%X x=%4d\n ",
                     ci,new_size,class_size,lc->cbeg[ci].r,lc->cbeg[ci].bot,lc->cbeg[ci].top,lc->cbeg[ci].bot-lc->cbeg[0].bot);

    }  /* end -- for ( ci = lc-> ... */
    ci=0;
    debug_printf("%2d: n=%2d s=%2d r=%5d b=%X t=%X x=%4d\n",
                 ci,new_size,class_size,lc->cbeg[ci].r,lc->cbeg[ci].bot,lc->cbeg[ci].top,lc->cbeg[ci].bot-lc->cbeg[0].bot);

    /* check if leds have been moved too far down due to round-off errors */
    if ( lc->cbeg[1].bot < lc->cbeg[0].top ) {
        fprintf(stderr, "Reorganization error: reallocation of event descriptors failed.\n");
        fflush(NULL);
        exit(1);
    }

    /*
    ** 3. update links ued->led
    ** If no realloc was called, the lowest class needs not to be moved.
    ** Than we start at ci=1
    */
    for ( ci = (led ==lc->cbeg->bot) ; ci < lc->num_classes; ci++ )
        for ( led = lc->cbeg[ci].bot;  led < lc->cbeg[ci].top; led++ )
            lc->event_moved(led);

    lc->number_of_reorgs++;  /* maintain counter for performance control */
    return;
}  /* end -- lc_reorg */

/*--------------------------------------------------------------------------*/

lc_event  *lc_store(lc_global *lc, lc_class *events_class, void *ued,
                    lc_reactivity_t r)

/* - places the event described by ued in the class given by events_class
     according to its NEW reactivity r
   - stores event's, increases class, total reactivity by r
   - sets link led->ued
   - does NOT update link ued->led
   - reorganize memory if necessary

   Called by: lc_enter, lc_knownchange
   Calls    : lc_reorg
*/

{ 
	lc_event *led;   /* pointer to led assigned to event */
	lc_class * class_i, *old_class_i;
   
   	LC_CHECK_QUEUE(lc,"before insert");
    	/* empty class */
	if(events_class->bot==events_class->top){
		
		if(lc->first){
			old_class_i=class_i= lc->first ;
			while( class_i && r < class_i->r){
				old_class_i=class_i;
				class_i=class_i->next;
			}
			if(class_i){
				if(class_i==lc->first){
					events_class->next=lc->first;
					lc->first->prev=events_class;
					events_class->prev=NULL;
					lc->first=events_class;
					LC_CHECK_QUEUE(lc,"class_i==first");
				}else{
					events_class->prev=class_i->prev;
					if(events_class->prev)
						events_class->prev->next=events_class;
					events_class->next=class_i;
					class_i->prev=events_class;
					LC_CHECK_QUEUE(lc,"class_i");
				}
			}else{
				old_class_i->next=events_class;
				events_class->prev=old_class_i;
				events_class->next=NULL;
				LC_CHECK_QUEUE(lc,"old_class_i");
			}
			
		}else{
			lc->first=events_class;
			events_class->next=events_class->prev=NULL;
			LC_CHECK_QUEUE(lc,"first");
		}
	}
	LC_CHECK_QUEUE(lc,"after insert");
    led = events_class->top;   /* top is the first "free" led */

    /* check for collision with next class */
    if ( led == (events_class+1)->bot ) {
        lc_reorg(lc);          /* reorganize memory */
        led = events_class->top; /* now it's okay     */
        LC_CHECK_QUEUE(lc,"lc_store reorg");
    }

    events_class->top++;   /* move top                   */
    led->ued = ued;        /* create link to ued         */
    led->r   = r;          /* store event's reactivity   */
    events_class->r += r;  /* update classes' reactivity */
    lc->r          += r;  /* update total reactivity    */
	LC_CHECK_QUEUE(lc,"lc_store end");
	return led;     /* ... pointer to assigned led */

} /* end -- lc_store */

/*--------------------------------------------------------------------------*/

lc_event *lc_unknownchange(lc_global *lc, lc_event *led, void *ued,
                           lc_reactivity_t r)

/* - move the event described by led/ued whose NEW reactivity is r to
     the appropriate class
   - update event's reactivity, class reactivity, total reactivity
   Called by: not used in this version
   Calls    : lc_getclass, lc_knownchange
*/

{
    return lc_knownchange(lc, lc_getclass(lc, led->r), led, ued, r);
}

/*
** lc_check: checks the integrity of all classes
** parameters:
**             lc_g  : the global pointer
**             index : a function returning the integer
**                     of the void * pointer representing the lc_base and
**             event : extracting its event
*/
void lc_check(lc_global * lc_g, size_t(index)(void*),lc_event * (event)(void *)) {
    int i;
    static int output=0;
    int error=0;
   
    lc_class *lc_c;
    lc_g->number_of_checks++;
      
   	lc_check_queue(lc_g,"check");
    
    if(error && !output) {
        output=1;
        lc_check(lc_g,index,event);
        exit(1);
    }
    if(error && output) {
        printf("\nclass integrity error\n");
        exit(1);
    }

}
