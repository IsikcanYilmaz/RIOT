#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "animation_manager.h"
// #include "animation_manager.h"
// #include "animation_scroller.h"
#include "animation_canvas.h"
#include "addr_led_driver.h"
// #include "animation_sparkles.h"
// #include "pico/time.h"
#include <string.h>
// #include "usr_commands.h"

#include "clk.h"
#include "board.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "timex.h"
#include "ztimer.h"
#include "xtimer.h"
#include "periph/pwm.h"
#include "ws281x.h"
#include "thread.h"

#include "colorspace_interface.h"

// Neopixel driver objects
// ws281x_t neopixelHandle;
// uint8_t pixelBuffer[NEOPIXEL_SIGNAL_BUFFER_LEN];
// ws281x_params_t neopixelParams = {
// 	.buf = pixelBuffer,
// 	.numof = NUM_LEDS,
// 	.pin = NEOPIXEL_SIGNAL_GPIO_PIN
// };

// repeating_timer_t animationManUpdateTimer;
Animation_s *currentAnimation;
AnimationManState_e animationManState;
AnimationIdx_e targetAnimation; // for when we're waiting for the switching of another animation
// TODO could have animations time out as a failsafe?
static bool animationManInitialized = false;

Animation_s animations[ANIMATION_MAX] = {
	// [ANIMATION_SCROLLER] = {
	// 	.name = "scroller",
	// 	.init = AnimationScroller_Init,
	// 	.deinit = AnimationScroller_Deinit,
	// 	.start = AnimationScroller_Start,
	// 	.stop = AnimationScroller_Stop,
	// 	.update = AnimationScroller_Update,
	// 	.buttonInput = AnimationScroller_ButtonInput,
	// 	.usrInput = AnimationScroller_UsrInput,
	// 	.signal = AnimationScroller_ReceiveSignal,
	// 	.getState = AnimationScroller_GetState
	// },
	// [ANIMATION_SPARKLES] = {
	// 	.name = "sparkles",
	// 	.init = AnimationSparkles_Init,
	// 	.deinit = AnimationSparkles_Deinit,
	// 	.start = AnimationSparkles_Start,
	// 	.stop = AnimationSparkles_Stop,
	// 	.update = AnimationSparkles_Update,
	// 	.buttonInput = AnimationSparkles_ButtonInput,
	// 	.usrInput = AnimationSparkles_UsrInput,
	// 	.signal = AnimationSparkles_ReceiveSignal,
	// 	.getState = AnimationSparkles_GetState
	// },
	[ANIMATION_CANVAS] = {
		.name = "canvas",
		.init = AnimationCanvas_Init,
		.deinit = AnimationCanvas_Deinit,
		.start = AnimationCanvas_Start,
		.stop = AnimationCanvas_Stop,
		.update = AnimationCanvas_Update,
		.buttonInput = AnimationCanvas_ButtonInput,
		.usrInput = AnimationCanvas_UsrInput,
		.signal = AnimationCanvas_ReceiveSignal,
		.getState = AnimationCanvas_GetState
	},
};

static Animation_s * AnimationMan_GetAnimationByIdx(AnimationIdx_e idx)
{
	if (idx >= ANIMATION_MAX)
	{
		printf("Bad anim idx %d at %s\n", idx, __FUNCTION__);
		return NULL;
	}
	return &animations[idx];
}

void AnimationMan_Init(void)
{
	animationManInitialized = true;
}

void AnimationMan_ThreadHandler(void *arg)
{
	(void) arg;
	thread_t *thisThread = thread_get_active();
	const char *thisThreadName = thread_get_name(thisThread);
	// Main thread for animations. What it should be doing is:
	// Go through list of interruptions
	// Interact with the currently running animation
	// Display current animation
	while (true)
	{
		// printf("ANIM MAN THREAD %s INITIALIZED %d PARAMS: numof %d\n", thisThreadName, animationManInitialized, neopixelHandle.params.numof);
		// teststripts(&neopixelHandle);
		// ws281x_write(&neopixelHandle);
		ztimer_sleep(ZTIMER_USEC, 2 * US_PER_SEC);
	}
}

