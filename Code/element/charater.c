#include "charater.h"
#include "../scene/sceneManager.h"
#include "projectile.h"
#include "../shapes/Rectangle.h"
#include "../algif5/src/algif.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <string.h>
#include "../global.h"
#include "levelup.h"

void load_character_config(const char *filename, CharacterConfig config[])
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

        int charaType = -1;
        if (strcmp(type, "default_chara_L") == 0)
            charaType = default_chara_L;
        else if (strcmp(type, "fire_chara_L") == 0)
            charaType = fire_chara_L;

        if (charaType != -1)
        {
            if (strcmp(state, "stop") == 0)
                strcpy(config[charaType].stop, value);
            else if (strcmp(state, "move") == 0)
                strcpy(config[charaType].move, value);
            else if (strcmp(state, "attack") == 0)
                strcpy(config[charaType].attack, value);
            else if (strcmp(state, "skill") == 0)
                strcpy(config[charaType].attack, value);
            else if (strcmp(state, "weapon_stop") == 0)
                strcpy(config[charaType].weapon_stop, value);
            else if (strcmp(state, "weapon_attack") == 0)
                strcpy(config[charaType].weapon_attack, value);
            else if (strcmp(state, "blood") == 0)
                config[charaType].blood = atof(value);
            else if (strcmp(state, "armor") == 0)
                config[charaType].armor = atof(value);
            else if (strcmp(state, "damage") == 0)
                config[charaType].damage = atof(value);
            else if (strcmp(state, "speed") == 0)
                config[charaType].speed = atof(value);
            else if (strcmp(state, "mp") == 0)
                config[charaType].mp = atof(value);
            else if (strcmp(state, "xp") == 0)
                config[charaType].xp = atof(value);
            else if (strcmp(state, "aura_dis") == 0)
                config[charaType].aura_dis = atof(value);
            else if (strcmp(state, "aura_cool") == 0)
                config[charaType].aura_cool = atof(value);
            else if (strcmp(state, "aura_time") == 0)
                config[charaType].aura_time = atof(value);
            else if (strcmp(state, "aura_dmg") == 0)
                config[charaType].aura_dmg = atof(value);
        }
    }

    fclose(file);
}

