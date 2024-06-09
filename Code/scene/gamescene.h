#ifndef GAMESCENE_H_INCLUDED
#define GAMESCENE_H_INCLUDED
#include "scene.h"
#include "../global.h"
#include "../element/element.h"
#include "../element/charater.h"
#include "../element/floor.h"
#include "../element/teleport.h"
#include "../element/tree.h"
#include "../element/projectile.h"
#include "../element/enemy.h"
#include "../element/boss.h"
#include "../element/wall.h"
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

/*
   [game scene object]
*/
typedef enum EleType
{
    Wall_L,
    Floor_L,
    Teleport_L,
    Tree_L,
    Character_L,
    Enemy_L,
    Boss_L,
    Projectile_L
} EleType;
typedef struct _GameScene
{
    ALLEGRO_FONT *font;
    ALLEGRO_BITMAP *background;
    int chara_blood_x, chara_blood_y;
} GameScene;
Scene *New_GameScene(int label);
void game_scene_update(Scene *self);
void game_scene_draw(Scene *self);
void game_scene_destroy(Scene *self);
void update_camera(Character *chara);

#endif
