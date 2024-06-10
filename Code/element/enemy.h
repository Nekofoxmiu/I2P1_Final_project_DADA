#ifndef ENEMY_H_INCLUDED
#define ENEMY_H_INCLUDED

#include "element.h"
#include "charater.h"
#include "../shapes/Shape.h"
#include "../algif5/src/algif.h"

typedef enum _EnemyType
{
    slime_L,
    dog_L,
    dragon_L
} EnemyType;

typedef struct _EnemyConfig
{
    char stop[100];
    char move[100];
    char attack[100];
    double blood;
    double armor;
    double damage;
    double chase_distance;
    double attack_distance;
    double chase_speed;
} EnemyConfig;

typedef struct _Enemy
{
    int x, y;
    int width, height;
    bool dir;
    int state;
    double blood;
    double armor;
    double damage;
    double chase_distance;
    double attack_distance;
    double chase_speed;
    Character *target;              // the character to track
    ALGIF_ANIMATION *gif_status[3]; // gif for each state: 0: stop, 1: move, 2: attack
    ALLEGRO_SAMPLE_INSTANCE *atk_Sound;
    int anime;      // counting the time of animation
    int anime_time; // indicate how long the animation
    Shape *hitbox;
    EnemyType type; // 新增的成員，用來存儲敵人的類型
} Enemy;

Elements *New_Enemy(int label, EnemyType enemyType, Character *target, 
    double atk_enhance, double def_enhance, double hp_enhance, double chasedis_enhance, double atkdis_enhance, double spd_enhance);
void _Enemy_update_position(Elements *self, int dx, int dy);
void Enemy_update(Elements *self);
void Enemy_draw(Elements *self, float camera_offset_x, float camera_offset_y);
void Enemy_destroy(Elements *self);

void load_enemy_config(const char *filename, EnemyConfig configs[]);

#endif
