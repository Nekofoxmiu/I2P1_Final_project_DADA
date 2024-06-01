#include "charater.h"
#include "../scene/sceneManager.h"
#include "projectile.h"
#include "../shapes/Rectangle.h"
#include "../algif5/src/algif.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "../global.h"

#define CHARA_SPEED 5


/*
   [Character function]
*/
Elements *New_Character(int label)
{
    Character *pDerivedObj = (Character *)malloc(sizeof(Character));
    Elements *pObj = New_Elements(label);
    // setting derived object member
    // load character images
    char state_string[3][10] = {"stop", "move", "attack"};
    for (int i = 0; i < 3; i++)
    {
        char buffer[53];
        sprintf(buffer, "assets/image/chara_%s.gif", state_string[i]);
        pDerivedObj->gif_status[i] = algif_new_gif(buffer, -1);
    }
    // load effective sound
    ALLEGRO_SAMPLE *sample = al_load_sample("assets/sound/atk_sound.wav");
    pDerivedObj->atk_Sound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(pDerivedObj->atk_Sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(pDerivedObj->atk_Sound, al_get_default_mixer());

    // initial the geometric information of character
    pDerivedObj->width = pDerivedObj->gif_status[0]->width;
    pDerivedObj->height = pDerivedObj->gif_status[0]->height;
    pDerivedObj->x = 300;
    pDerivedObj->y = HEIGHT - pDerivedObj->height - 60;
    pDerivedObj->weapon_dir_x = pDerivedObj->x;
    pDerivedObj->weapon_dir_y = pDerivedObj->y;
    pDerivedObj->hitbox = New_Rectangle(pDerivedObj->x,
                                        pDerivedObj->y,
                                        pDerivedObj->x + pDerivedObj->width,
                                        pDerivedObj->y + pDerivedObj->height);
    pDerivedObj->dir = 'L'; // true: face to right, false: face to left
    // initial the animation component
    pDerivedObj->state = STOP;
    pDerivedObj->new_proj = false;
    pObj->pDerivedObj = pDerivedObj;
    // setting derived object function
    pObj->Draw = Character_draw;
    pObj->Update = Character_update;
    pObj->Interact = Character_interact;
    pObj->Destroy = Character_destory;
    pObj->GetPosition = Character_get_position;
    return pObj;
}

void Character_update(Elements *self)
{
    // 使用有限狀態機的概念處理不同狀態
    Character *chara = ((Character *)(self->pDerivedObj));

    // 解決奇怪的初始化時碰撞箱似乎不正確問題(非常醜的解法可以的話最好改掉就是)
    _Character_update_position(self, 0, 0);

    if (chara->state == STOP)
    {
        if (key_state[ALLEGRO_KEY_SPACE])
        {
            chara->state = ATK;
        }
        else if (key_state[ALLEGRO_KEY_A] || key_state[ALLEGRO_KEY_D] || key_state[ALLEGRO_KEY_W] || key_state[ALLEGRO_KEY_S])
        {
            chara->state = MOVE;
        }
    }
    else if (chara->state == MOVE)
    {
        if (key_state[ALLEGRO_KEY_SPACE])
        {
            chara->state = ATK;
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
            float speed = CHARA_SPEED;
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
        if (chara->gif_status[ATK]->display_index == 2 && chara->new_proj == false)
        {
            double offset_x = (chara->dir == 'R') ? chara->width - 50 : -50;
            double offset_y = 10;
            double weapon_x = chara->x + offset_x;
            double weapon_y = chara->y + offset_y;

            double dx = mouse.x - weapon_x;
            double dy = mouse.y - weapon_y;
            double len = sqrt(dx * dx + dy * dy);

            if (len != 0)
            {
                chara->weapon_dir_x = dx / len;
                chara->weapon_dir_y = dy / len;
            }
            else
            {
                chara->weapon_dir_x = 1;
                chara->weapon_dir_y = 0;
            }

            Elements *pro;
            pro = New_Projectile(Projectile_L,
                                 weapon_x,
                                 weapon_y,
                                 10, chara->weapon_dir_x, chara->weapon_dir_y);
            _Register_elements(scene, pro);
            chara->new_proj = true;
        }
    }
}



void Character_draw(Elements *self)
{
    // with the state, draw corresponding image
    Character *chara = ((Character *)(self->pDerivedObj));
    ALLEGRO_BITMAP *frame = algif_get_bitmap(chara->gif_status[chara->state], al_get_time());
    if (frame)
    {
        al_draw_bitmap(frame, chara->x, chara->y, ((chara->dir == 'R') ? ALLEGRO_FLIP_HORIZONTAL : 0));
    }
    if (chara->state == ATK && chara->gif_status[chara->state]->display_index == 2)
    {
        al_play_sample_instance(chara->atk_Sound);
    }
}

void Character_destory(Elements *self)
{
    Character *Obj = ((Character *)(self->pDerivedObj));
    al_destroy_sample_instance(Obj->atk_Sound);
    for (int i = 0; i < 3; i++)
        algif_destroy_animation(Obj->gif_status[i]);
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
}

void Character_interact(Elements *self, Elements *tar) {}
