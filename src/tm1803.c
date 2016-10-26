
#include "mraa.h"
#include <stdio.h>
#include <unistd.h>
#include "tm1803.h"

//our global data_pin variable will be used universally as the output pin on the edison.
mraa_gpio_context data_pin = NULL;

/**
 * Initialize the data pin and memory mapping etc...
 */
int tm1803_init() {
	//change this to whatever pin you'd like to use for data
	data_pin = mraa_gpio_init(0); //this is J17-1 on the intel breakout board

	// set the pin as output
	if (mraa_gpio_dir(data_pin, MRAA_GPIO_OUT) != MRAA_SUCCESS) {
		fprintf(stderr, "Can't set digital pin as output, exiting");
		return MRAA_ERROR_UNSPECIFIED;
	};

	//we're using memory mapped access so that we can write fast enough for the 1803's tight timing
	if(mraa_gpio_use_mmaped(data_pin,1)) {
		fprintf(stderr, "Unable to initialize memory mapped GPIO");
		return MRAA_ERROR_UNSPECIFIED;
	}

	return 0;
}

/**
 * Write 24 bit color string out to the tm1803.
 * The 1803 wants 24 bits, in R,G,B order with MSB first.
 * This function is intentionally long and repetitive and doesn't call out to other functions
 * because of the extremely sensitive timing of the 1803. I don't trust the compiler to properly
 * inline the function calls, even with 'inline'
 *
 * 0 code:
 *
 * ---------      T0L
 * |  T0H  |________________|
 *
 * 1 code:
 *
 * --------------    T1L
 * |  T1H       |___________|
 *
 * Low-speed mode time
 * Name 	Description 			TYP 	Tolerance
 * T0H 		0 code, high time 		0.68us 	±150ns
 * T1H 		1 code, high time 		1.36us 	±150ns
 * T0L 		0 code, low time 		1.36us 	±150ns
 * T1L 		1 code, low time 		0.68us 	±150ns
 * Treset 	Resetcode，low time 		24us 	-
 *
 * The formula for the number of loops needed to create a pulse of length t is:
 * n = 71.36*t - 21.276
 *
 * PLEASE NOTE: it appears to me that the datasheet an docs for the TM1803 are incorrect, or I'm missing
 * something, because I had to switch the color order that's being sent on the wire to GBR in order
 * to make the correct colors display at the correct places. No idea why.
 */
void write_color(unsigned char red, unsigned char green, unsigned char blue) {
	int i=0;
	int j=0;

	/*
	//it takes about .25us to write to low when high, so actual delay time is .68us - .25us = .43us
	char t0h_wait_time = 9;
	//it takes about .3us to write to high when low, so actual delay time is 1.36us - .3us = 1.06us
	char t0l_wait_time = 54;
	char t1h_wait_time = 54;
	char t1l_wait_time = 9;
	*/

	 //these times come from poking and fiddling with an o-scope and the edison. Likely to break
	//with edison updates and timing changes.
	int t0h_wait_time = 60;
	int t0l_wait_time = 43; //there's a time delay switching from low to high which means we have to reduce this even though it's a longer wait period
	int t1h_wait_time = 120;
	int t1l_wait_time = 10;

	//write green
	for(i=0; i<8; i++) {
		if(green & 0x80) {
			mraa_gpio_write(data_pin, HIGH);
			for(j=0; j<t1h_wait_time; j++) {__asm__("nop");}

			mraa_gpio_write(data_pin, LOW);
			for(j=0; j<t1l_wait_time; j++) {__asm__("nop");}
		}
		else {
			mraa_gpio_write(data_pin, HIGH);
			for(j=0; j<t0h_wait_time; j++) {__asm__("nop");}

			mraa_gpio_write(data_pin, LOW);
			for(j=0; j<t0l_wait_time; j++) {__asm__("nop");}
		}
		green <<= 1;
	}

	//write blue
	for(i=0; i<8; i++) {
		if(blue & 0x80) {
			mraa_gpio_write(data_pin, HIGH);
			for(j=0; j<t1h_wait_time; j++) {__asm__("nop");}

			mraa_gpio_write(data_pin, LOW);
			for(j=0; j<t1l_wait_time; j++) {__asm__("nop");}
		}
		else {
			mraa_gpio_write(data_pin, HIGH);
			for(j=0; j<t0h_wait_time; j++) {__asm__("nop");}

			mraa_gpio_write(data_pin, LOW);
			for(j=0; j<t0l_wait_time; j++) {__asm__("nop");}
		}
		blue <<= 1;
	}

	//write red
	for(i=0; i<8; i++) {
		if(red & 0x80) {
			mraa_gpio_write(data_pin, HIGH);
			for(j=0; j<t1h_wait_time; j++) {__asm__("nop");}

			mraa_gpio_write(data_pin, LOW);
			for(j=0; j<t1l_wait_time; j++) {__asm__("nop");}
		}
		else {
			mraa_gpio_write(data_pin, HIGH);
			for(j=0; j<t0h_wait_time; j++) {__asm__("nop");}

			mraa_gpio_write(data_pin, LOW);
			for(j=0; j<t0l_wait_time; j++) {__asm__("nop");}
		}
		red <<= 1;
	}

}

/**
 * Write a reset, data line pulled down for > 24us. This causes colors to latch
 * and be displayed.
 */
void write_reset() {
	int i;
	mraa_gpio_write(data_pin, LOW);
	for(i=0; i<2000; i++) {
		__asm__("nop");
	}
}



