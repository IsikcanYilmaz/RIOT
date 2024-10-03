#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "addr_led_driver.h"
#include "ws281x.h"

#include "ztimer.h"
#include "xtimer.h"
#include "ws281x.h"
#include "thread.h"

bool pixelChanged = true;

// WS2812B Related 

// Convert $byte into codes that WS2812B accepts, that we push out via PWM.
// 1 Byte converts into 8 bytes, so make sure $*codes pointer points to a block of memory that has 8 bytes allocated
static inline void ByteToCodes(uint8_t byte, uint16_t *codes)
{
}

// Convert a Pixel_t object $p into codes that WS2812B accepts. 
// 1 Byte converts into 8 bytes, a Pixel_t is 3 bytes so make sure $*packet points to a block of memory that has 24 bytes allocated
static inline void PixelToPacket(Pixel_t *pixel, PixelPacket_t *packet)
{
}


void AddrLedDriver_Init(void)
{
	// TODO initialize datastructures
	
	// Initialize RMT module
	// uint8_t channel = _rmt_channel(dev);
	
}

void AddrLedDriver_Test(void)
{

}

void AddrLedDriver_DisplayStrip(AddrLedStrip_t *l)
{
	PixelPacket_t *pixelPacketBufferPtr = (PixelPacket_t *) (l->pixelPacketBuffer);
  for (int i = 0; i < l->numLeds; i++)
  {
    Pixel_t *currPixel = &(l->pixels[i]);
    PixelPacket_t *currPixelPacket = pixelPacketBufferPtr + i;
    PixelToPacket(currPixel, currPixelPacket);
  }
	// dma_channel_transfer_from_buffer_now(dma_chan, pixelPacketBufferPtr, sizeof(PixelPacket_t) * NUM_LEDS);
	// dma_channel_wait_for_finish_blocking(dma_chan);
	// pwm_set_gpio_level(ADDR_LED_SIGNAL_GPIO_PIN, 0);
	pixelChanged = false;
}
