#include "enemy.h"
#include "../shapes/Rectangle.h"
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MAX_DISTANCE 400
#define CHASE_DISTANCE 200
#define ATTACK_DISTANCE 50
#define CHASE_SPEED 3
#define OK_RANGE 1
#define M_PI 3.14159265358979323846

/*
   [Enemy function]
*/
Elements *New_Enemy(int label, Character *target)
{
    Enemy *pDerivedObj = (Enemy *)malloc(sizeof(Enemy));
    Elements *pObj = New_Elements(label);

    // 加載動畫
    char state_string[3][10] = {"stop", "move", "attack"};
    for (int i = 0; i < 3; i++)
    {
        char buffer[53];
        sprintf(buffer, "assets/image/enemy_%s.gif", state_string[i]);
        pDerivedObj->gif_status[i] = algif_new_gif(buffer, -1);
    }
    // load effective sound
    ALLEGRO_SAMPLE *sample = al_load_sample("assets/sound/atk_sound.wav");
    pDerivedObj->atk_Sound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(pDerivedObj->atk_Sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(pDerivedObj->atk_Sound, al_get_default_mixer());

    // 隨機生成初始位置在角色周遭400範圍內
    double angle = ((double)rand() / RAND_MAX) * 2 * M_PI;
    double radius = ((double)rand() / RAND_MAX) * MAX_DISTANCE;
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
    pObj->Draw = Enemy_draw;
    pObj->Update = Enemy_update;
    pObj->Destroy = Enemy_destroy;
    return pObj;
}

void Enemy_update(Elements *self)
{
    Enemy *enemy = (Enemy *)(self->pDerivedObj);
    Character *target = enemy->target;

    if(enemy->blood <= 0)
    {
        self->dele = true;
        return;
    }

    // 計算敵人與角色之間的距離
    int dx = target->x - enemy->x;
    int dy = target->y - enemy->y;
    double distance = sqrt(dx * dx + dy * dy);
    int chaseRange_x = 0;
    int chaseRange_y = 0;

    // 如果距離小於200，則進行追逐
    if (distance < CHASE_DISTANCE)
    {
        if (distance > ATTACK_DISTANCE)
        {
            enemy->state = MOVE;
            if (target->x > enemy->x)
            {
                enemy->dir = true;
            }
            else if (target->x < enemy->x)
            {
                enemy->dir = false;
            }

            if (abs(dx) < CHASE_SPEED)
            {
                chaseRange_x = dx;
            }
            else
            {
                if (dx & 0x80000000)
                {
                    //負數
                    chaseRange_x = -CHASE_SPEED;
                }
                else
                {
                    chaseRange_x = CHASE_SPEED;
                }
            }

            if (abs(dy) < CHASE_SPEED)
            {
                chaseRange_y = dy;
            }
            else
            {
                if (dy & 0x80000000)
                {
                    //負數
                    chaseRange_y = -CHASE_SPEED;
                }
                else
                {
                    chaseRange_y = CHASE_SPEED;
                }
            }

            _Enemy_update_position(self, chaseRange_x, chaseRange_y);
        }
        else
        {
            enemy->state = ATK;
        }
    }
    else
    {
        enemy->state = STOP;
    }
}

void Enemy_draw(Elements *self)
{
    Enemy *enemy = (Enemy *)(self->pDerivedObj);
    ALLEGRO_BITMAP *frame = algif_get_bitmap(enemy->gif_status[enemy->state], al_get_time());
    if (frame)
    {
        al_draw_bitmap(frame, enemy->x, enemy->y, ((enemy->dir) ? ALLEGRO_FLIP_HORIZONTAL : 0));
    }
    if (enemy->state == ATK && enemy->gif_status[enemy->state]->display_index == 2)
    {
        al_play_sample_instance(enemy->atk_Sound);
    }
}

void _Enemy_update_position(Elements *self, int dx, int dy)
{
    Enemy *enemy = ((Enemy *)(self->pDerivedObj));
    enemy->x += dx;
    enemy->y += dy;
    Shape *hitbox = enemy->hitbox;
    hitbox->update_center_x(hitbox, dx);
    hitbox->update_center_y(hitbox, dy);
}

void Enemy_destroy(Elements *self)
{
    Enemy *enemy = (Enemy *)(self->pDerivedObj);
    for (int i = 0; i < 3; i++)
        algif_destroy_animation(enemy->gif_status[i]);
    free(enemy->hitbox);
    free(enemy);
    free(self);
}
