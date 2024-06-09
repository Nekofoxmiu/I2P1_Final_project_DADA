#ifndef Projectile_H_INCLUDED
#define Projectile_H_INCLUDED
#include "element.h"
#include "../scene/gamescene.h" // for element label
#include "../scene/sceneManager.h"
#include "../shapes/Shape.h"
/*
   [Projectile object]
*/
typedef struct _Projectile
{
    Elements* creator;
    double x, y;          // the position of image
    int width, height; // the width and height of image
    int v;             // projectile speed
    double vdx, vdy; // projectile velocity unit vector
    bool chase;
    double damage;
    ALLEGRO_BITMAP *img;
    Shape *hitbox; // the hitbox of object
} Projectile;


Elements *New_Projectile(Elements *creator, int label, double damage, double x, double y, int v, double vdx, double vdy, bool chase);
void Projectile_update(Elements *self);
void Projectile_interact(Elements *self, Elements *tar);
void Projectile_draw(Elements *self);
void Projectile_destory(Elements *self);
void _Projectile_update_position(Elements *self, double dx, double dy);

void Attack_Normal(Elements* creator, int num_bullets, int speed, bool chase);
void Attack_Radial(Elements* creator, int num_bullets, int speed, int rounds);
#endif
