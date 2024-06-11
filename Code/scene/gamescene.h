#ifndef GAMESCENE_H_INCLUDED
#define GAMESCENE_H_INCLUDED
#include "scene.h"
#include "../global.h"
#include "../element/element.h"
#include "../element/charater.h"
#include "../element/floor.h"
//#include "../element/teleport.h"
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
/*
    如果要新增元素，請在這裡新增
    如果是要在遊戲初始就通過註冊事件生成的請注意順序gamescene.c中註冊順序
    否則不能使用_Get_all_elements(self).arr[enum number]->pDerivedObj直接取得
    而須改用_Get_label_elements(self, enum number)取得陣列後在取得
*/
typedef enum EleType
{
    Floor_L,
    //Teleport_L,
    Tree_L,
    Character_L,
    Enemy_L,
    Boss_L,
    Projectile_L,
    Wall_L,
    Drop_L,
    Levelup_L,
    Explosion_L
} EleType;
typedef struct _GameScene
{
    ALLEGRO_FONT *font;
    ALLEGRO_BITMAP *background;
    int chara_blood_x, chara_blood_y;
    double ene_hp_enhance;
    double ene_def_enhance;
    double ene_atk_enhance;
    double ene_chasedis_enhance;
    double ene_atkdis_enhance;
    double ene_spd_enhance;
    double start_time;
    double start_time_spawn;
    double start_time_boss;
    double elapsed_time;
    double elapsed_time_spawn;
    double elapsed_time_boss;

    int chara_mp_x, chara_mp_y;
    int chara_exp_x, chara_exp_y;
    double ene_spawn_rate;
    double ene_spawn_acc;
    double boss_spawn_rate;
    double boss_spawn_acc;

    double slime_proportion;
    double dog_proportion;

    ALLEGRO_SAMPLE *song;
    ALLEGRO_SAMPLE_INSTANCE *sample_instance;
} GameScene;
Scene *New_GameScene(int label);
void game_scene_update(Scene *self);
void game_scene_draw(Scene *self);
void game_scene_destroy(Scene *self);
void update_camera(Character *chara);

#endif
