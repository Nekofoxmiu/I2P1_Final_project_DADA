#include "global.h"
#include "shapes/Shape.h"
#include <stdbool.h>
// variables for global usage
const double FPS = 60.0;
const int WIDTH = 960;
const int HEIGHT = 640;
const int PIXEL_SIZE = 32;
// world size
const int WORLD_WIDTH = 4096;
const int WORLD_HEIGHT = 4096;
float camera_x = 0;
float camera_y = 0;
int window = 0;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_EVENT event;
ALLEGRO_TIMER *fps = NULL;
bool key_state[ALLEGRO_KEY_MAX] = {false};
bool mouse_state[ALLEGRO_MOUSE_MAX_EXTRA_AXES] = {false};
Point mouse;
bool debug_mode = true;
bool spawn_enemy = false;
bool spawn_boss = false;
bool spawn_wall = false;
bool everything_stop = false;
bool game_over = false;
bool level_up = false;
ALLEGRO_DISPLAY *displayptr = NULL;