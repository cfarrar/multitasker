#ifndef MT_INT_H
#define MT_INT_H

#include "core.h"

void mt_prepare_stack_image( ubyte **stack_pointer, void (*task) (void) );
void mt_jump_into_task( ubyte **stack_pointer );

void mt_suspend_execution( void );

ubyte mt_get_interrupt_status( void );
void mt_set_interrupt_status( ubyte status );

#endif
