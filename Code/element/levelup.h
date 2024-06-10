#ifndef LEVELUP_H_INCLUDED
#define LEVELUP_H_INCLUDED
#include "element.h"
#include "../scene/gamescene.h" // for element label
#include "charater.h"
/*
   [LEVELUP object]
*/
typedef struct _Levelup
{
    int x, y;          // the position of image
    int width, height; // the width and height of image
    Character *player;
    ALLEGRO_BITMAP *img;
    ALLEGRO_BITMAP *choiceVec[10]; // the choice vector
} Levelup;
Elements *New_Levelup(int label, Character *player);
void Levelup_update(Elements *self);
void Levelup_interact(Elements *self, Elements *tar);
void Levelup_draw(Elements *self, float camera_offset_x, float camera_offset_y);
void Levelup_destory(Elements *self);

#endif
