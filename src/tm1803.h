
//these can be inverted, for example if you're using an NPN transistor for level shifting up to 5v
#ifndef HIGH
#define HIGH 0
#endif

#ifndef LOW
#define LOW 1
#endif

//set this to the number of tm1803 chips you have in a daisy chain
#ifndef NUM_CHIPS
#define NUM_CHIPS 10
#endif

int tm1803_init();
void write_color(unsigned char red, unsigned char green, unsigned char blue);
void write_reset();
