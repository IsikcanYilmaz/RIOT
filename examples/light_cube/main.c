#include <stdio.h>

#include "clk.h"
#include "board.h"
#include "periph_conf.h"
#include "timex.h"
#include "ztimer.h"

#include "periph/pwm.h"

#include "thread.h"

char blink_threadStack[THREAD_STACKSIZE_DEFAULT];

// static void delay(void)
// {
//     if (IS_USED(MODULE_ZTIMER)) {
//         ztimer_sleep(ZTIMER_USEC, 1 * US_PER_SEC);
//     }
//     else {
//         /*
//          * As fallback for freshly ported boards with no timer drivers written
//          * yet, we just use the CPU to delay execution and assume that roughly
//          * 20 CPU cycles are spend per loop iteration.
//          *
//          * Note that the volatile qualifier disables compiler optimizations for
//          * all accesses to the counter variable. Without volatile, modern
//          * compilers would detect that the loop is only wasting CPU cycles and
//          * optimize it out - but here the wasting of CPU cycles is desired.
//          */
//         uint32_t loops = coreclk() / 20;
//         for (volatile uint32_t i = 0; i < loops; i++) { }
//     }
// }

bool on = false;

void *blink_threadHandler(void *arg)
{
	(void) arg;
	thread_t *thisThread = thread_get_active();
	const char *thisThreadName = thread_get_name(thisThread);
	while (1) {
		if (on)
		{
			LED0_ON;
		}
		else
		{
			LED0_OFF;
		}
		on = !on;
		printf("TEST %d thread %s\n\b", on, thisThreadName);
		ztimer_sleep(ZTIMER_USEC, 1 * US_PER_SEC);
	}

}

int main(void)
{
	kernel_pid_t blink_threadId = thread_create(
		blink_threadStack,
		sizeof(blink_threadStack),
		THREAD_PRIORITY_MAIN - 1,
		THREAD_CREATE_STACKTEST,
		blink_threadHandler,
		NULL,
		"blink_thread"
	);
	for (;;) {
		printf("Main created blink thread %d\r\n", blink_threadId);
		ztimer_sleep(ZTIMER_USEC, 2 * US_PER_SEC);
	}

	return 0;
}
