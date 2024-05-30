#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "element.h"
#include "charater.h"
#include "../shapes/Shape.h"
#include "../algif5/src/algif.h"

typedef struct _Enemy
{
    int x, y;
    int width, height;
    bool dir;
    int state;
    float blood;
    float armor;
    float damage;
    Character *target;              // the character to track
    ALGIF_ANIMATION *gif_status[3]; // gif for each state: 0: stop, 1: move, 2: attack
    ALLEGRO_SAMPLE_INSTANCE *atk_Sound;
    int anime;      // counting the time of animation
    int anime_time; // indicate how long the animation
    Shape *hitbox;
} Enemy;

Elements *New_Enemy(int label, Character *target);
void _Enemy_update_position(Elements *self, int dx, int dy);
void Enemy_update(Elements *self);
void Enemy_draw(Elements *self);
void Enemy_destroy(Elements *self);

#endif
