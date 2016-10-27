#include <stdlib.h>

#include "animation.h"
#include "tm1803.h"
#include "time.h"

#define CLOCKS_PER_MS (CLOCKS_PER_SEC / 1000)
#define FRAMES_PER_SECOND 30
#define MS_PER_FRAME 1000/30
#define MAX_ANIMATIONS 10

char colors[NUM_CHIPS][3];
void (*animations[MAX_ANIMATIONS])(double);


/**
 * Render the current color buffer out to the led chain
 */
void render() {
	int i=0;

	for(i=0; i<NUM_CHIPS; i++) {
		write_color(colors[i][0],colors[i][1],colors[i][2]);
	}
}

/**
 * Basic animation loop that attempts to render at FRAMES_PER_SEC framerate
 */
void animation_loop() {
	clock_t start_clock;
	clock_t current_clock;
	clock_t clock_diff;
	double elapsed_time = 0;
	time_t t;
	//initialize random number generator
	srand((unsigned)time(&t));

	render(); //clear to black

	while(1) {
		start_clock = clock();

		animate(elapsed_time);

		render();

		while(1) {
			current_clock = clock();
			clock_diff = (double)current_clock - start_clock;
			elapsed_time = (double)clock_diff / CLOCKS_PER_MS;
			if(elapsed_time > MS_PER_FRAME)
				break;
		}
	}

}

/**
 * Kick off animation logic and events
 */
void animate(double elapsed_time) {
	static double total_time = 0;
	int i=0;

	total_time += elapsed_time;

	for(i=0;i<MAX_ANIMATIONS;i++)
		if(animations[i])
			(*animations[i])(total_time);
}

/**
 * Add an animation callback to the list. If MAX_ANIMATIONS are exceeded, will return non-zero.
 */
int animation_add(animation_callback callback) {
	int i=0;

	//find a slot to add the animation
	for(i=0;i<MAX_ANIMATIONS; i++) {
		if(!animations[i]) {
			animations[i]=callback;
			return 0;
		}
	}
	return 1;
}

/**
 * This renders a fade in background that pulses all leds to fade in and out
 */
void animation_pulse(double elapsed_time) {
	static int color_r=0;
	static int color_g=0;
	static int color_b=0;
	static float r_delta=0;
	static float g_delta=0;
	static float b_delta=0;
	static double fade_time=0;
	static int direction = 1;

	static unsigned char goal_r;
	static unsigned char goal_g;
	static unsigned char goal_b;
	static int wait_count;
	static int wait_time=2000;

	int color_change_time = 3000;
	int i=0;

	if((elapsed_time-fade_time) >= color_change_time) direction = -1;
	else direction = 1;

	//let's randomly fade along a line using lerp to a point in 3d space
	if(
		(color_r <=0) &&
		(color_g <= 0) &&
		(color_b <= 0)
		) {

		wait_count++;

		//let's wait a random amount of time between "pulses" - later we can trigger this from beat detection
		if((wait_count * MS_PER_FRAME) < wait_time) {
			//render black frame
			for(i=0; i<NUM_CHIPS; i++) {
				colors[i][0] = 0;
				colors[i][1] = 0;
				colors[i][2] = 0;
			}
			return;
		}

		wait_count = 0;
		wait_time = rand() % 5000;
		wait_time += 1000;

		fade_time=elapsed_time;
		goal_r = rand() % 256;
		goal_g = rand() % 256;
		goal_b = rand() % 256;

		//randomly change the speed
		color_change_time = rand() % 500;
		color_change_time += 100;

		r_delta = (float)((int)goal_r) / color_change_time;
		g_delta = (float)((int)goal_g) / color_change_time;
		b_delta = (float)((int)goal_b) / color_change_time;
	}

	for(i=0; i<NUM_CHIPS;i++) {
		colors[i][0] = color_r;
		colors[i][1] = color_g;
		colors[i][2] = color_b;
	}

	color_r += r_delta * MS_PER_FRAME * direction;
	color_g += g_delta * MS_PER_FRAME * direction;
	color_b += b_delta * MS_PER_FRAME * direction;
	if(color_r > goal_r) color_r = goal_r;
	if(color_g > goal_g) color_g = goal_g;
	if(color_b > goal_b) color_b = goal_b;
	if(color_r < 0) color_r = 0;
	if(color_g < 0) color_g = 0;
	if(color_b < 0) color_b = 0;

}

/**
 * Simple chase animation of a random color
 */
void animation_chase(double elapsed_time) {
	const int color_change_time = 3000;
	static double start_time = 0;
	static int color_index = 0;
	static float speed = 50;
	static int direction = 1;
	static unsigned char color_r=255;
	static unsigned char color_g=255;
	static unsigned char color_b=255;
	static float r_delta=0;
	static float g_delta=0;
	static float b_delta=0;
	static double fade_time=0;
	static unsigned char arrived=1;

	unsigned char goal_r;
	unsigned char goal_g;
	unsigned char goal_b;
	int temp_direction = 0;


	//initialize
	if(start_time == 0) {
		colors[0][0] = color_r;
		colors[0][1] = color_g;
		colors[0][2] = color_b;
		start_time = 1;
		return;
	}

	//let's randomly fade along a line using lerp to a point in 3d space
	if(arrived) {
		arrived = 0;
		fade_time=elapsed_time;
		goal_r = rand() % 256;
		goal_g = rand() % 256;
		goal_b = rand() % 256;

		r_delta = (float)((int)goal_r - (int)color_r) / color_change_time;
		g_delta = (float)((int)goal_g - (int)color_g) / color_change_time;
		b_delta = (float)((int)goal_b - (int)color_b) / color_change_time;

		//randomly change the direction
		temp_direction = rand() % 2;
		direction = temp_direction == 0 ? -1 : 1;

		//randomly change the speed
		speed = rand() % 200;
	}

	color_r += r_delta * MS_PER_FRAME;
	color_g += g_delta * MS_PER_FRAME;
	color_b += b_delta * MS_PER_FRAME;

	if((elapsed_time-fade_time) >= color_change_time) arrived = 1;

	//support fades outside of motion with this
	colors[color_index][0] = color_r;
	colors[color_index][1] = color_g;
	colors[color_index][2] = color_b;


	if((elapsed_time - start_time) > speed) {
		color_index += direction;
		//do some edge detection
		if(color_index >= NUM_CHIPS)
			color_index = 0;
		if(color_index < 0)
			color_index = NUM_CHIPS - 1;

		//set the new color
		colors[color_index][0] = color_r;
		colors[color_index][1] = color_g;
		colors[color_index][2] = color_b;

		//reset timing
		start_time = elapsed_time;
	}

}
