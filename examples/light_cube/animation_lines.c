#include "animation_lines.h"
#include "addr_led_driver.h"
#include "color.h"
#include "colorspace_interface.h"
// #include "pico/rand.h"
#include "visual_util.h"
#include "editable_value.h"
#include "usr_commands.h"
// #include "hardware/timer.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define DEFAULT_H 0.00
#define DEFAULT_S 0.80
#define DEFAULT_V 0.80

#define DEFAULT_RANDOM_UPPER_LIM_H 360.0 // 345.0
#define DEFAULT_RANDOM_UPPER_LIM_S 1.0
#define DEFAULT_RANDOM_UPPER_LIM_V 1.0

#define DEFAULT_RANDOM_LOWER_LIM_H 0.0 // 250.0
#define DEFAULT_RANDOM_LOWER_LIM_S 0.8
#define DEFAULT_RANDOM_LOWER_LIM_V 0.8

// #define DEFAULT_SPARK_MODE SPARKLES_MODE_DROPS
// #define DEFAULT_COLOR_MODE SPARKLES_COLOR_RANDOM
// #define DEFAULT_BURST_MODE SPARKLES_BURST_RANDOM

// #define DEFAULT_SPARKLE_CHANCE_PERCENT 15 
// #define DEFAULT_NUM_SPARKLES_PER_BURST 1
// #define DEFAULT_BURST_PERIOD 100 // 1000ms

#define DEFAULT_ITER_UNTIL_HUE_CHANGE 5

#define DEFAULT_PIXEL_H_CHANGE_PER_ITER 0
#define DEFAULT_PIXEL_S_CHANGE_PER_ITER 0.0
#define DEFAULT_PIXEL_V_CHANGE_PER_ITER -0.1

#define DEFAULT_NUM_COLORS 10
#define MAX_COLORS 10

#define DEFAULT_FADE_DIFF 0.01

// 
static uint16_t iter = 0;
static uint64_t lastBurstTimestampMs = 0;

static uint8_t numColors = DEFAULT_NUM_COLORS; // editable
static Color_t colorArr[MAX_COLORS];
static Color_t *currColor = &colorArr[0];
static uint8_t colorIdx = 0;

// static LinesSparkMode_e sparkMode = DEFAULT_SPARK_MODE;
// static LinesColorMode_e colorMode = DEFAULT_COLOR_MODE;
// static LinesBurstMode_e burstMode = DEFAULT_BURST_MODE;

static double randomLowerLimH = DEFAULT_RANDOM_LOWER_LIM_H;
static double randomUpperLimH = DEFAULT_RANDOM_UPPER_LIM_H;

static double randomLowerLimS = DEFAULT_RANDOM_LOWER_LIM_S;
static double randomUpperLimS = DEFAULT_RANDOM_UPPER_LIM_S;

static double randomLowerLimV = DEFAULT_RANDOM_LOWER_LIM_V;
static double randomUpperLimV = DEFAULT_RANDOM_UPPER_LIM_V;

static uint16_t iterUntilChange = DEFAULT_ITER_UNTIL_HUE_CHANGE;

static double hChange = DEFAULT_PIXEL_H_CHANGE_PER_ITER;
static double sChange = DEFAULT_PIXEL_S_CHANGE_PER_ITER;
static double vChange = DEFAULT_PIXEL_V_CHANGE_PER_ITER;

static volatile AnimationState_e state = ANIMATION_STATE_UNINITIALIZED;

static uint8_t lineX = 0;
static uint8_t lineY = 0;
static Position_e lineHeadPos = SOUTH;

static uint16_t iterBuffer = 5;
static uint16_t loops = 0;

