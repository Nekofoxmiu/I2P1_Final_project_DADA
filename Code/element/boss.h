#ifndef BOSS_H_INCLUDED
#define BOSS_H_INCLUDED

#include "element.h"
#include "charater.h"
#include "drop.h"
#include "../shapes/Shape.h"
#include "../algif5/src/algif.h"

/*
   [boss object]
*/

typedef enum _BossType
{
    default_boss_L
} BossType;

typedef struct _BossConfig
{
    char stop[100];
    char move[100];
    char attack[100];
    double blood;
    double armor;
    double damage;
    double attack_distance;
    DropConfig DropConfig;
} BossConfig;


typedef struct _Boss
{
    int x, y;
    int width, height;
    bool dir;
    int state;
    double blood;
    double armor;
    double damage;
    double attack_distance;
    DropConfig DropConfig;
    Character *target;              // the character to track
    ALGIF_ANIMATION *gif_status[3]; // gif for each state: 0: stop, 1: move, 2: attack
    bool new_proj;
    double weapon_dir_x, weapon_dir_y;
    ALLEGRO_SAMPLE_INSTANCE *atk_Sound;
    int anime;      // counting the time of animation
    int anime_time; // indicate how long the animation
    Shape *hitbox;
    BossType type; // 新增的成員，用來存儲敵人的類型
    double aura_dmg_start_time;
    double aura_dmg_elapsed_time;
    double atk_start_time;
    double atk_elapsed_time;
} Boss;

Elements *New_Boss(int label, Character *target);
void _Boss_update_position(Elements *self, int dx, int dy);
void Boss_update(Elements *self);
void Boss_draw(Elements *self, float camera_offset_x, float camera_offset_y);
void Boss_destroy(Elements *self);

#endif
