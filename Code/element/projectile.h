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
    double vdx, vdy; // the velocity unit vector
    double damage;
    ALLEGRO_BITMAP *img;
    Shape *hitbox; // the hitbox of object
} Projectile;
Elements *New_Projectile(Elements *creator, int label, double damage, double x, double y, int v, double vdx, double vdy);
void Projectile_update(Elements *self);
void Projectile_interact(Elements *self, Elements *tar);
void Projectile_draw(Elements *self);
void Projectile_destory(Elements *self);
void _Projectile_update_position(Elements *self, double dx, double dy);
#endif