static EditableValue_t editableValues[] = 
{
	(EditableValue_t) {.name = "iterUntilChange", .valPtr = (union EightByteData_u *) &iterUntilChange, .type = UINT16_T, .ll.u16 = 1, .ul.u16 = 0xffff},
	(EditableValue_t) {.name = "numColors", .valPtr = (union EightByteData_u *) &numColors, .type = UINT8_T, .ll.u8 = 0, .ul.u8 = MAX_COLORS},
	(EditableValue_t) {.name = "randomLowerLimH", .valPtr = (union EightByteData_u *) &randomLowerLimH, .type = DOUBLE, .ll.d = 0.00, .ul.d = 360.00},
	(EditableValue_t) {.name = "randomUpperLimH", .valPtr = (union EightByteData_u *) &randomUpperLimH, .type = DOUBLE, .ll.d = 0.00, .ul.d = 360.00},
	(EditableValue_t) {.name = "randomLowerLimS", .valPtr = (union EightByteData_u *) &randomLowerLimS, .type = DOUBLE, .ll.d = 0.00, .ul.d = 1.00},
	(EditableValue_t) {.name = "randomUpperLimS", .valPtr = (union EightByteData_u *) &randomUpperLimS, .type = DOUBLE, .ll.d = 0.00, .ul.d = 1.00},
	(EditableValue_t) {.name = "randomLowerLimV", .valPtr = (union EightByteData_u *) &randomLowerLimV, .type = DOUBLE, .ll.d = 0.00, .ul.d = 1.00},
	(EditableValue_t) {.name = "randomUpperLimV", .valPtr = (union EightByteData_u *) &randomUpperLimV, .type = DOUBLE, .ll.d = 0.00, .ul.d = 1.00},
	(EditableValue_t) {.name = "hChange", .valPtr = (union EightByteData_u *) &hChange, .type = DOUBLE, .ll.d = -360.00, .ul.d = 360.00},
	(EditableValue_t) {.name = "sChange", .valPtr = (union EightByteData_u *) &sChange, .type = DOUBLE, .ll.d = -1.00, .ul.d = 1.00},
	(EditableValue_t) {.name = "vChange", .valPtr = (union EightByteData_u *) &vChange, .type = DOUBLE, .ll.d = -1.00, .ul.d = 1.00},
	// (EditableValue_t) {.name = "burstMode", .valPtr = (union EightByteData_u *) &burstMode, .type = UINT8_T, .ll.u8 = 0, .ul.u8 = SPARKLES_BURST_MAX},
	// (EditableValue_t) {.name = "burstPeriod", .valPtr = (union EightByteData_u*) &burstPeriod, .type = UINT32_T, .ll.u16 = 1, .ul.u16 = 0xffff},
	// (EditableValue_t) {.name = "burstChance", .valPtr = (union EightByteData_u *) &burstChance, .type = UINT8_T,  .ll.u8 = 0, .ul.u8 = 100},
	// (EditableValue_t) {.name = "burstSize", .valPtr = (union EightByteData_u *) &burstSize, .type = UINT8_T,  .ll.u8 = 0, .ul.u8 = NUM_LEDS},
};
static EditableValueList_t editableValuesList = {.name = "lines", .values = &editableValues[0], .len = sizeof(editableValues)/sizeof(EditableValue_t)};

// uint8_t numVacancies = NUM_LEDS;
// uint8_t vacancyIndexes[NUM_LEDS];

static Color_t GenerateRandomColor(void)
{
	double h = fmod(rand(), (randomUpperLimH - randomLowerLimH + 1)) + randomLowerLimH;
	double s = (double) randomLowerLimS + fmod((rand() % 1000) / 100, (randomUpperLimS - randomLowerLimS + 0.01));
	double v = (double) randomLowerLimV + fmod((rand() % 1000) / 100, (randomUpperLimV - randomLowerLimV + 0.01));
	// printf("%f %f %f\n", h, s, v);
	Color_t c = Color_CreateFromHsv(h, s, v);
	// Color_PrintColor(c);
	return c;
}

static void InitColors(void)
{
	for (uint8_t i = 0; i < MAX_COLORS; i++)
	{
		colorArr[i] = GenerateRandomColor();
	}
}

static void IterThruColors(void)
{
	colorIdx = (colorIdx + 1) % numColors;
	colorArr[colorIdx] = GenerateRandomColor();
}

