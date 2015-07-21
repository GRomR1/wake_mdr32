#include "led.h"

void led_init (void)
{
	MDR_RST_CLK->PER_CLOCK				|=(1<<23);
	MDR_PORTC->OE									 =0x0003;
	MDR_PORTC->ANALOG							 =0x0003;
	MDR_PORTC->PWR								 =0x000F;
}

void led1_set (void)
{
	MDR_PORTC->RXTX|=(1<<1);
}

void led1_reset (void)
{
	MDR_PORTC->RXTX &= ~(1<<1);
}

void led0_set (void)
{
	MDR_PORTC->RXTX|=(1<<0);
}

void led0_reset (void)
{
	MDR_PORTC->RXTX &= ~(1<<0);
}
