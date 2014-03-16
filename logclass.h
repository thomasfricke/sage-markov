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

#ifndef __LOGCLASS_H__
#define __LOGCLASS_H__
#ifdef __cplusplus
extern "C" {
#endif

  /*

    THE LOGCLASS PACKAGE

    User interface, type declarations and function headers for the
    dual-logarithmic-classes code.

    Author     : Thomas Fricke

    Last modification          : 2006

    Documentation & References : see logclass.doc.*
    Revision history           : see logclass.rev

  */

  /* ========================================================================= */
  /*                                                                           */
  /*                                  Contents                                 */
  /*                                                                           */
  /* ========================================================================= */

  /* This file consists of four parts:
     - Includes and definition of constants:
     contains all #include directives required by logclass.*
     and defines all preprocessor constants
     - Public type definitions:
     lc_event
     lc_class
     lc_global
     which the user needs to define corresponding variables
     - Public function declarations:
     lc_init
     lc_clear
     lc_enter
     lc_rand
     lc_knownchange
     lc_safechange
     which constitute the user interface of the logclass package
     - Private function declarations:
     lc_delete
     lc_getclass
     lc_reorg
     lc_store
     lc_unknownchange
     which are internal to logclass and should not be called by the user
  */


  /* ========================================================================= */
  /*                                                                           */
  /*                 Includes and definition of constants                      */
  /*                                                                           */
  /* ========================================================================= */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <limits.h>   /* compiling with ULTRIX gcc results in warning that */
#include <float.h>    /* FLT_MAX has been redefined by float.h, but this   */
  /* does not seem to cause problems                   */
#include <rand55.h>   /* random generator by Fricke/Knuth                  */

  /* ========================================================================= */
  /*                                                                           */
  /*                        Public type definitions                            */
  /*                                                                           */
  /* ========================================================================= */

  typedef unsigned long lc_reorg_t;
  extern char *lc_types[];

#define LC_REORG_FORMAT "%Lu"

#define LC_DOUBLE 1
#define LC_ULONG 2
#define LC_LONG_DOUBLE 3
#define LC_LONG_ULONG 4

#define LC_REACTIVITY_TYPE LC_DOUBLE

#ifndef LC_REACTIVITY_TYPE
#define LC_REACTIVITY_TYPE LC_ULONG
#endif

#if (LC_REACTIVITY_TYPE==LC_DOUBLE)
  typedef double lc_reactivity_t;
#define lc_reactivity_max DBL_MAX
#define lc_reactivity_min DBL_MIN
#define LC_FORMAT "%lg"
#define LC_ROUND_OFF_ERRORS
#define lc_t_rand(r) (drand55()*(r))
  //#warn "LC_REACTIVITY_TYPE==LC_DOUBLE"
#endif

#if (LC_REACTIVITY_TYPE==LC_LONG_ULONG)
  typedef unsigned long long lc_reactivity_t;
#define lc_reactivity_max 18446744073709551615UL
#define lc_reactivity_min 1
#define LC_FORMAT "%Lu"
#define lc_t_rand(r) lrand55(r)
  //#warn "LC_REACTIVITY_TYPE==LC_LONG_ULONG"
#endif

#if (LC_REACTIVITY_TYPE==LC_LONG_DOUBLE)
  typedef unsigned long lc_reactivity_t;
#define lc_reactivity_max ULONG_MAX
#define lc_reactivity_min 1
#define LC_FORMAT "%Lf"
#define lc_t_rand(r) (drand55()*(r))
  //#warn "LC_REACTIVITY_TYPE==LC_LONG_DOUBLE"
#endif

#if (LC_REACTIVITY_TYPE==LC_ULONG)
  typedef unsigned long lc_reactivity_t;
#define LC_REACTIVITY_TYPE LC_ULONG
#define lc_reactivity_max ULONG_MAX
#define lc_reactivity_min 1
#define LC_FORMAT "%lu"
#define lc_t_rand(r) lrand55(r)
  //#error "LC_REACTIVITY_TYPE==LC_ULONG"
#endif

  typedef struct LC_EVENT{
    void   *ued;               /* pointer to user's event descriptor */
    lc_reactivity_t r;   /* reactivity of the event            */
  } lc_event;

  /* Type of the LC' event descriptors (leds), representing individual
     events and being the elementary data of the LC package (elements
     of arrays F[] in Fricke).

     Pointer to user's event descriptor is void *, so it may be cast to any
     pointer type and each user may choose her/his own type of ued.

     The user-program needs to read element ued; it may read element r, but it
     MUST NOT assign to any element; the entries are maintained by the
     LC functions, so please KEEP YOUR HANDS OFF !!!

     For details, see lc.doc.*
  */

#define LC_DERIVED   lc_event * lc_ev
typedef struct LC_BASE{
  LC_DERIVED;
} lc_base;



  /* for lc_init */
extern void lc_tell_cell_that_event_moved(lc_event *lce);

  /* useful macros */
#define LC_GLOBAL_PTR() (&lc_g)
#define LC_GLOBAL_DEF lc_global lc_g

#define LC_MAIN_BEGIN(lcn,timescale) {lc_init(LC_GLOBAL_PTR(),(lcn),NULL,(timescale))
#define LC_MAIN_END lc_clear(LC_GLOBAL_PTR());}

#define LC_DRAW(type,source) lc_class *lc_c; lc_event *lc_e=lc_rand(LC_GLOBAL_PTR(),&lc_c); type * source=(type*)(lc_e->ued)
#define LC_UPDATE_DRAWN(source) (source)->lc_ev=lc_knownchange(LC_GLOBAL_PTR(),lc_c,lc_e,(source),(lc_reactivity_t)LC_REACTIVITY(source));

#define LC_UPDATE(name) (name)->lc_ev=lc_safechange(LC_GLOBAL_PTR(), (name)->lc_ev, (name), (lc_reactivity_t)LC_REACTIVITY(name));

#define LC_TIME_UNIT() (LC_GLOBAL_PTR()->time_scale/LC_GLOBAL_PTR()->r)
#define LC_TIME_STEP() (exp_rand55()*LC_TIME_UNIT())   /* draw time step */

#define LC_EVENT_BEFORE(time,now,time_step) ( (time)-(now)<(time_step))


  /*--------------------------------------------------------------------------*/

  typedef struct LC_CLASS{ lc_event        *bot, *top;
    lc_reactivity_t           r;
    struct LC_CLASS *next, *prev;
  } lc_class;

  /* Type of class descriptors, each describing one class, see Fricke&Wendt.

     A class L[z] (dual-logarithmic class) is a set of events e whose
     reactivities Q(e) yield to the inequality
     1/2 2^z <= Q(e) < 2^z
     and is represented as an array F[] of events.

     Elements:
     - bot : pointer to array F[], i.e. its first element
     - top : pointer to the first led "behind" this array, i.e. top does not
     point to a valid led, but serves as upper limit
     - r   : reactivity of the class
     - next: links each class to its successor in a list, which is
     (imperfectly) sorted by class reactivity (descending order)

     The type lc_class is made public only to enable function lc_rand to
     pass a pointer to a class descriptor to lc_knownchange via the user-
     program.
     Direct contact to these class descriptors should not be necessary,
     so please KEEP YOUR HANDS OFF !!!
  */

  /*--------------------------------------------------------------------------*/

  typedef struct LC_GLOBAL  {
		lc_class *cbeg, *cend, *first;
    int       min_class, num_classes;
    size_t    max_events;
    lc_reactivity_t    r;
		double time_scale;
#ifdef LC_ROUND_OFF_ERRORS
    lc_reactivity_t  eps;
#endif
    lc_reorg_t number_of_reorgs;
    size_t number_of_checks;
    void      (*event_moved)(lc_event *);
  } lc_global;

  /* The global data structure of LC containing all information required
     by LC' functions to operate.

     The user-program has to allocate memory for and provide a pointer to this
     structure, e.g. defining
     lc_global global_data, *global_data_ptr = &global_data;
     this pointer is passed to all LC functions as first argument.

     All entries are set by function lc_init and maintained by LC
     throughout program execution.  They are

     cbeg   : pointer to descriptor of the class with lowest reactivity, i.e.
     "beginning" of the array of class descriptors
     cend   : pointer to descriptor of an empty class, marking the upper
     limit of the array, i.e. valid pointers to classes are given
     by
     cbeg[n] < cend, 0 <= n ( < num_classes )
     first  : pointer to first element in linked list of class descriptors,
     i.e. class where lc_rand starts search
     min_class  : index of lowest class
     num_classes: number of class descriptors allocated, i.e. number of elements
     in array beginning at cbeg;
     event reactivities < 2^(min_class-1) are treated as zero,
     reactivities >= 2^(min_class+num_classes) cause an overflow
     max_events : number of LC event descriptors (leds) allocated, i.e.
     maximum number of events that can be handled
     r          : total reactivity of the system, 1/r is mean of the
     first passage time distribution of the Markoff-events
     eps        : lowest reactivity not treated as zero, i.e. 2^(min_class-1)
     number_of_reorgs: counts how often memory has been reorganized since
     initialization
     *event_moved    : pointer to function defined in user-program to update
     the link ued->led
     err_file        : name of file for error messages
     *err_proc       : pointer to function defined in user-program to "wrap
     things up" before exiting due to error in LC

     The element r is used by the user program to determine the time step
     according to Gillespie.
     The element number_of_reorgs may be read to monitor "memory performance".

     Any other contact to the global data should not be necessary,
     so please KEEP YOUR HANDS OFF !!!
  */

  /* ========================================================================= */
  /*                                                                           */
  /*                      Public function declarations                         */
  /*                                                                           */
  /* ========================================================================= */

  void lc_init(lc_global *lc, size_t num_leds, void (*event_moved)(lc_event *),double timescale);

  /* Task:
     - allocates memory for class and LC event descriptors
     - initializes elements of lc_global data structure
     - sets seed for random generator

     Arguments:
     - lc        : pointer to global data structure
     - num_leds   : number of LC event descriptors to allocate
     - rand_seed  : random seed, use 0L to take clock as seed
     - event_moved: (pointer to) function updating link ued->led

     Exits with error message if
     - range of reactivities to large
     - to few leds have been allocated per class
     - there was not enough memory available

     Called by:
     user-program

     Remark:
     MUST be called before any other LC routine !
  */

  /*--------------------------------------------------------------------------*/

  void lc_clear(lc_global *lc);

  /* Task:
     - returns memory allocated by lc_init() to operating system

     Arguments:
     - lc : pointer to global data structure

     Called by:
     user-program

     Remarks:
     - Should be called at the end of simulation
  */

  /*--------------------------------------------------------------------------*/

  lc_event *lc_enter(lc_global *lc, void *ued, lc_reactivity_t r);
  /*
    shortcut if the macro LC_REACTIVITY computes the reactivity
  */
#define LC_ENTER(lc_d) (lc_d)->lc_ev=lc_enter(LC_GLOBAL_PTR(),(lc_d),(lc_reactivity_t)LC_REACTIVITY((lc_d)))

  /* Task:
     - assigns an led to the event represented by ued and places it in a
     class according to its reactivity r; sets up link led->ued and
     stores reactivity in led

     Arguments:
     - lc : pointer to global data struture
     - ued : pointer to events descriptor in user-programs data structure
     - r   : reactivity of the event

     Returns:
     - pointer to led

     Exits with error message and error code 1, if
     - frexp() sets errno
     - r < 0
     - reactivity is out of range of classes
     - lc or ued are NULL
     - all leds allocated have been assigned already
     - memory reorganization failed

     Called by:
     user-program, lc_safechange

     Calls:
     lc_getclass, lc_store

     Remarks:
     - an event's ued must exist before it can be assigned a led
     - events MUST be assigned a led before they can be selected
     - lc_enter should be called by the user-program only once for each event
  */

  /*--------------------------------------------------------------------------*/

  lc_event *lc_rand(lc_global *lc, lc_class **re_class_ptr);

  /* Task:
     - select one event at random as described by Fricke&Wendt

     Arguments:
     - lc         : pointer to the global data structure
     - re_class_ptr: pointer to return value, see below

     Returns:
     - lc_rand    : pointer to the led of the event selected
     - re_class_ptr: pointer to pointer to the class descriptor of
     the class from which the event was selected,
     provided for use by lc_knownchange

     Exits with error message and error code 1, if
     - a non-existing class was selected
     - the class selected does not contain any events
     - no class was selected at all

     Called by:
     user-program


     Remarks:
     - as a side effect, lc_rand exchanges adjacent elements of the
     linked list of class descriptors if they are no longer in descen-
     ding order of class reactivity, so the list remains partially
     sorted
  */

  /*--------------------------------------------------------------------------*/

  lc_event *lc_knownchange(lc_global *lc, lc_class *events_class,
			     lc_event *led, void *ued, lc_reactivity_t r);

  /* Task:
     - move the event described by led/ued whose reactivity r has changed
     to the appropriate class if its current class is known;
     - update event's reactivity entry in led, class reactivities of all
     classes involved and total reactivity
     - reorganize memory if necessary

     Arguments:
     - lc          : pointer to global data structure
     - events_class : pointer to descriptor of event's current class
     - led, ued     : pointers to event's led/ued, respectively
     - r            : NEW reactivity of the event

     Returns:
     - NEW pointer to the event's led, NULL if r==0

     Exits with error message, if
     - round-off errors are detected
     - frexp() sets errno
     - r < 0
     - reactivity is out of range of classes
     - lc or ued are NULL
     - all leds allocated have been assigned already
     - memory reorganization failed

     Called by:
     user-program, lc_safechange, lc_unknownchange

     Calls:
     lc_delete, lc_getclass, lc_store

     Remarks:
     - led MUST BE != NULL, otherwise behavior is unpredictable;
     if led == NULL might occur, use lc_safechange !
     - is provided to move the event selected by lc_rand after execution
     - argument events_class is provided by lc_knownchange as re_class
     - does NOT update link ued -> led, since it is faster if the user program
     does it, e.g. by
     ued->led_ptr = lc_knownchange(lc, events_class, led, ued, r);
  */

  /*--------------------------------------------------------------------------*/

  lc_event *lc_safechange(lc_global *lc, lc_event *led, void *ued,
			    lc_reactivity_t r);

  /* Task:
     - move or enter the event described by led/ued whose NEW reactivity is r
     to the appropriate class
     - update event's reactivity entry in led, class reactivities of all
     classes involved and total reactivity
     - reorganize memory if necessary

     Arguments:
     - lc      : pointer to global data structure
     - led, ued : pointers to events led/ued, respectively
     - r        : NEW reactivity of the event

     Returns:
     - NEW pointer to the event's led, NULL if r==0

     Exits with error message, if
     - serious round-off errors are detected
     - frexp() sets errno
     - r < 0
     - reactivity is out of range of classes
     - lc or ued are NULL
     - all leds allocated have been assigned already
     - memory reorganization failed

     Called by:
     user-program

     Calls:
     lc_enter, lc_knownchange

     Remarks:
     - led == NULL will work correctly
     - does NOT update link ued -> led, since it is faster if the user program
     does it, e.g. by
     ued->led_ptr = lc_safechange(lc, led, ued, r);
  */


  /* ========================================================================= */
  /*                                                                           */
  /*          Private section: users should keep their HANDS OFF !!!           */
  /*                                                                           */
  /* ========================================================================= */

  void lc_delete(lc_global *lc, lc_class *events_class, lc_event *led);

  /* Task:
     - remove event from its class, reduce class and total reactivity
     accordingly

     Arguments:
     - lc         : pointer to the global data structure
     - events_class: ptr to descriptor of current class of event to be removed
     - led         : ptr to led of event

     Exits with error message, if
     - round-off errors are detected

     Called by:
     lc_knownchange

     Calls:
     lc->event_moved

     Remarks:
     - lc_delete does not verify that the event led is indeed a member
     of class events_class; if this is not the case, unpredictable
     things may happen!!!
  */

  /*--------------------------------------------------------------------------*/

  lc_class *lc_getclass(lc_global *lc, lc_reactivity_t r);

  /* Task:
     - determines the class that comprises the reactivity r

     Arguments:
     - lc : pointer to the global data structure
     - r   : reactivity

     Returns:
     - lc_getclass: pointer to the descriptor of the corresponding class

     Exits with error message, if
     - frexp() sets errno
     - r < 0
     - reactivity is out of range of classes

     Called by:
     lc_enter, lc_knownchange, lc_unknownchange


     Remarks:
     - if r < 2^(lc->min_class-1), the NULL pointer is returned
  */

  /*--------------------------------------------------------------------------*/

  void lc_reorg(lc_global *lc);

  /* Task:
     - reorganizes memory when a class gets overcrowded
     - after leds have been moved in memory, lc->event_moved is used
     to update the ued->led link
     - recalculates class and total reactivities to reduce round-off errors

     Arguments:
     - lc : pointer to the global data structure

     Exits with error message, if
     - all leds allocated have been assigned already
     - memory reorganization failed otherwise

     Called by:
     lc_store

     Calls:
     lc->event_moved
  */

  /*--------------------------------------------------------------------------*/

  lc_event  *lc_store(lc_global *lc, lc_class *events_class, void *ued,
			lc_reactivity_t r);

  /* Task:
     - place the event described by ued in the class given by events_class
     according to its NEW reactivity r
     - store event's, increase class, total reactivity by r
     - set link led->ued
     - reorganize memory if necessary

     Arguments:
     - lc         : pointer to the global data structure
     - events_class: pointer to descriptor of class in which to place event
     - ued         : pointer to event's ued
     - r           : NEW reactivity of the event

     Returns:
     - lc_store : pointer to the events led

     Exits with error message, if
     - lc or ued are NULL
     - all leds allocated have been assigned already
     - memory reorganization failed

     Called by:
     lc_enter, lc_knownchange

     Calls:
     lc_reorg

     Remarks:
     - lc_store does not verify if the event belongs to the class given;
     if it does not, this will lead to wrong results and might cause
     an error message by lc_rand after some time
     - does NOT update link ued->led
     - if events_class == NULL, the NULL pointer is returned
  */

  /*--------------------------------------------------------------------------*/

  lc_event *lc_unknownchange(lc_global *lc, lc_event *led, void *ued,
			       lc_reactivity_t r);

  /* Task:
     - move the event described by led/ued whose NEW reactivity is r to
     the appropriate class
     - update event's reactivity entry in led, class reactivities of all
     classes involved and total reactivity

     Arguments:
     - lc          : pointer to global data structure
     - led, ued     : pointers to events led/ued, respectively
     - r            : NEW reactivity of the event

     Returns:
     - NEW pointer to the event's led, NULL if r==0

     Exits with error message, if
     - round-off errors are detected
     - frexp() sets errno
     - r < 0
     - reactivity is out of range of classes
     - lc or ued are NULL
     - all leds allocated have been assigned already
     - memory reorganization failed

     Called by:
     not used in this version

     Calls:
     lc_getclass, lc_knownchange

     Remarks:
     - The same as lc_knownchange, except the current class of the event
     needs not to be specified.
     - led MUST BE != NULL, otherwise behavior is unpredictable;
     if led == NULL might occur, use lc_safechange !
     - does NOT update link ued -> led, since it is faster if the user program
     does it, e.g. by
     ued->led_ptr = lc_unknownchange(lc, led, ued, r);
  */

  /*
  ** lc_check: checks the integrity of all classes
  ** parameters:
  **             lc_g  : the global pointer
  **             index : a function returning the integer
  **                     of the void * pointer representing the lc_base and
  **             event : extracting its event
  */
  void lc_check(lc_global * lc_g, size_t(index)(void*),lc_event * (event)(void *));

#ifdef __cplusplus
  }
#endif
#endif

