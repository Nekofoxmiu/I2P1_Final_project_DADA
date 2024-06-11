#ifndef LEVELUP_H_INCLUDED
#define LEVELUP_H_INCLUDED
#include "element.h"
#include "../scene/gamescene.h" // for element label
#include "../gameWindow.h" // for display
#include "charater.h"
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

typedef struct _Enhancement
{
    const char *description;
    ALLEGRO_BITMAP *icon;
    int width;
    int height;
} Enhancement;

typedef struct _Levelup {
    int x, y;          // the position of image
    int width, height; // the width and height of image
    Character *player;
    ALLEGRO_BITMAP *background;
    ALLEGRO_FONT *font;
    Enhancement enhancements[3]; // the three enhancements to display
    bool mouse_button_pressed;
    ALLEGRO_SAMPLE *song;
    ALLEGRO_SAMPLE_INSTANCE *sample_instance;
} Levelup;


Elements *New_Levelup(int label, Character *player);
void Levelup_update(Elements *self);
void Levelup_interact(Elements *self, Elements *tar);
void Levelup_draw(Elements *self, float camera_offset_x, float camera_offset_y);
void Levelup_destory(Elements *self);
void Levelup_handle_mouse_click(Levelup *Obj, Elements *self, int mouse_x, int mouse_y);
void Levelup_handle_mouse_hover(Levelup *Obj, int mouse_x, int mouse_y);

#endif
