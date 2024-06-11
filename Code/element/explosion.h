#ifndef EXPLOSION_H_INCLUDED
#define EXPLOSION_H_INCLUDED
#include "element.h"
#include "../scene/gamescene.h" // for element label
#include "../shapes/Shape.h"
#include "../algif5/src/algif.h"
/*
   [explosion object]
*/
typedef struct _Explosion
{
    double x, y;
    ALGIF_ANIMATION *explosion;
} Explosion;

Elements *New_Explosion(int label, double x, double y);
void Explosion_update(Elements *self);
void Explosion_interact(Elements *self, Elements *tar);
void Explosion_draw(Elements *self, float camera_offset_x, float camera_offset_y);
void Explosion_destory(Elements *self);

#endif