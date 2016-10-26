#include "animation.h"
#include "tm1803.h"
#include "time.h"

#define CLOCKS_PER_MS CLOCKS_PER_SEC / 1000
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

	while(1) {
		start_clock = clock();

		animate(elapsed_time);

		while(1) {
			current_clock = clock();
			clock_diff = (double)start_clock - current_clock;
			elapsed_time = (double)clock_diff / CLOCKS_PER_MS;
			if(elapsed_time > MS_PER_FRAME)
				break;
		}
	}

}

/**
 * Kick off animation logic and events
 */
void animate(elapsed_time) {
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
 * Simple chase animation of a random color
 */
void animation_chase(double elapsed_time) {
	static double start_time = 0;
	static int color_index = 0;
	static float speed = 1000;
	static int direction = 1;
	static char old_color_r;
	static char old_color_g;
	static char old_color_b;
	static char color_r=255;
	static char color_g=255;
	static char color_b=255;

	//initialize
	if(start_time == 0) {
		old_color_r = (char)colors[0][0];
		old_color_g = (char)colors[0][1];
		old_color_b = (char)colors[0][2];
		return;
	}

	if((elapsed_time - start_time) > speed) {

		//restore the old color
		colors[color_index][0] = old_color_r;
		colors[color_index][1] = old_color_g;
		colors[color_index][2] = old_color_b;

		color_index += direction;
		//do some edge detection
		if(color_index >= NUM_CHIPS)
			color_index = 0;
		if(color_index < 0)
			color_index = NUM_CHIPS - 1;

		//cache the existing color at the new index
		old_color_r = (char)colors[color_index][0];
		old_color_g = (char)colors[color_index][1];
		old_color_b = (char)colors[color_index][2];

		//set the new color
		colors[color_index][0] = color_r;
		colors[color_index][1] = color_g;
		colors[color_index][2] = color_b;

		//reset timing
		start_time = elapsed_time;
	}
}
