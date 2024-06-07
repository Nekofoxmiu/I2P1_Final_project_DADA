#include "boss.h"
#include "../shapes/Rectangle.h"
#include <allegro5/allegro_image.h>
#include "../scene/gamescene.h"
#include "../scene/sceneManager.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MIN_DISTANCE 400
#define MAX_DISTANCE 600
#define CHASE_DISTANCE 500
#define ATTACK_DISTANCE 200
#define CHASE_SPEED 3
#define OK_RANGE 1
#define M_PI 3.14159265358979323846

/*
   [Boss function]
*/
Elements *New_Boss(int label, Character *target)
{
    Boss *pDerivedObj = (Boss *)malloc(sizeof(Boss));
    Elements *pObj = New_Elements(label);

    // 加載動畫
    char state_string[3][10] = {"stop", "move", "attack"};
    for (int i = 0; i < 3; i++)
    {
        char buffer[53];
        sprintf(buffer, "assets/image/boss_%s.gif", state_string[i]);
        pDerivedObj->gif_status[i] = algif_new_gif(buffer, -1);
    }
    // load effective sound
    ALLEGRO_SAMPLE *sample = al_load_sample("assets/sound/atk_sound.wav");
    pDerivedObj->atk_Sound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(pDerivedObj->atk_Sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(pDerivedObj->atk_Sound, al_get_default_mixer());

    // 隨機生成初始位置在角色周遭MAX_DISTANCE範圍內MIN_DISTANCE外
    double angle = ((double)rand() / RAND_MAX) * 2 * M_PI;
    double radius = MIN_DISTANCE + ((double)rand() / RAND_MAX) * (MAX_DISTANCE - MIN_DISTANCE);
    pDerivedObj->x = target->x + (int)(radius * cos(angle));
    pDerivedObj->y = target->y + (int)(radius * sin(angle));

    // 初始化敵人的其他成員
    pDerivedObj->width = pDerivedObj->gif_status[0]->width;
    pDerivedObj->height = pDerivedObj->gif_status[0]->height;
    pDerivedObj->blood = 20;
    pDerivedObj->armor = 1;
    pDerivedObj->damage = 2;
    pDerivedObj->dir = true;      // 初始方向
    pDerivedObj->state = STOP;    // 初始狀態
    pDerivedObj->target = target; // 設定目標角色
    pDerivedObj->anime = 0;
    pDerivedObj->anime_time = 0;
    pDerivedObj->hitbox = New_Rectangle(pDerivedObj->x, pDerivedObj->y,
                                        pDerivedObj->x + pDerivedObj->width,
                                        pDerivedObj->y + pDerivedObj->height);

    pObj->pDerivedObj = pDerivedObj;
    pObj->Draw = Boss_draw;
    pObj->Update = Boss_update;
    pObj->Destroy = Boss_destroy;
    return pObj;
}

void Boss_update(Elements *self)
{
    Boss *boss = (Boss *)(self->pDerivedObj);
    Character *target = boss->target;

    if (boss->blood <= 0)
    {
        self->dele = true;
        return;
    }

    // 計算敵人與角色之間的距離
    int dx = target->x - boss->x;
    int dy = target->y - boss->y;
    double distance = sqrt(dx * dx + dy * dy);
    int chaseRange_x = 0;
    int chaseRange_y = 0;

    // 解決初始化時碰撞箱不正確問題
    _Boss_update_position(self, 0, 0);

    // 保持小於 CHASE_DISTANCE 距離
    if (distance > CHASE_DISTANCE)
    {
        boss->state = MOVE;
        if (target->x > boss->x)
        {
            boss->dir = true;
        }
        else if (target->x < boss->x)
        {
            boss->dir = false;
        }

        // 正規化速度
        if (distance != 0)
        {
            chaseRange_x = (int)(CHASE_SPEED * dx / distance);
            chaseRange_y = (int)(CHASE_SPEED * dy / distance);
        }

        _Boss_update_position(self, chaseRange_x, chaseRange_y);
    }

    // 保持大於 ATTACK_DISTANCE 距離
    if (distance < ATTACK_DISTANCE)
    {
        boss->state = MOVE;
        if (target->x > boss->x)
        {
            boss->dir = true;
        }
        else if (target->x < boss->x)
        {
            boss->dir = false;
        }

        // 正規化速度
        if (distance != 0)
        {
            chaseRange_x = -(int)(CHASE_SPEED * dx / distance);
            chaseRange_y = -(int)(CHASE_SPEED * dy / distance);
        }

        _Boss_update_position(self, chaseRange_x, chaseRange_y);
    }
    else
    {
        boss->state = ATK;

        if (target->x > boss->x)
        {
            boss->dir = true;
        }
        else if (target->x < boss->x)
        {
            boss->dir = false;
        }

        if (boss->gif_status[boss->state]->done)
        {
            boss->new_proj = false;
        }
        // GIF 速度調快的時候偵測的 Index 要像後調或去掉這個條件
        if (boss->gif_status[ATK]->display_index == 2 && boss->new_proj == false)
        {
            double offset_x = (boss->dir == true) ? boss->width + 5 : -50;
            double offset_y = 10;
            double weapon_x = boss->x + offset_x;
            double weapon_y = boss->y + offset_y;

            double dx = boss->target->x - weapon_x;
            double dy = boss->target->y - weapon_y;
            double len = sqrt(dx * dx + dy * dy);

            if (len != 0)
            {
                boss->weapon_dir_x = dx / len;
                boss->weapon_dir_y = dy / len;
            }
            else
            {
                boss->weapon_dir_x = 1;
                boss->weapon_dir_y = 1;
            }

            Elements *pro;
            pro = New_Projectile(Projectile_L,
                                weapon_x,
                                weapon_y,
                                10, boss->weapon_dir_x, boss->weapon_dir_y);
            _Register_elements(scene, pro);
            boss->new_proj = true;
        }
    }
}


void Boss_draw(Elements *self)
{
    Boss *boss = (Boss *)(self->pDerivedObj);
    ALLEGRO_BITMAP *frame = algif_get_bitmap(boss->gif_status[boss->state], al_get_time());
    if (frame)
    {
        al_draw_bitmap(frame, boss->x, boss->y, ((boss->dir) ? ALLEGRO_FLIP_HORIZONTAL : 0));
    }
    if (boss->state == ATK && boss->gif_status[boss->state]->display_index == 2)
    {
        al_play_sample_instance(boss->atk_Sound);
    }
}

void _Boss_update_position(Elements *self, int dx, int dy)
{
    Boss *boss = ((Boss *)(self->pDerivedObj));
    boss->x += dx;
    boss->y += dy;
    Shape *hitbox = boss->hitbox;
    hitbox->update_center_x(hitbox, dx);
    hitbox->update_center_y(hitbox, dy);
}

void Boss_destroy(Elements *self)
{
    Boss *boss = (Boss *)(self->pDerivedObj);
    for (int i = 0; i < 3; i++)
        algif_destroy_animation(boss->gif_status[i]);
    free(boss->hitbox);
    free(boss);
    free(self);
}
