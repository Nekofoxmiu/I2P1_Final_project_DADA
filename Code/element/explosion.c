#include "explosion.h"
/*
   [Explosion function]
*/
Elements *New_Explosion(int label, double x, double y)
{
    Explosion *pDerivedObj = (Explosion *)malloc(sizeof(Explosion));
    Elements *pObj = New_Elements(label);
    // setting derived object member
    pDerivedObj->explosion = algif_new_gif("assets/image/hit_explosion.gif", -1);
    pDerivedObj->x = x;
    pDerivedObj->y = y;

    // setting derived object function
    pObj->pDerivedObj = pDerivedObj;
    pObj->Update = Explosion_update;
    pObj->Interact = Explosion_interact;
    pObj->Draw = Explosion_draw;
    pObj->Destroy = Explosion_destory;
    return pObj;
}
void Explosion_update(Elements *self) {
    Explosion *Obj = ((Explosion *)(self->pDerivedObj));
    if (Obj->explosion->display_index == 3) {
        self->dele = true;
    }
}
void Explosion_interact(Elements *self, Elements *tar) {}
void Explosion_draw(Elements *self, float camera_offset_x, float camera_offset_y)
{
    Explosion *Obj = ((Explosion *)(self->pDerivedObj));
    ALLEGRO_BITMAP *frame = algif_get_bitmap(Obj->explosion, al_get_time());
    //al_draw_bitmap(Obj->img, Obj->x, Obj->y, 0);
    if (frame) {
        al_draw_bitmap(frame, Obj->x - camera_offset_x, Obj->y - camera_offset_y, 0);
    }
}
void Explosion_destory(Elements *self)
{
    Explosion *Obj = ((Explosion *)(self->pDerivedObj));
    algif_destroy_animation(Obj->explosion);
    free(Obj);
    free(self);
}
