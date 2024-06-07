#ifndef Projectile_H_INCLUDED
#define Projectile_H_INCLUDED
#include "element.h"
#include "../scene/gamescene.h" // for element label
#include "../shapes/Shape.h"
/*
   [Projectile object]
*/
typedef struct _Projectile
{
    double x, y;          // the position of image
    int width, height; // the width and height of image
    int v;             // the velocity of projectile
    double damage;
    double wx, wy;
    ALLEGRO_BITMAP *img;
    Shape *hitbox; // the hitbox of object
} Projectile;
Elements *New_Projectile(int label, double damage, double x, double y, int v, double wx, double wy);
void Projectile_update(Elements *self);
void Projectile_interact(Elements *self, Elements *tar);
void Projectile_draw(Elements *self);
void Projectile_destory(Elements *self);
void _Projectile_update_position(Elements *self, double dx, double dy);
#endif
