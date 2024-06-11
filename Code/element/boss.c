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

void load_boss_config(const char *filename, BossConfig configs[])
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Could not open config file: %s\n", filename);
        exit(1);
    }

    char type[20], state[20], value[100];
    while (fscanf(file, "%s %s %s", type, state, value) != EOF)
    {
        int bossType = -1;
        if (strcmp(type, "default_boss_L") == 0)
            bossType = default_boss_L;

        if (bossType != -1)
        {
            if (strcmp(state, "stop") == 0)
                strcpy(configs[bossType].stop, value);
            else if (strcmp(state, "move") == 0)
                strcpy(configs[bossType].move, value);
            else if (strcmp(state, "attack") == 0)
                strcpy(configs[bossType].attack, value);
            else if (strcmp(state, "blood") == 0)
                configs[bossType].blood = atof(value);
            else if (strcmp(state, "armor") == 0)
                configs[bossType].armor = atof(value);
            else if (strcmp(state, "damage") == 0)
                configs[bossType].damage = atof(value);
            else if (strcmp(state, "attack_distance") == 0)
                configs[bossType].attack_distance = atof(value);
            else if (strcmp(state, "xp_drop_amount") == 0)
                configs[bossType].DropConfig.xp = atof(value);
            else if (strcmp(state, "hp_drop_amount") == 0)
                configs[bossType].DropConfig.hp = atof(value);
            else if (strcmp(state, "mp_drop_amount") == 0)
                configs[bossType].DropConfig.mp = atof(value);
            else if (strcmp(state, "drop_amount") == 0)
                configs[bossType].DropConfig.drop_amount = atof(value);
            else if (strcmp(state, "drop_rate") == 0)
                configs[bossType].DropConfig.drop_rate = atof(value);
        }
    }

    fclose(file);
}

Elements *New_Boss(int label, Character *target)
{
    static BossConfig configs[100];
    static int configs_loaded = 0;

    if (!configs_loaded)
    {
        load_boss_config("config/boss_config.txt", configs);
        configs_loaded = 1;
    }

    Boss *pDerivedObj = (Boss *)malloc(sizeof(Boss));
    Elements *pObj = New_Elements(label);

    // 加載動畫
    pDerivedObj->gif_status[STOP] = algif_new_gif(configs[default_boss_L].stop, -1);
    pDerivedObj->gif_status[MOVE] = algif_new_gif(configs[default_boss_L].move, -1);
    pDerivedObj->gif_status[ATK] = algif_new_gif(configs[default_boss_L].attack, -1);

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
    if(pDerivedObj->x < 20){
        pDerivedObj->x = 20;
    }
    if(pDerivedObj->x > 4076){
        pDerivedObj->x = 4076;
    }
    if(pDerivedObj->y < 20){
        pDerivedObj->y = 20;
    }
    if(pDerivedObj->y > 4076){
        pDerivedObj->y = 4076;
    }

    // 根據敵人類型初始化敵人的屬性
    pDerivedObj->blood = configs[default_boss_L].blood;
    pDerivedObj->armor = configs[default_boss_L].armor;
    pDerivedObj->damage = configs[default_boss_L].damage;
    pDerivedObj->attack_distance = configs[default_boss_L].attack_distance;
    pDerivedObj->DropConfig.xp = configs[default_boss_L].DropConfig.xp;
    pDerivedObj->DropConfig.hp = configs[default_boss_L].DropConfig.hp;
    pDerivedObj->DropConfig.mp = configs[default_boss_L].DropConfig.mp;
    pDerivedObj->DropConfig.drop_amount = configs[default_boss_L].DropConfig.drop_amount;
    pDerivedObj->DropConfig.drop_rate = configs[default_boss_L].DropConfig.drop_rate;

    // 初始化敵人的其他成員
    pDerivedObj->width = pDerivedObj->gif_status[0]->width;
    pDerivedObj->height = pDerivedObj->gif_status[0]->height;
    pDerivedObj->dir = true;      // 初始方向
    pDerivedObj->state = STOP;    // 初始狀態
    pDerivedObj->target = target; // 設定目標角色
    pDerivedObj->anime = 0;
    pDerivedObj->anime_time = 0;
    pDerivedObj->hitbox = New_Rectangle(pDerivedObj->x, pDerivedObj->y,
                                        pDerivedObj->x + pDerivedObj->width,
                                        pDerivedObj->y + pDerivedObj->height);
    pDerivedObj->nextattack = true;
    pDerivedObj->aura_dmg_start_time = 0;
    pDerivedObj->aura_dmg_elapsed_time = 0;

    pObj->pDerivedObj = pDerivedObj;
    pObj->Draw = Boss_draw;
    pObj->Update = Boss_update;
    pObj->Destroy = Boss_destroy;
    return pObj;
}

void Boss_update(Elements *self)
{
    static int prepause_state = -1;
    Boss *boss = (Boss *)(self->pDerivedObj);
    if (everything_stop)
    {
        boss->state = STOP;
        return;
    }
    else
    {
        if (prepause_state != -1)
        {
            boss->state = prepause_state;
        }
    }
    Character *target = boss->target;

    if (boss->blood <= 0)
    {
        HandleDrop(boss->DropConfig, target, scene, boss->x, boss->y);
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
            // Attack_Radial(self, 10, 5, 2);
            Attack_Normal(self, 2, 5, true);
            boss->new_proj = true;
        }
    }
    prepause_state = boss->state;

    double current_time = al_get_time();

    if (target->aura && distance <= target->aura_dis) {
        if (boss->aura_dmg_start_time == 0) {
            boss->aura_dmg_start_time = current_time;
        }
        else{
            boss->aura_dmg_elapsed_time = current_time - boss->aura_dmg_start_time;
            if (boss->aura_dmg_elapsed_time > 1) {
                boss->blood -= target->aura_dmg;
                printf("boss blood: %f\n", boss->blood);
                boss->aura_dmg_start_time = current_time;
            }
        }
    } else {
        boss->aura_dmg_start_time = 0;
    }
}

void Boss_draw(Elements *self, float camera_offset_x, float camera_offset_y)
{
    Boss *boss = (Boss *)(self->pDerivedObj);
    ALLEGRO_BITMAP *frame = algif_get_bitmap(boss->gif_status[boss->state], al_get_time());
    if (frame)
    {
        // al_draw_bitmap(frame, boss->x, boss->y, ((boss->dir) ? ALLEGRO_FLIP_HORIZONTAL : 0));
        al_draw_bitmap(frame, boss->x - camera_offset_x, boss->y - camera_offset_y, ((boss->dir) ? ALLEGRO_FLIP_HORIZONTAL : 0));
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
