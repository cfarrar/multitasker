#ifndef MT_UTIL_H
#define MT_UTIL_H

#include "core.h"

void mt_set_slices( ubyte slices );

ubyte mt_get_slices( void);

void mt_task_halt( void );

void mt_task_start( 
    void (*task) (void)
  , ubyte slices
  , _task_cb *task_handle
  , void *stack
  , unsigned int task_id
);

void mt_task_suspend( void );

void mt_init( void );

void mt_grab( void );
void mt_free( void );

#endif
