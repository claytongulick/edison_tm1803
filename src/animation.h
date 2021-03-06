//callback type for animations
typedef void (*animation_callback)(double);

void render();
void animation_loop();
void animate(double elapsed_time);
int animation_add(animation_callback);
void animation_pulse(double elapsed_time);
void animation_chase(double elapsed_time);
