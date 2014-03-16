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

#ifndef __LOGCLASSEVENT_HH__

#include "logclass.h"

#define LC_REACTIVITY(x) ((x)->reactivity())
#undef LC_GLOBAL_PTR
#define LC_GLOBAL_PTR() (&LCEvent::lc_g)

/*
** LCEvent is a C++ interface of the the logclasses defined in
** logclass.h
**
** It is only necessary to overload the reactivity() and the action()
** function to provide arbitrary action(), which are selected by the
** probability rate reactivity()
**
** The updateDrawn() and update() functions provide the correct entry
** and update mechanisms, updateDrawn offers a shortcut method to
** update the last drawn event and must be called before any other
** event is updated
**
*/
class LCEvent  :  public lc_base{ /* see the comments in logclass.h */
//class LCEvent {
 public:
 //  LC_DERIVED; /* see the comments in logclass.h */
  // private:
  /*
  **	global, i.e. static variables
  */
  static lc_event  * lc_e;
  static lc_global   lc_g;
  static lc_class  * lc_c;
 public:
  inline LCEvent(){
    lc_ev=NULL;
  }
  //	inline static lc_global & global() { return lc_g;}
  /*
  ** enter() puts the event into the logclass data structures
  ** correctly
  */
  inline void enter() {
    lc_reactivity_t r=reactivity();
    lc_ev=lc_enter(LC_GLOBAL_PTR(),this,r);
  }
  /*
  ** cleans the data structure and frees all memory
  */
  static void end();

  /*
  ** initializes the data structures and defines the timescale
  */
  static void begin(double timescale=1);

  /*
  ** return the exponentially distributed time step
  */
  static inline double timestep(){
    return LC_TIME_STEP();
  }
  /*
  ** tells the event to move
  */
  static void move(lc_event *);

  /*
  ** updates the last drawn event effectively.
  ** USE FIRST, BEFORE ANY UPDATE
  */
  inline void updateDrawn() {
    //printf("updateDrawn: %X\n",this);
    LC_UPDATE_DRAWN(this);
  }

  /*
  ** updates any event
  ** USE AFTER updateDrawn
  */
  inline void update() {
    //printf("update: %X\n",this);
    LC_UPDATE(this);
  }

  /*
  ** draws an event with a probability according to its reactivity
  */
  inline static LCEvent * draw() {
    lc_e=lc_rand(LC_GLOBAL_PTR(),&lc_c);
    return (LCEvent*)lc_e->ued;
  }
  /*
  ** what to do if drawn
  */
  virtual void action()=0;

  /*
  ** reactivity means reaction probality
  */
  virtual lc_reactivity_t reactivity() =0;

  inline static int numberOfClasses(){
    return LC_GLOBAL_PTR()->num_classes;
  }

  /*
  ** index of first class
  */
  inline static int firstClass(){
		//return LC_GLOBAL_PTR()->cbeg-LC_GLOBAL_PTR()->first;
    return LC_GLOBAL_PTR()->min_class;
  }

  /*
  ** number of elements in class
  */
  inline static int elementsInClass(int i){
    if(i<0 || i>=numberOfClasses())
      return 0;
    else {
      lc_class *ci=LC_GLOBAL_PTR()->cbeg+i;
      return ci->top-ci->bot;
    }
  }

  /*
  ** reactivity of class
  */
  inline static lc_reactivity_t classReactivity(int i){
    return (LC_GLOBAL_PTR()->cbeg+i)->r;
  }

  /*
  ** global reactivity
  */
  inline static lc_reactivity_t globalReactivity(){
    return LC_GLOBAL_PTR()->r;
  }

  /*
  ** setting and getting timescale
  */

  inline static double timescale(){
    return LC_GLOBAL_PTR()->time_scale;
  }

  inline static void timescale(double timescale){
    LC_GLOBAL_PTR()->time_scale=timescale;
  }

  static void step();

  static LCEvent *lc_stack[100];
  static LCEvent **lc_stackP;

  template <class T> class StackPtr {
  public:
    StackPtr(T& rt){ ptr=&rt;}
    StackPtr(T *  rt=NULL){ptr=rt;}
    inline ~StackPtr() {
      //  printf("LCEvent::StackPtr %X %X\n",lc_stackP,ptr);
      *lc_stackP=ptr; lc_stackP++;
      //printf("%X ",lc_stackP);fflush(NULL);
    }
    //		inline ~StackPtr() {*lc_stackP=ptr; lc_stackP++;}
    inline T* operator->() { return ptr; }
    inline T* operator=(T*rt) {return ptr=rt;}

    inline T* operator+=(size_t i) {return ptr+=i;}
    inline T* operator-=(size_t i) {return ptr-=i;}

    inline T* operator++() {return ptr++;}
    inline T* operator--() {return ptr--;}

    inline T* operator+(size_t i) {return ptr+i;}
    inline T* operator-(size_t i) {return ptr-i;}

    inline bool operator==(T*rt) {return ptr==rt;}
    inline bool operator!=(T*rt) {return ptr!=rt;}
    inline bool operator<(T*rt) {return ptr<rt;}
    inline bool operator>(T*rt) {return ptr>rt;}
  private:
    T* ptr;
  };
};

class LCEventCheck: public LCEvent{
 public:
  virtual size_t index()=0;
  virtual lc_event * event()=0;
  static size_t indexFunction(void * );
  static lc_event * eventFunction(void *);

  static void check();

};



#undef LC_REACTIVITY

#define __LOGCLASSEVENT_HH__
#endif
