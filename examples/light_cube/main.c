#include <stdio.h>

#include "animation_manager.h"
#include "clk.h"
#include "board.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "timex.h"
#include "ztimer.h"
#include "xtimer.h"

#include "addr_led_driver.h"

#include "periph/pwm.h"

#include "ws281x.h"

#include "thread.h"


char blink_threadStack[THREAD_STACKSIZE_DEFAULT];
char animationMan_threadStack[THREAD_STACKSIZE_DEFAULT];

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
		printf("TEST %d thread %s\r\n", on, thisThreadName);
		ztimer_sleep(ZTIMER_USEC, 1 * US_PER_SEC);
	}
}

void readPixelBuffer(ws281x_t *neopixel)
{
	
}

int main(void)
{
	printf("MAIN\n");

	// AnimationMan_Init();
	// kernel_pid_t animationMan_threadId = thread_create(
	// 	animationMan_threadStack,
	// 	sizeof(animationMan_threadStack),
	// 	THREAD_PRIORITY_MAIN - 1,
	// 	THREAD_CREATE_STACKTEST,
	// 	AnimationMan_ThreadHandler,
	// 	NULL,
	// 	"animationman_thread"
	// );
	
	ztimer_sleep(ZTIMER_USEC, 4 * US_PER_SEC);
	AddrLedDriver_Init();
	
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
		printf("MAIN\n");
		AddrLedDriver_Test();
		ztimer_sleep(ZTIMER_USEC, 1 * US_PER_SEC);
	}

	return 0;
}
