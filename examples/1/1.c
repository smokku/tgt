/* ----------------------------------------------------------------------------
**      1.c    TGT Usage Example
** ----------------------------------------------------------------------------
**      TGT                                       TextGUIToolkit Library
**      Copyright (C)   2000-2001       Tomasz Sterna and Mateusz Golicz
**
**      LGPL license
**
** ----------------------------------------------------------------------------
**      MAINTAINER      Mateusz Golicz
**
**      Log:
**      
**
*/

/* This is the Thing that we include while using tgt */
#include "tgt.h"

/* this is only for #define NULL ... ;) */
#include <stdio.h>
#include <stdlib.h>

int quit_fn(struct tgt_object * obj, int q);

struct tgt_object * desktop;
    /* this structure describes all controls in tgt ... example controls are:
       desktop (root object), window, button, checkbox, label, etc. */
    /* in this case we'll use it to store the root object */
int main(int argc, char ** argv)
{
    struct tgt_object * window;
    /* and this variable will store our window */
    struct tgt_object * button;


    desktop = tgt_start(NULL);
    /* this function initializes tgt (see reference docs for exact description) */
    /* and returns the root object */

    window = tgt_createobject(TGT_CLASS_WINDOW, (tgtt[])
		{TGTT_WINDOW_TITLE, (tgtt) "Hello, world!",
		 TGTT_XS, 30,
		 TGTT_YS, 5,
		 TGTT_END,0});		 
    /* this function creates an object ... in fact, many functions, such
       as tgt_start() are just a frontend to it ... 
       
       the first argument is class ID number, like TGT_CLASS_WINDOW, TGT_CLASS_DESKTOP,
       TGT_CLASS_BUTTON, etc.
       
       as a second argument you can pass a taglist.
       tgt specifies all object capabilities by tags. each tag has its ID,
       there are global tgt tags, and tags valid only for specified object
       types .. eg. TGTT_XS is the object width, TGTT_BUTTON_CAPTION is a
       string that is displayed inside a button object... tags can be set
       using tgt_set(), read using tgt_get() and passed in a taglist while
       object creation. by the term 'taglist' we understand an array of
       pairs: <tag key ID>, <tag value> casted to type tgtt, and ended by
       TGTT_END empty tag.
       */

   
    tgt_set(window, TGTT_X, (tgt_getterminal()->x_size - 30) / 2);

    /*   the tgt_getterminal() returns a tgt terminal description pointer, to
       the last opened terminal. (in our program we did it in tgt_start())
       there is some useful information inside this struct, eg. the terminal
       size ...
    */
    /* this code sets TGTT_X tag (which is object X position) 
       in object described by window to  (tgt_getterminal()->x_size - 30) / 2
       The x_size field in terminal description structure specifies
       ofcourse the terminal width - so, because our window is 30 characters
       wide, this call will allow it to be beautyfully centered :) */
    tgt_set(window, TGTT_Y, (tgt_getterminal()->y_size - 5) / 2);

    button = tgt_createobject(TGT_CLASS_BUTTON, (tgtt[])
		{TGTT_BUTTON_CAPTION, (tgtt) "[OK, that's great]",
		 TGTT_CALLBACK, (tgtt) quit_fn,
		     /* callback is the function, that is called by tgt,
		        when something happens with an object. the moment
			it'll be called is totally class-dependent, for
			instance in case of button - it's called whenever
			the button is hit by enter or space, in case of
			window it's called whenever an unknown key arrives
			(which can be used to implement hotkeys) etc. */
		 TGTT_X, 3,
		 TGTT_Y, 2, /* X and Y positions are relative to parent object ... */
		 TGTT_END,0});		 

    /* ... of course at the creation time, an object does not have a parent */
    
    tgt_link(button, window);
    /* ... but by using this function, we attached object pointed by 'button' to
       object pointed by 'window' ... */

    tgt_link(window, desktop);
    /* and here we attach window to our desktop (root) object */
    
    tgt_refresh(desktop);
    /* tgt_refresh does not only refresh given object, but also
       all its children ... */
       
    tgt_waitkeys(desktop);
    /* tgt_waitkeys waits for input events which are received by given
       object and its children. this function NEVER returns. */
    return(0);
    /* .. but we add the above to avoid a warning while compiling with gcc :) */
}

/* this function will be called when user presses the button, because
   we specified it as a callback when we were creating that object */
int quit_fn(struct tgt_object * obj, int q)
/* each callback has to match this prototype .. obj is the object, that
   is sending the message, and q is an additional parameter, which value
   is class-dependent. in case of buttons, you should ignore it */
{
    tgt_destroyobject(desktop);
    /* this also destroys all object children */
    
    exit(0);
}
