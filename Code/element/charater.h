#ifndef CHARATER_H_INCLUDED
#define CHARATER_H_INCLUDED

#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include "element.h"
#include "../shapes/Shape.h"
#include "../algif5/src/algif.h"
#include <stdbool.h>

/*
   [character object]
*/
typedef enum CharacterType
{
    default_chara_L,
    fire_chara_L
} CharacterType;

typedef enum CharacterMoveType
{
    STOP = 0,
    MOVE,
    ATK
} CharacterMoveType;

typedef struct _CharacterConfig
{
    char stop[100];
    char move[100];
    char attack[100];
    double blood;
    double armor;
    double damage;
    double speed;
} CharacterConfig;

typedef struct _Character
{
    int x, y;
    int width, height; // the width and height of image
    char dir;          // 'L' for left, 'R' for right
    double weapon_dir_x, weapon_dir_y;
    int state;                      // the state of character
    ALGIF_ANIMATION *gif_status[3]; // gif for each state. 0: stop, 1: move, 2:attack
    ALLEGRO_SAMPLE_INSTANCE *atk_Sound;
    int anime;      // counting the time of animation
    int anime_time; // indicate how long the animation
    bool new_proj;
    Shape *hitbox; // the hitbox of object
    double blood;
    double armor;
    double damage;
    double speed;
    CharacterType type;
} Character;

Elements *New_Character(int label, CharacterType charaType);
void _Character_update_position(Elements *self, int dx, int dy);
void Character_update(Elements *self);
void Character_interact(Elements *self, Elements *target);
void Character_draw(Elements *self);
void Character_destory(Elements *self);
void Character_get_position(Elements *self, float *x, float *y);
void load_character_config(const char *filename, CharacterConfig config[]);

#endif
