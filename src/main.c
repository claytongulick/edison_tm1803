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
#include "animation.h"

int main()
{
	printf("Initializing...\n");
	if(tm1803_init() > 0) {
		return printf("Failed to initialize");
	}

	//we set the thread scheduling priority here with a FIFO queue to "encourage" linux not to interrupt us
	const struct sched_param priority={1};
	sched_setscheduler(0,SCHED_FIFO,&priority);

	printf("Starting animation\n");

	animation_add(animation_pulse);
	animation_add(animation_chase);
	animation_loop();

	return 0;
}
