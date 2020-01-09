#include <msp430.h>

#define LED_RED			BIT0			// The Red LED is on P1.0

int main(void) {
	WDTCTL = WDTPW + WDTHOLD;

	BCSCTL1 = CALBC1_1MHZ;			// Tell the 430 to run at a calibrated
	DCOCTL = CALDCO_1MHZ;			// Clk speed of 1MHz

	P1DIR = LED_RED;		// We want outputs on Red and Green LEDs only
	P1OUT = 0;						// Start with both LEDs off

	while(1)
	{
		__delay_cycles(3000000);		// Delay for 1/2 a second (clock is at a known 1MHz)
		P1OUT ^= LED_RED;			// Toggle the red LED
	}
}
