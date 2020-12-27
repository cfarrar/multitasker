#ifndef TIME_SLICE_H
#define TIME_SLICE_H

#include "core.h"

// do the preliminaries and interrupt set vector
void time_slice_interrupt_init( void (*vector) (void) );

// start the 10ms timer
void time_slice_interrupt_start( void );

// enable/disable timer intterrupt
void time_slice_interrupt_enable( void );
void time_slice_interrupt_disable( void );

// returns true if timer timed out
ubyte time_slice_timed_out( void )

// do what ever is needed to prepare for next 10ms timeout - maybe nothing
void time_slice_interrupt_reset( void );

#endif
