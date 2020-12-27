#ifndef FIRST_TASK_H
#define FIRST_TASK_H

#include "core.h"

#define FIRST_TASK_FLAG      (0)
#define FIRST_TASK_STACK_SIZE (1060)

extern unsigned first_task_stack_fence;
extern ubyte    first_task_stack[ FIRST_TASK_STACK_SIZE ];
extern _task_cb first_task_handle;

void first_task( void );

#endif
