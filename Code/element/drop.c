#include "drop.h"
#include "../shapes/circle.h"
/*
   [drop function]
*/
Elements *New_Drop(int label, DropType type, Character *target, int x, int y, double amount)
{
    Drop *pDerivedObj = (Drop *)malloc(sizeof(Drop));
    Elements *pObj = New_Elements(label);
    // setting derived object member

    if (type == XP_L) {
        pDerivedObj->img = al_load_bitmap("assets/image/red_potion.png");
    } else if (type == HP_L) {
        pDerivedObj->img = al_load_bitmap("assets/image/red_potion.png");
    } else if (type == MP_L) {
        pDerivedObj->img = al_load_bitmap("assets/image/red_potion.png");
    }

    // load effective sound
    ALLEGRO_SAMPLE *sample = al_load_sample("assets/sound/atk_sound.wav");
    pDerivedObj->pick_Sound = al_create_sample_instance(sample);
    al_set_sample_instance_playmode(pDerivedObj->pick_Sound, ALLEGRO_PLAYMODE_ONCE);
    al_attach_sample_instance_to_mixer(pDerivedObj->pick_Sound, al_get_default_mixer());
    
    pDerivedObj->width = al_get_bitmap_width(pDerivedObj->img);
    pDerivedObj->height = al_get_bitmap_height(pDerivedObj->img);
    pDerivedObj->x = x;
    pDerivedObj->y = y;
    pDerivedObj->hitbox = New_Circle(pDerivedObj->x + pDerivedObj->width / 3,
                                        pDerivedObj->y + pDerivedObj->height / 3,
                                        min(pDerivedObj->width, pDerivedObj->height) / 2);
    pDerivedObj->type = type;
    pDerivedObj->life = 0;
    pDerivedObj->amount = amount;
    pDerivedObj->attract_distance = 200;
    pDerivedObj->attract_speed = 6;
    pDerivedObj->target = target;


    // setting the interact object
    pObj->inter_obj[pObj->inter_len++] = Character_L;
    // setting derived object function
    pObj->pDerivedObj = pDerivedObj;

    pObj->Update = Drop_update;
    pObj->Interact = Drop_interact;
    pObj->Draw = Drop_draw;
    pObj->Destroy = Drop_destory;
    return pObj;
}
void Drop_update(Elements *self) {
    if(everything_stop) return;
    Drop *Obj = ((Drop *)(self->pDerivedObj));
    Character *target = Obj->target;

    Obj->life++;
    if (Obj->life >= 1000) {
        self->dele = true;
    }

    // 計算物品與角色之間的距離
    int dx = target->x - Obj->x;
    int dy = target->y - Obj->y;
    double distance = sqrt(dx * dx + dy * dy);
    int attractRange_x = 0;
    int attractRange_y = 0;

    if (distance < Obj->attract_distance) 
    {
        if (distance != 0)
        {
            attractRange_x = Obj->attract_speed * dx / distance;
            attractRange_y = Obj->attract_speed * dy / distance;
        }

        _Drop_update_position(self, attractRange_x, attractRange_y);
    }

    
}
void Drop_interact(Elements *self, Elements *tar) {
    Drop *Obj = ((Drop *)(self->pDerivedObj));
    Character *chara = ((Character *)(tar->pDerivedObj));
    if (Obj->hitbox->overlap(Obj->hitbox, chara->hitbox)) {
        al_play_sample_instance(Obj->pick_Sound);
        Character *chara = (Character *)tar->pDerivedObj;
        if (Obj->type == XP_L) {
            chara->xp += Obj->amount;
        } else if (Obj->type == HP_L) {
            chara->blood += Obj->amount;
        } else if (Obj->type == MP_L) {
            chara->mp += Obj->amount;
        }
        self->dele = true;
    }
}
void Drop_draw(Elements *self, float camera_offset_x, float camera_offset_y)
{
    Drop *Obj = ((Drop *)(self->pDerivedObj));
    //al_draw_bitmap(Obj->img, Obj->x, Obj->y, 0);
    al_draw_bitmap(Obj->img, Obj->x - camera_offset_x, Obj->y - camera_offset_y, 0);
}
void Drop_destory(Elements *self)
{
    Drop *Obj = ((Drop *)(self->pDerivedObj));
    al_destroy_bitmap(Obj->img);
    free(Obj->hitbox);
    free(Obj);
    free(self);
}

void HandleDrop(DropConfig dropConfig, Character *target, Scene *scene, double x, double y)
{

    for (int i = 0; i < dropConfig.drop_amount; i++)
    {
        if ((rand() % 100) < dropConfig.drop_rate)
        {
            // 隨機化生成類型、位置
            int drop_type = rand() % 3;
            int drop_x = x + (rand() % 50) - 25;
            int drop_y = y + (rand() % 50) - 25;

            if (drop_x < 0 + 64)
                drop_x = 0 + 64;
            if (drop_y < 0 + 64)
                drop_y = 0 + 64;
            if (drop_x > WORLD_WIDTH - 64)
                drop_x = WORLD_WIDTH - 64;
            if (drop_y > WORLD_HEIGHT - 64)
                drop_y = WORLD_HEIGHT - 64;

            if (drop_type == XP_L)
            {
                Elements *drop = New_Drop(Drop_L, XP_L, target, drop_x, drop_y, dropConfig.xp);
                _Register_elements(scene, drop);
            }
            else if (drop_type == HP_L)
            {
                Elements *drop = New_Drop(Drop_L, HP_L, target, drop_x, drop_y, dropConfig.hp);
                _Register_elements(scene, drop);
            }
            else if (drop_type == MP_L)
            {
                Elements *drop = New_Drop(Drop_L, MP_L, target, drop_x, drop_y, dropConfig.mp);
                _Register_elements(scene, drop);
            }
        }
    }
}


void HandleOneTypeDrop(DropConfig dropConfig, Character *target, Scene *scene, double x, double y, int type)
{
    for (int i = 0; i < dropConfig.drop_amount; i++)
    {
        if ((rand() % 100) < dropConfig.drop_rate)
        {
            // 隨機化生成類型、位置
            int drop_x = x + (rand() % 50) - 25;
            int drop_y = y + (rand() % 50) - 25;

            if (drop_x < 0 + 64)
                drop_x = 0 + 64;
            if (drop_y < 0 + 64)
                drop_y = 0 + 64;
            if (drop_x > WORLD_WIDTH - 64)
                drop_x = WORLD_WIDTH - 64;
            if (drop_y > WORLD_HEIGHT - 64)
                drop_y = WORLD_HEIGHT - 64;

            if (type == XP_L)
            {
                Elements *drop = New_Drop(Drop_L, XP_L, target, drop_x, drop_y, dropConfig.xp);
                _Register_elements(scene, drop);
            }
            else if (type == HP_L)
            {
                Elements *drop = New_Drop(Drop_L, HP_L, target, drop_x, drop_y, dropConfig.hp);
                _Register_elements(scene, drop);
            }
            else if (type == MP_L)
            {
                Elements *drop = New_Drop(Drop_L, MP_L, target, drop_x, drop_y, dropConfig.mp);
                _Register_elements(scene, drop);
            }
        }
    }
}


void _Drop_update_position(Elements *self, int dx, int dy)
{
    Drop *drop = ((Drop *)(self->pDerivedObj));
    drop->x += dx;
    drop->y += dy;
    Shape *hitbox = drop->hitbox;
    hitbox->update_center_x(hitbox, dx);
    hitbox->update_center_y(hitbox, dy);
}