static void SetCurrColorRandomly(void)
{
	if (numColors > 1)
	{
		currColor = &colorArr[rand() % numColors];
	}
}

static void RunningAction(void)
{
	iter++;
	if (iter % iterBuffer != 0)
	{
		return;
	}

	Pixel_t *head = AddrLedDriver_GetPixelInPanel(lineHeadPos, lineX, lineY);
	AddrLedDriver_SetPixelRgbInPanel(lineHeadPos, lineX, lineY, currColor->red, currColor->green, currColor->blue);
	AddrLedDriver_SetPixelRgbInPanel(lineHeadPos, lineX, lineY+2, currColor->red, currColor->green, currColor->blue);

	// AddrLedDriver_SetPixelRgbInPanel((lineHeadPos + 2) % TOP, lineX, lineY, currColor->red, currColor->green, currColor->blue);
	// AddrLedDriver_SetPixelRgbInPanel((lineHeadPos + 2) % TOP, lineX, lineY+2, currColor->red, currColor->green, currColor->blue);

	lineX++;
	if (lineX >= NUM_LEDS_PER_PANEL_SIDE)
	{
		lineX = 0;
		lineHeadPos = (lineHeadPos+1) % TOP;

		if (lineHeadPos == SOUTH) // We looped
		{
			loops++;
			if (loops % iterUntilChange)
			{
				// SetCurrColorRandomly();
				IterThruColors();
			}
		}
	}
	Visual_IncrementAllByHSV(hChange,sChange,vChange);
}

static void FadeOffAction(void)
{
	// If we're stopping, fade off all LEDs. Check everytime if all LEDs are off
	Visual_IncrementAllByHSV(0,0,-DEFAULT_FADE_DIFF);
	if (Visual_IsAllDark())
	{
		state = ANIMATION_STATE_STOPPED;
		printf("Fade off done state %d\n", state);
	}
}

bool AnimationLines_Init(void *arg)
{
	InitColors();
	AddrLedDriver_Clear();
	state = ANIMATION_STATE_RUNNING;
	printf("%s\n", __FUNCTION__);
	return true;
}

void AnimationLines_Deinit(void)
{
	// TODO when/if i end up using a dynamic allocator i'll do freeing here. for now, STOP basically means UNINITIALIZED
	state = ANIMATION_STATE_UNINITIALIZED;
}

void AnimationLines_Start(void)
{
}

void AnimationLines_Stop(void)
{
}

void AnimationLines_Update(void)
{
	switch(state)
	{
		case ANIMATION_STATE_STARTING:
		{
			state = ANIMATION_STATE_RUNNING; // TODO populate this area
		}
		case ANIMATION_STATE_RUNNING:
		{
			RunningAction();
			break;
		}
		case ANIMATION_STATE_STOPPING:
		{
			FadeOffAction();
			break;
		}
		case ANIMATION_STATE_STOPPED:
		{
			// NOP
			break;
		}
		default:
		{
			break;
		}
	}
}

void AnimationLines_ButtonInput(Button_e b, ButtonGesture_e g)
{
}

void AnimationLines_UsrInput(int argc, char **argv)
{
	ASSERT_ARGS(1);
	printf("Lines received usr input:");
	for (int i = 0; i < argc; i++)
	{
		printf(" %s", argv[i]);
	}
	printf("\n");
	AnimationMan_GenericGetSetValPath(&editableValuesList, argc, argv);
}

void AnimationLines_ReceiveSignal(AnimationSignal_e s)
{
	printf("%s signal received %d\n", __FUNCTION__, s);
	switch(s)
	{
		case ANIMATION_SIGNAL_START:
		{
			state = ANIMATION_STATE_STARTING;
			break;
		}
		case ANIMATION_SIGNAL_STOP:
		{
			state = ANIMATION_STATE_STOPPING;
			break;
		}
		default:
		{
			printf("%s bad signal %d\n", __FUNCTION__, s);
			break;
		}
	}
}

AnimationState_e AnimationLines_GetState(void)
{
	return state;
}
