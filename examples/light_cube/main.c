#include <stdio.h>

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

	//
	uint8_t numPixels = (4*4*5);
	//
	// // NeoPixel strip
	ws281x_t neopixel;
	uint8_t neo_state[numPixels * sizeof(color_rgb_t)];
	ws281x_params_t neo_params = {
		.buf = neo_state,
		.numof = numPixels,
		.pin = GPIO_PIN(0, 9),
	};
	if(ws281x_init(&neopixel, &neo_params) != 0) {
		printf("failed to initialize NeoPixel\n");
		return 3;
	}
	else 
	{
		printf("initialized NeoPixel\n");
	}

	// xtimer_ticks32_t last_wakeup = xtimer_now();
	printf("Starting NeoPixel loop\n");
	while(true) {
		color_rgb_t neo_color = {0, 0, 0};
		for(int i = 0; i < neo_params.numof; i++)
			ws281x_set(&neopixel, i, neo_color);
		ws281x_write(&neopixel);
		// xtimer_periodic_wakeup(&last_wakeup, 1000 * US_PER_MS);
		ztimer_sleep(ZTIMER_USEC, 5 * US_PER_SEC);
		printf("NEOPIXEL LOOP\n");
	}

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
