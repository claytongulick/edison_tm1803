//callback type for animations
typedef void (*animation_callback)(double);

void render();
void animation_loop();
void animate();
int animation_add(animation_callback);
void animation_chase(double);