Elements *New_Character(int label, CharacterType charaType)
{
    static CharacterConfig configs[100];
    static int config_loaded = 0;

    if (!config_loaded)
    {
        load_character_config("config/character_config.txt", configs);
        config_loaded = 1;
    }

    Character *pDerivedObj = (Character *)malloc(sizeof(Character));
    Elements *pObj = New_Elements(label);

    // 設置主角類型
    pDerivedObj->type = (CharacterType)charaType;

    // 加載動畫
    pDerivedObj->gif_status[STOP] = algif_new_gif(configs[charaType].stop, -1);
    pDerivedObj->gif_status[MOVE] = algif_new_gif(configs[charaType].move, -1);
    pDerivedObj->gif_status[ATK] = algif_new_gif(configs[charaType].attack, -1);
    pDerivedObj->gif_status[SKILL] = algif_new_gif(configs[charaType].attack, -1);
    pDerivedObj->gif_status[RADIAL_ATTACK] = algif_new_gif(configs[charaType].attack, -1);

    // load weapon
    pDerivedObj->weapon = al_load_bitmap(configs[charaType].weapon_stop);
    pDerivedObj->weapon_attack = algif_new_gif(configs[charaType].weapon_attack, -1);

    // load skill overlay
    pDerivedObj->skill_overlay = al_load_bitmap("assets/image/skill_overlay.png");

    // load effective sound
    ALLEGRO_SAMPLE *sample = al_load_sample("assets/sound/chara_atk.wav");
    pDerivedObj->atk_Sound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(pDerivedObj->atk_Sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(pDerivedObj->atk_Sound, al_get_default_mixer());

    // 初始化角色的其他成員
    pDerivedObj->width = pDerivedObj->gif_status[0]->width;
    pDerivedObj->height = pDerivedObj->gif_status[0]->height;
    pDerivedObj->x = 1300;
    pDerivedObj->y = HEIGHT + 800;
    pDerivedObj->weapon_x = pDerivedObj->x;
    pDerivedObj->weapon_y = pDerivedObj->y;
    pDerivedObj->level = 0;
    pDerivedObj->levelExpNeed = 100;
    pDerivedObj->ene_level = 1.1;
    pDerivedObj->hitbox = New_Rectangle(pDerivedObj->x,
                                        pDerivedObj->y,
                                        pDerivedObj->x + pDerivedObj->width,
                                        pDerivedObj->y + pDerivedObj->height);
    pDerivedObj->dir = 'L'; // 初始方向
    pDerivedObj->aura = false;

    // setting the interact object
    // pObj->inter_obj[pObj->inter_len++] = Tree_L;

    // 根據配置文件初始化屬性
    pDerivedObj->max_blood = configs[charaType].blood;
    pDerivedObj->blood = configs[charaType].blood;
    pDerivedObj->armor = configs[charaType].armor;
    pDerivedObj->damage = configs[charaType].damage;
    pDerivedObj->speed = configs[charaType].speed;
    pDerivedObj->max_mp = configs[charaType].mp;
    pDerivedObj->mp = configs[charaType].mp;
    pDerivedObj->xp = configs[charaType].xp;
    pDerivedObj->bullet_num = 1;
    pDerivedObj->radial_bullet_num = 10;
    pDerivedObj->aura_dis = configs[charaType].aura_dis;
    pDerivedObj->aura_cool = configs[charaType].aura_cool;
    pDerivedObj->aura_time = configs[charaType].aura_time;
    pDerivedObj->aura_dmg = configs[charaType].aura_dmg;

    pDerivedObj->aura_start_time = 0;
    pDerivedObj->aura_elapsed_time = 0;
    pDerivedObj->aura_usable = true;

    // 初始化動畫成員
    pDerivedObj->state = STOP;
    pDerivedObj->new_proj = false;

    // sound
    pDerivedObj->levelup = al_load_sample("assets/sound/levelup.wav");
    if (pDerivedObj->levelup == NULL) printf("Error");
    al_reserve_samples(20);
    pDerivedObj->sample_levelup = al_create_sample_instance(pDerivedObj->levelup);
    // Loop the song until the display closes
    al_set_sample_instance_playmode(pDerivedObj->sample_levelup, ALLEGRO_PLAYMODE_ONCE);
    al_restore_default_mixer();
    al_attach_sample_instance_to_mixer(pDerivedObj->sample_levelup, al_get_default_mixer());
    // set the volume of instance
    al_set_sample_instance_gain(pDerivedObj->sample_levelup, 0.7);

    pObj->pDerivedObj = pDerivedObj;
    // 設定派生對象的函數
    pObj->Draw = Character_draw;
    pObj->Update = Character_update;
    pObj->Interact = Character_interact;
    pObj->Destroy = Character_destory;
    pObj->GetPosition = Character_get_position;
    return pObj;
}

void Character_update(Elements *self)
{
    static int prepause_state = -1;
    Character *chara = ((Character *)(self->pDerivedObj));
    if (everything_stop)
    {
        chara->state = STOP;
        return;
    }
    else
    {
        if (prepause_state != -1)
        {
            chara->state = prepause_state;
        }
    }

    // 升級邏輯
    if (chara->xp >= chara->levelExpNeed)
    {
        chara->level++;
        chara->xp -= chara->levelExpNeed;
        chara->levelExpNeed = chara->levelExpNeed * chara->ene_level;
        level_up = true;
        al_play_sample_instance(chara->sample_levelup);
        Elements *levelup = New_Levelup(Levelup_L, chara);
        _Register_elements(scene, levelup);
        everything_stop = true;
    }

    // 使用有限狀態機的概念處理不同狀態
    float speed = chara->speed;

    // 解決初始化時碰撞箱不正確問題
    _Character_update_position(self, 0, 0);

    if (chara->state == STOP)
    {
        // mouse_state[1] 左鍵 2 右鍵 3 中鍵
        if (mouse_state[1])
        {
            chara->state = ATK;
        }
        else if (key_state[ALLEGRO_KEY_A] || key_state[ALLEGRO_KEY_D] || key_state[ALLEGRO_KEY_W] || key_state[ALLEGRO_KEY_S])
        {
            chara->state = MOVE;
        }
        else if ((mouse_state[2] || mouse_state[3]) && chara->aura_usable)
        {
            chara->state = SKILL;
        }
        else if (key_state[ALLEGRO_KEY_SPACE])
        {
            chara->state = RADIAL_ATTACK;
        }
    }
    else if (chara->state == MOVE)
    {
        if (mouse_state[1])
        {
            chara->state = ATK;
        }
        else if ((mouse_state[2] || mouse_state[3]) && chara->aura_usable)
        {
            chara->state = SKILL;
        }
        else if (key_state[ALLEGRO_KEY_SPACE])
        {
            chara->state = RADIAL_ATTACK;
        }
        else
        {
            int dx = 0, dy = 0;
            if (key_state[ALLEGRO_KEY_A])
            {
                chara->dir = 'L';
                dx -= 1;
            }
            if (key_state[ALLEGRO_KEY_D])
            {
                chara->dir = 'R';
                dx += 1;
            }
            if (key_state[ALLEGRO_KEY_W])
            {
                dy -= 1;
            }
            if (key_state[ALLEGRO_KEY_S])
            {
                dy += 1;
            }

            // 計算單位移動向量並乘以固定速度
            float length = sqrt(dx * dx + dy * dy);
            if (length != 0)
            {
                dx = (int)(dx / length * speed);
                dy = (int)(dy / length * speed);
            }

            _Character_update_position(self, dx, dy);
            if (!key_state[ALLEGRO_KEY_A] && !key_state[ALLEGRO_KEY_D] && !key_state[ALLEGRO_KEY_W] && !key_state[ALLEGRO_KEY_S])
            {
                chara->state = STOP;
            }
        }
    }
    else if (chara->state == ATK)
    {
        if (chara->gif_status[chara->state]->done)
        {
            chara->state = STOP;
            chara->new_proj = false;
        }
        else
        {
            int dx = 0, dy = 0;
            if (key_state[ALLEGRO_KEY_A])
            {
                chara->dir = 'L';
                dx -= 1;
            }
            if (key_state[ALLEGRO_KEY_D])
            {
                chara->dir = 'R';
                dx += 1;
            }
            if (key_state[ALLEGRO_KEY_W])
            {
                dy -= 1;
            }
            if (key_state[ALLEGRO_KEY_S])
            {
                dy += 1;
            }

            // 計算單位移動向量並乘以固定速度
            float length = sqrt(dx * dx + dy * dy);
            if (length != 0)
            {
                dx = (int)(dx / length * speed);
                dy = (int)(dy / length * speed);
            }

            _Character_update_position(self, dx, dy);
        }

        // GIF 速度調快的時候偵測的 Index 要像後調或去掉這個條件
        if (chara->gif_status[ATK]->display_index == 2 && chara->new_proj == false)
        {
            Attack_Normal(self, chara->bullet_num, 10, false);
            chara->new_proj = true;
        }
    }
    else if (chara->state == SKILL)
    {
        if (chara->gif_status[chara->state]->done)
        {
            chara->state = STOP;
            chara->new_proj = false;
        }
        else
        {
            int dx = 0, dy = 0;
            if (key_state[ALLEGRO_KEY_A])
            {
                chara->dir = 'L';
                dx -= 1;
            }
            if (key_state[ALLEGRO_KEY_D])
            {
                chara->dir = 'R';
                dx += 1;
            }
            if (key_state[ALLEGRO_KEY_W])
            {
                dy -= 1;
            }
            if (key_state[ALLEGRO_KEY_S])
            {
                dy += 1;
            }

            // 計算單位移動向量並乘以固定速度
            float length = sqrt(dx * dx + dy * dy);
            if (length != 0)
            {
                dx = (int)(dx / length * speed);
                dy = (int)(dy / length * speed);
            }

            _Character_update_position(self, dx, dy);
        }
    }
    else if (chara->state == RADIAL_ATTACK)
    {
        if (chara->gif_status[chara->state]->done)
        {
            chara->state = STOP;
            chara->new_proj = false;
        }
        else
        {
            int dx = 0, dy = 0;
            if (key_state[ALLEGRO_KEY_A])
            {
                chara->dir = 'L';
                dx -= 1;
            }
            if (key_state[ALLEGRO_KEY_D])
            {
                chara->dir = 'R';
                dx += 1;
            }
            if (key_state[ALLEGRO_KEY_W])
            {
                dy -= 1;
            }
            if (key_state[ALLEGRO_KEY_S])
            {
                dy += 1;
            }

            // 計算單位移動向量並乘以固定速度
            float length = sqrt(dx * dx + dy * dy);
            if (length != 0)
            {
                dx = (int)(dx / length * speed);
                dy = (int)(dy / length * speed);
            }

            _Character_update_position(self, dx, dy);

            if (chara->gif_status[RADIAL_ATTACK]->display_index == 2 && chara->new_proj == false)
            {
                if(chara->mp >= 1)
                {
                    chara->mp -= 1;
                    Attack_Radial(self, chara->radial_bullet_num, 5, 1);
                    chara->new_proj = true;
                }
            }
        }
    }

    // 確認技能是否處於激活狀態
    if (chara->gif_status[SKILL]->display_index == 2)
    {
        if (chara->mp >= 20 && !chara->aura && chara->aura_usable)
        {
            chara->mp -= 20;
            chara->aura = true;
            chara->aura_usable = false;
            chara->aura_start_time = al_get_time();
        }
    }

        // 處理技能持續時間
        if (chara->aura && chara->aura_start_time != 0)
        {
            double current_time = al_get_time();
            chara->aura_elapsed_time = current_time - chara->aura_start_time;
            //printf("aura_elapsed_time: %f\n", chara->aura_elapsed_time);
            if (chara->aura_elapsed_time > chara->aura_time)
            {
                chara->aura = false;
                chara->aura_start_time = 0;  // 重設開始時間
                chara->aura_elapsed_time = 0;
            }
        }

    // 處理技能冷卻時間
    if (!chara->aura && !chara->aura_usable && chara->aura_start_time == 0)
    {
        chara->aura_start_time = al_get_time(); // 開始冷卻時間
    }

    if (!chara->aura && !chara->aura_usable && chara->aura_start_time != 0)
    {
        double current_time = al_get_time();
        chara->aura_elapsed_time = current_time - chara->aura_start_time;
        if (chara->aura_elapsed_time > chara->aura_cool)
        {
            chara->aura_usable = true;
            chara->aura_start_time = 0;
            chara->aura_elapsed_time = 0;
        }
    }

    prepause_state = chara->state;
}

void Character_draw(Elements *self, float camera_offset_x, float camera_offset_y)
{
    Character *chara = ((Character *)(self->pDerivedObj));

    if (chara->aura && chara->aura_start_time != 0)
    {
        // 獲取光環大小
        float aura_size = chara->aura_dis;

        // 設置半透明度並縮放貼圖
        ALLEGRO_BITMAP *skill_overlay = chara->skill_overlay;
        if (skill_overlay)
        {
            float overlay_width = al_get_bitmap_width(skill_overlay);
            float overlay_height = al_get_bitmap_height(skill_overlay);

            al_draw_tinted_scaled_bitmap(skill_overlay,
                                         al_map_rgba_f(1.0, 1.0, 1.0, 0.5),
                                         0, 0, overlay_width, overlay_height,
                                         chara->x + chara->width / 2 - camera_offset_x - aura_size / 2,
                                         chara->y + chara->height / 2 - camera_offset_y - aura_size / 2,
                                         aura_size, aura_size,
                                         0);
        }
    }

    ALLEGRO_BITMAP *char_frame = algif_get_bitmap(chara->gif_status[chara->state], al_get_time());
    if (char_frame)
    {
        al_draw_bitmap(char_frame, chara->x - camera_offset_x, chara->y - camera_offset_y, ((chara->dir == 'R') ? ALLEGRO_FLIP_HORIZONTAL : 0));
    }

    if (chara->state == ATK)
    {
        ALLEGRO_BITMAP *weapon_frame = algif_get_bitmap(chara->weapon_attack, al_get_time());
        if (weapon_frame)
        {
            al_draw_bitmap(weapon_frame, chara->weapon_x - camera_offset_x, chara->weapon_y - camera_offset_y, ((chara->dir == 'R') ? ALLEGRO_FLIP_HORIZONTAL : 0));
        }

        if (chara->gif_status[ATK]->display_index == 3)
        {
            al_play_sample_instance(chara->atk_Sound);
        }
    }
    else
    {
        al_draw_bitmap(chara->weapon, chara->weapon_x - camera_offset_x, chara->weapon_y - camera_offset_y, ((chara->dir == 'R') ? ALLEGRO_FLIP_HORIZONTAL : 0));
    }
}

void Character_destory(Elements *self)
{
    Character *Obj = ((Character *)(self->pDerivedObj));
    al_destroy_sample_instance(Obj->atk_Sound);
    al_destroy_sample_instance(Obj->sample_levelup);
    for (int i = 0; i < 3; i++)
    {
        algif_destroy_animation(Obj->gif_status[i]);
    }
    al_destroy_bitmap(Obj->weapon);
    algif_destroy_animation(Obj->weapon_attack);
    free(Obj->hitbox);
    free(Obj);
    free(self);
}

void Character_get_position(Elements *self, float *x, float *y)
{
    Character *chara = (Character *)(self->pDerivedObj);
    *x = chara->x;
    *y = chara->y;
}

void _Character_update_position(Elements *self, int dx, int dy)
{
    Character *chara = ((Character *)(self->pDerivedObj));
    chara->x += dx;
    chara->y += dy;
    Shape *hitbox = chara->hitbox;
    hitbox->update_center_x(hitbox, dx);
    hitbox->update_center_y(hitbox, dy);
    if (chara->dir == 'L')
    {
        chara->weapon_x = chara->x - 10;
    }
    else
    {
        chara->weapon_x = chara->x + 40;
    }
    chara->weapon_y = chara->y + 15;
}

/*
void Character_interact(Elements *self, Elements *tar)
{
    Character *chara = ((Character *)(self->pDerivedObj));
    if (tar->label == Tree_L)
    {
        Tree *tree = ((Tree *)(tar->pDerivedObj));
        if (tree->hitbox->overlap(tree->hitbox, chara->hitbox))
        {
            if(chara->x > tree->x){
                _Character_update_position(self, 5, 0);
            }
            if(chara->x < tree->x){
                _Character_update_position(self, -5, 0);
            }
            if(chara->y > tree->y){
                _Character_update_position(self, 0, 5);
            }
            if(chara->y < tree->y){
                _Character_update_position(self, 0, -5);
            }
        }
    }
}
*/

void Character_interact(Elements *self, Elements *tar) {}