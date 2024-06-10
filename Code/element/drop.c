#include "drop.h"
#include "../shapes/circle.h"
/*
   [drop function]
*/
Elements *New_Drop(int label, DropType type, int x, int y)
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
    Drop *Obj = ((Drop *)(self->pDerivedObj));
    Obj->life++;
    if (Obj->life >= 1000) {
        self->dele = true;
    }
}
void Drop_interact(Elements *self, Elements *tar) {
    Drop *Obj = ((Drop *)(self->pDerivedObj));
    Character *chara = ((Character *)(tar->pDerivedObj));
    if (Obj->hitbox->overlap(Obj->hitbox, chara->hitbox)) {
        al_play_sample_instance(Obj->pick_Sound);
        self->dele = true;
    }
}
void Drop_draw(Elements *self)
{
    Drop *Obj = ((Drop *)(self->pDerivedObj));
    al_draw_bitmap(Obj->img, Obj->x, Obj->y, 0);
}
void Drop_destory(Elements *self)
{
    Drop *Obj = ((Drop *)(self->pDerivedObj));
    al_destroy_bitmap(Obj->img);
    free(Obj->hitbox);
    free(Obj);
    free(self);
}
