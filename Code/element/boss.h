#ifndef BOSS_H_INCLUDED
#define BOSS_H_INCLUDED

#include "element.h"
#include "charater.h"
#include "drop.h"
#include "../shapes/Shape.h"
#include "../algif5/src/algif.h"

typedef struct _Boss
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
    bool new_proj;
    double weapon_dir_x, weapon_dir_y;
    ALLEGRO_SAMPLE_INSTANCE *atk_Sound;
    int anime;      // counting the time of animation
    int anime_time; // indicate how long the animation
    Shape *hitbox;
} Boss;

Elements *New_Boss(int label, Character *target);
void _Boss_update_position(Elements *self, int dx, int dy);
void Boss_update(Elements *self);
void Boss_draw(Elements *self);
void Boss_destroy(Elements *self);

#endif
