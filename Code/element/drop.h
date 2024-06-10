#ifndef DROP_H_INCLUDED
#define DROP_H_INCLUDED
#include "element.h"
#include "../scene/gamescene.h" // for element label
#include "../shapes/Shape.h"
#include "../algif5/src/algif.h"
/*
   [drop object]
*/
typedef enum _DropType {
    XP_L,
    HP_L,
    MP_L
} DropType;

typedef struct _Drop
{
    int x, y;          // the position of image
    int width, height; // the width and height of image
    ALLEGRO_BITMAP *img;
    ALLEGRO_SAMPLE_INSTANCE *pick_Sound;
    Shape *hitbox; // the hitbox of object
    DropType type;
    int life;
} Drop;

Elements *New_Drop(int label, DropType type, int x, int y);
void Drop_update(Elements *self);
void Drop_interact(Elements *self, Elements *tar);
void Drop_draw(Elements *self);
void Drop_destory(Elements *self);

#endif
