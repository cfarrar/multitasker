#include "core.h"       // core defines needed by entire system
#include "mtutil.h"     // multi tasker prototypes
#include "mtint.h"      // machine interface prototypes for multi-tasker
#include "firstTask.h"   // prototypes for module contining main task
#include "timeSlice.h"  // time slice interrupt control

ubyte        multi_tasker_grabbed;
ubyte        task_switch_requested;
unsigned int remaining_slices;
ubyte        task_volenteered_suspend;
_task_cb *   current_task;
_task_cb *   queue_anchor;

void mt_set_slices( ubyte slices )
{
    ubyte isr = mt_get_interrupt_status();
    signed long difference;

    difference = slices - current_task->slices;
    current_task->slices = slices;
    difference += remaining_slices;
    if ( difference <= 0 ) mt_task_suspend();
    remaining_slices = difference;

    mt_set_interrupt_status( isr );
}

ubyte mt_get_slices( void )
{
    return current_task->slices;
}

void mt_task_suspend( void )
{
    ubyte isr = mt_get_interrupt_status();

    // indicate a volentary task suspension
    task_volenteered_suspend = TRUE;

    // do the suspension
    mt_suspend_execution();

    mt_set_interrupt_status( isr );
}

void mt_task_start(
    void          (*task) (void)
  , ubyte         slices
  , _task_cb *    task_handle
  , void *        stack_pointer
  , unsigned int  task_id
)
{
    ubyte isr = mt_get_interrupt_status();

    // prepare the task control block
    task_handle->slices = slices;
    task_handle->task_id = task_id;
    task_handle->stack_pointer = stack_pointer;
    task_handle->next_tcb = NULL;

    // make initial stack image with new task's context
    // reset to first instruction of task
    mt_prepare_stack_image( &task_handle->stack_pointer, task );

    // place the task in the task queue for execution
    // if there is currently no task in the queue then
    if ( queue_anchor == NULL )
    {
		// initiate the queue
        task_handle->next_tcb = task_handle;
        queue_anchor = task_handle;
        current_task = task_handle;
	}
    else
    {
        // put task in of queue
		task_handle->next_tcb = queue_anchor->next_tcb;
        queue_anchor->next_tcb = task_handle;
        queue_anchor = task_handle;
    }

    mt_set_interrupt_status( isr );
}

void mt_init( void )
{
    // start up the time slice timer
    // this enables the pre-emptive mode
    time_slice_interrupt_start();
    time_slice_interrupt_enable();
}

void mt_grab( void )
{
    multi_tasker_grabbed = TRUE;
}

void mt_free( void )
{
    multi_tasker_grabbed = FALSE;
    if ( task_switch_requested )
    {
        mt_suspend_execution();
    }
}

C_task void main( void )
{
	// initiate the task queue with no tasks in it
    queue_anchor = NULL;
    current_task = NULL;

    // the following line bootstrapps the system into running the main
    // task. Main task is un-aware at start that it is a task and thinks
    // that it is starting the multi tasker.  In reality it only switches
    // the multi tasker from a strictly cooperative to a blended system
    // that mixes pre-emption with cooperation.  The main task starts the
    // task slice timer.
    mt_task_start(
        first_task
      , 1
      ,&first_task_handle
      ,&first_task_stack[FIRST_TASK_STACK_SIZE - 1]
      , FIRST_TASK_FLAG
    );

    // init time slice timer and set its interrupt vector
    time_slice_interrupt_init( mt_suspend_execution );

    // begin running tasks
    do
    {

        // if there are tasks in the queue to execute then
        if ( queue_anchor != NULL )
        {
            // get next task to execute from beginning of queue
            current_task = current_task->next_tcb;

            remaining_slices = current_task->slices;

            // if last task volenteered to give up its time and its time
            // is not up then give next task one extra slice so it gets at
            // least one full slice to execute
            if ( task_volenteered_suspend )
            {
              ++remaining_slices;
            }

            // clear all possible events that can occure because of
            // task execution
            multi_tasker_grabbed = FALSE;
            task_switch_requested = FALSE;
            task_volenteered_suspend = FALSE;

            // go run task until it gives up or is out of time
            do
            {
                // shut down interrupts before enabling timer interrupt to prevent
                // timer interrupt durring mt execution.  If the task that is running
                // has enabled interrupts then interrupts will be re-enabled by
                // my_jump_into_task just as it begins execution of the task.
                disable_interrupt();
                time_slice_interrupt_enable();

                // go run task for a while
                mt_jump_into_task( &current_task->stack_pointer );

                // if 10 ms heartbeat timer has timed out then
                time_slice_interrupt_disable();
                enable_interrupt();

                // check if left task because of timer interrupt
                if ( time_slice_timed_out() )
                {
                    // if left task because of timer interrupt then must reset the
                    // time slice interrupt timer.
                    disable_interrupt();
                    time_slice_interrupt_reset();
                    enable_interrupt();
                }

                // if multi tasker is not to allow exit from above task then
                // note that either a timeout occured or that the task
                // tried to exit with a task suspend and continue executing it
                if ( multi_tasker_grabbed )
                {
                    task_switch_requested = TRUE;
                    continue;
                }

                // if the multi tasker is allowed to exit above task and
                // time has run out or task suspends it self then exit loop and
                // get next task to execute
                if ( --remaining_slices == 0 ) break;
                if ( task_volenteered_suspend ) break;
            }
            while ( FOREVER );
        }
        else
        {
            // loop until watchdog resets
            disable_interrupt();
            while ( FOREVER )
            {
                halt();
            }
        }
    }
    while ( FOREVER );
}
