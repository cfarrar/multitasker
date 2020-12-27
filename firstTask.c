#include "core.h"
#include "mtutil.h"
#include "mtint.h"

unsigned first_task_stack_fence;
ubyte    first_task_stack[FIRST_TASK_STACK_SIZE];
_task_cb first_task_handle;

C_task void first_task( void )
{
    enable_interrupt();

    // only main task does this
    mt_init();

    // start other tasks maby???


    do
    {

        // do some narly stuff


    } while ( FOREVER );
}
