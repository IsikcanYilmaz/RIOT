#include <stdint.h>
#include <assert.h>
#include "addr_led_driver.h"
#include "ws281x.h"

/* Stolen from RIOT OS ws281x driver */

#include "esp_private/esp_clk.h"
#include "hal/cpu_hal.h"
#include "soc/rtc.h"

// TODO add ifdef to enable RMT
// Below includes for RMT support. RMT is an esp32 hw module that we'll use to
// output our arbitrary addr led signal.
#include "esp_intr_alloc.h"
#include "driver/rmt.h"
#include "hal/rmt_types.h"
#include "hal/rmt_ll.h"
#include "soc/rmt_struct.h"

// RMT related
// static uint32_t _ws281x_one_on;
// static uint32_t _ws281x_one_off;
// static uint32_t _ws281x_zero_on;
// static uint32_t _ws281x_zero_off;
//
// static uint8_t _rmt_channel(ws281x_t *dev)
// {
// 	for (unsigned i = 0; i < RMT_CH_NUMOF; i++) {
// 		if (rmt_channel_config[i].gpio == dev->params.pin) {
// 			return rmt_channel_config[i].channel;
// 		}
// 	}
// 	assert(0);
// }


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
	dma_channel_transfer_from_buffer_now(dma_chan, pixelPacketBufferPtr, sizeof(PixelPacket_t) * NUM_LEDS);
	dma_channel_wait_for_finish_blocking(dma_chan);
	pwm_set_gpio_level(ADDR_LED_SIGNAL_GPIO_PIN, 0);
	pixelChanged = false;
}
