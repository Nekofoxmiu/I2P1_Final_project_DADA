#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED
#include "scene.h"
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include "../algif5/src/algif.h"
/*
   [Menu object]
*/

typedef struct _Menu
{
    ALLEGRO_FONT *font_large;
    ALLEGRO_FONT *font_small;
    ALLEGRO_BITMAP *background;
    ALGIF_ANIMATION *torch;
    ALGIF_ANIMATION *charactor;
    ALLEGRO_SAMPLE *song_background;
    ALLEGRO_SAMPLE *song_enter;
    ALLEGRO_SAMPLE_INSTANCE *sample_instance_background;
    ALLEGRO_SAMPLE_INSTANCE *sample_instance_enter;
    int title_x, title_y;
} Menu;
Scene *New_Menu(int label);
void menu_update(Scene *self);
void menu_draw(Scene *self);
void menu_destroy(Scene *self);

#endif
