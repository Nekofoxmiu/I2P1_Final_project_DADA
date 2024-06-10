#include "enemy.h"
#include "../shapes/Rectangle.h"
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define MIN_DISTANCE 400
#define MAX_DISTANCE 600
#define OK_RANGE 1
#define M_PI 3.14159265358979323846

void load_enemy_config(const char *filename, EnemyConfig configs[])
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
        int enemyType = -1;
        if (strcmp(type, "slime_L") == 0)
            enemyType = slime_L;
        else if (strcmp(type, "dog_L") == 0)
            enemyType = dog_L;
        else if (strcmp(type, "dragon_L") == 0)
            enemyType = dragon_L;

        if (enemyType != -1)
        {
            if (strcmp(state, "stop") == 0)
                strcpy(configs[enemyType].stop, value);
            else if (strcmp(state, "move") == 0)
                strcpy(configs[enemyType].move, value);
            else if (strcmp(state, "attack") == 0)
                strcpy(configs[enemyType].attack, value);
            else if (strcmp(state, "blood") == 0)
                configs[enemyType].blood = atof(value);
            else if (strcmp(state, "armor") == 0)
                configs[enemyType].armor = atof(value);
            else if (strcmp(state, "damage") == 0)
                configs[enemyType].damage = atof(value);
            else if (strcmp(state, "chase_distance") == 0)
                configs[enemyType].chase_distance = atof(value);
            else if (strcmp(state, "attack_distance") == 0)
                configs[enemyType].attack_distance = atof(value);
            else if (strcmp(state, "chase_speed") == 0)
                configs[enemyType].chase_speed = atof(value);
            else if (strcmp(state, "xp_drop_amount") == 0)
                configs[enemyType].dropConfig.xp = atof(value);
            else if (strcmp(state, "hp_drop_amount") == 0)
                configs[enemyType].dropConfig.hp = atof(value);
            else if (strcmp(state, "mp_drop_amount") == 0)
                configs[enemyType].dropConfig.mp = atof(value);
            else if (strcmp(state, "drop_amount") == 0)
                configs[enemyType].dropConfig.drop_amount = atof(value);
            else if (strcmp(state, "drop_rate") == 0)
                configs[enemyType].dropConfig.drop_rate = atof(value);
        }
    }

    fclose(file);
}

Elements *New_Enemy(int label, EnemyType enemyType, Character *target,
                    double atk_enhance, double def_enhance, double hp_enhance, double chasedis_enhance, double atkdis_enhance, double spd_enhance)
{
    static EnemyConfig configs[100];
    static int configs_loaded = 0;

    if (!configs_loaded)
    {
        load_enemy_config("config/enemy_config.txt", configs);
        configs_loaded = 1;
    }

    Enemy *pDerivedObj = (Enemy *)malloc(sizeof(Enemy));
    Elements *pObj = New_Elements(label);

    // 設置敵人類型
    pDerivedObj->type = (EnemyType)enemyType;

    // 加載動畫
    pDerivedObj->gif_status[STOP] = algif_new_gif(configs[enemyType].stop, -1);
    pDerivedObj->gif_status[MOVE] = algif_new_gif(configs[enemyType].move, -1);
    pDerivedObj->gif_status[ATK] = algif_new_gif(configs[enemyType].attack, -1);

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

    // 根據敵人類型初始化敵人的屬性
    pDerivedObj->blood = configs[enemyType].blood * hp_enhance;
    pDerivedObj->armor = configs[enemyType].armor * def_enhance;
    pDerivedObj->damage = configs[enemyType].damage * atk_enhance;
    pDerivedObj->chase_distance = configs[enemyType].chase_distance * chasedis_enhance;
    pDerivedObj->attack_distance = configs[enemyType].attack_distance * atkdis_enhance;
    pDerivedObj->chase_speed = configs[enemyType].chase_speed * spd_enhance;
    pDerivedObj->dropConfig.xp = configs[enemyType].dropConfig.xp;
    pDerivedObj->dropConfig.hp = configs[enemyType].dropConfig.hp;
    pDerivedObj->dropConfig.mp = configs[enemyType].dropConfig.mp;
    pDerivedObj->dropConfig.drop_amount = configs[enemyType].dropConfig.drop_amount;
    pDerivedObj->dropConfig.drop_rate = configs[enemyType].dropConfig.drop_rate;

    // 初始化其他成員
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

    if (enemy->blood <= 0)
    {
        target->xp += enemy->dropConfig.xp;
        HandleDrop(enemy->dropConfig, scene, enemy->x, enemy->y);
        self->dele = true;
        return;
    }

    // 計算敵人與角色之間的距離
    int dx = target->x - enemy->x;
    int dy = target->y - enemy->y;
    double distance = sqrt(dx * dx + dy * dy);
    int chaseRange_x = 0;
    int chaseRange_y = 0;

    // 解決初始化時碰撞箱不正確問題
    _Enemy_update_position(self, 0, 0);

    // 如果距離小於 chase_distance，則進行追逐
    if (distance < enemy->chase_distance)
    {
        if (distance > enemy->attack_distance)
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

            // 正規化速度
            if (distance != 0)
            {
                chaseRange_x = (int)(enemy->chase_speed * dx / distance);
                chaseRange_y = (int)(enemy->chase_speed * dy / distance);
            }

            _Enemy_update_position(self, chaseRange_x, chaseRange_y);
        }
        else
        {
            enemy->state = ATK;
            if (enemy->gif_status[enemy->state]->done)
            {
                if (target->armor > enemy->damage)
                    target->blood -= 1;
                else
                    target->blood -= enemy->damage - target->armor;
            }
        }
    }
    else
    {
        enemy->state = STOP;
    }
}

void Enemy_draw(Elements *self, float camera_offset_x, float camera_offset_y)
{
    Enemy *enemy = (Enemy *)(self->pDerivedObj);
    ALLEGRO_BITMAP *frame = algif_get_bitmap(enemy->gif_status[enemy->state], al_get_time());
    if (frame)
    {
        // al_draw_bitmap(frame, enemy->x, enemy->y, ((enemy->dir) ? ALLEGRO_FLIP_HORIZONTAL : 0));
        al_draw_bitmap(frame, enemy->x - camera_offset_x, enemy->y - camera_offset_y, ((enemy->dir) ? ALLEGRO_FLIP_HORIZONTAL : 0));
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
