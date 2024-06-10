#ifndef RESULT_H_INCLUDED
#define RESULT_H_INCLUDED
#include "scene.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "../algif5/src/algif.h"
/*
   [Result object]
*/

typedef struct _Result
{
    ALLEGRO_FONT *font_large;
    ALLEGRO_FONT *font_small;
    ALLEGRO_BITMAP *background;
    ALGIF_ANIMATION *torch;
    ALGIF_ANIMATION *charactor;
    ALLEGRO_SAMPLE *song;
    ALLEGRO_SAMPLE_INSTANCE *sample_instance;
    int title_x, title_y;
} Result;
Scene *New_Result(int label);
void result_update(Scene *self);
void result_draw(Scene *self);
void result_destroy(Scene *self);

#endif
