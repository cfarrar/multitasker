#ifndef CORE_H
#define CORE_H

typedef ubyte unsigned char;

#define FOREVER (1)
#define FALSE   (0)
#define TRUE    (!FALSE)

typedef struct _tcb
{
  struct _tcb  *next_tcb;
  unsigned int  task_id;
  ubyte         slices;
  ubyte        *stack_pointer;
} _task_cb;

// these will have to be replaced with machine/compiler specific
intrinsic void enable_interrupt( void );
intrinsic void disable_interrupt( void );

intrinsic void halt( void );

// the following means dont waste stack space on saving registers
intrinsic C_task

#endif
