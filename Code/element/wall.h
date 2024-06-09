#ifndef FLOOR_H_INCLUDED
#define FLOOR_H_INCLUDED
#include "element.h"
#include "charater.h"
#include "enemy.h"
#include "boss.h"
#include "projectile.h"
#include "../shapes/Shape.h"
#include "../scene/gamescene.h" // for element label

/*
   [wall object]
*/
typedef struct _Wall
{
    int x, y;          // the position of image
    int width, height; // the width and height of image
    Shape *hitbox;     // the hitbox of object
    ALLEGRO_BITMAP *img;
} Wall;

Elements *New_Wall(int label, int x, int y);
void Wall_update(Elements *self);
void Wall_interact(Elements *self, Elements *tar);
void Wall_draw(Elements *self);
void Wall_destroy(Elements *self);

#endif