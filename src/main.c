/*
 * TM1803 LED Control for Intel Edison
 *
 * @author Clayton Gulick
 * @email claytongulick@gmail.com
 *
 * Copyright 2016 Clayton Gulick
 *
 * This code is released under the MIT license. Please keep my name and give me
 * credit if you use it, that's all I ask.
 */

#include <stdio.h>
#include <unistd.h>
#include <sched.h>

#include "tm1803.h"

int main()
{
	printf("Initializing...");
	if(tm1803_init() > 0) {
		return printf("Failed to initialize");
	}

	//we set the thread scheduling priority here with a FIFO queue to "encourage" linux not to interrupt us
	const struct sched_param priority={1};
	sched_setscheduler(0,SCHED_FIFO,&priority);

	printf("writing colors\n");
	write_reset();

	while(1) {
		write_color(255,0,0);
		write_color(0,255,0);
		write_color(0,0,255);
		write_color(255,0,0);
		write_color(0,255,0);
		write_color(0,0,255);
		write_color(255,0,0);
		write_color(0,255,0);
		write_color(0,0,255);

		write_reset();
		//sleep(1);
	}
	printf("colors written\n");
	return 0;
}