void AnimationMan_StartPollTimer(void)
{
	printf("%s\n", __FUNCTION__);
	// add_repeating_timer_ms(ANIMATION_UPDATE_PERIOD_MS, AnimationMan_PollCallback, NULL, &(animationManUpdateTimer));
}

void AnimationMan_StopPollTimer(void)
{
	printf("%s\n", __FUNCTION__);
	// cancel_repeating_timer(&(animationManUpdateTimer));
}

void AnimationMan_SetAnimation(AnimationIdx_e anim, bool immediately)
{
	if (anim >= ANIMATION_MAX)
	{
		printf("Bad anim idx %d to %s\n", anim, __FUNCTION__);
		return;
	}

	// TODO make this so that this sends a signal to the running animation which then does its cleanup and fades off
	// for now, it abruptly changes
	targetAnimation = anim;
	if (immediately)
	{
		// AnimationMan_StopPollTimer();
		// currentAnimation->deinit();
		// AddrLedDriver_Clear();
		// currentAnimation = &animations[targetAnimation];
		// currentAnimation->init(NULL);
		// animationManState = ANIMATION_MAN_STATE_RUNNING;
		// AnimationMan_StartPollTimer();
	}
	else
	{
		// currentAnimation->signal(ANIMATION_SIGNAL_STOP);
		animationManState = ANIMATION_MAN_STATE_SWITCHING;
	}

	printf("%s Setting animation to %s. %s\n", __FUNCTION__, animations[anim].name, immediately ? "Immediately" : "Signal sent");
}

void AnimationMan_TakeUsrCommand(uint8_t argc, char **argv)
{
	// if (!animationManInitialized)
	// {
	// 	return;
	// }
	// ASSERT_ARGS(2);
	// if (strcmp(argv[1], "set") == 0)
	// {
	// 	ASSERT_ARGS(3);
	// 	for (int i = 0; i < ANIMATION_MAX; i++)
	// 	{
	// 		if (strcmp(argv[2], animations[i].name) == 0)
	// 		{
	// 			AnimationMan_SetAnimation(i, false);
	// 			return;
	// 		}
	// 	}
	// }
	// else
	// {
	// 	currentAnimation->usrInput(argc-1, &argv[1]);
	// }
}

void AnimationMan_GenericGetSetValPath(EditableValueList_t *l, uint8_t argc, char **argv)
{
	// if (strcmp(argv[0], "setval") == 0)
	// {
	// 	ASSERT_ARGS(3);
	// 	bool ret = EditableValue_FindAndSetValueFromString(l, argv[1], argv[2]);
	// 	printf("%s set to %s %s\n", argv[1], argv[2], (ret) ? "SUCCESS" : "FAIL");
	// }
	// else if (strcmp(argv[0], "getval") == 0)
	// {
	// 	ASSERT_ARGS(1);
	// 	if (argc == 1)
	// 	{
	// 		EditableValue_PrintList(l);
	// 	}
	// 	else if (argc == 2)
	// 	{
	// 		bool isNumber = (argv[1][0] >= '0' && argv[1][0] <= '9');
	// 		if (isNumber)
	// 		{
	// 			uint16_t valIdx = atoi(argv[1]);
	// 			if (valIdx >= l->len)
	// 			{
	// 				printf("%s bad val idx %d!\n", __FUNCTION__, valIdx);
	// 				return;
	// 			}
	// 			printf("%d ", valIdx);
	// 			EditableValue_PrintValue(&(l->values[valIdx]));
	// 		}
	// 		else
	// 		{
	// 			EditableValue_t *ev = EditableValue_FindValueFromString(l, argv[1]);
	// 			if (ev)
	// 			{
	// 				printf("%d ", EditableValue_GetValueIdxFromString(l, argv[1]));
	// 				EditableValue_PrintValue(ev);
	// 			}
	// 		}
	// 	}
	// }
}
