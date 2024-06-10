#include "levelup.h"
/*
   [levelup function]
*/
Elements *New_Levelup(int label, Character *player)
{
    Levelup *pDerivedObj = (Levelup *)malloc(sizeof(Levelup));
    Elements *pObj = New_Elements(label);
    // setting derived object member
    pDerivedObj->img = al_load_bitmap("assets/image/levelup.png");
    pDerivedObj->width = al_get_bitmap_width(pDerivedObj->img);
    pDerivedObj->height = al_get_bitmap_height(pDerivedObj->img);
    pDerivedObj->x = 0;
    pDerivedObj->y = 0;
    pDerivedObj->player = player;

    // setting derived object function
    pObj->pDerivedObj = pDerivedObj;
    pObj->Update = Levelup_update;
    pObj->Interact = Levelup_interact;
    pObj->Draw = Levelup_draw;
    pObj->Destroy = Levelup_destory;
    return pObj;
}
void Levelup_update(Elements *self) {
    if(level_up) 
    {
        Levelup *Obj = ((Levelup *)(self->pDerivedObj));

    }
}
void Levelup_interact(Elements *self, Elements *tar) {}
void Levelup_draw(Elements *self, float camera_offset_x, float camera_offset_y)
{
    Levelup *Obj = ((Levelup *)(self->pDerivedObj));
    al_draw_bitmap(Obj->img, Obj->x, Obj->y, 0);
}
void Levelup_destory(Elements *self)
{
    Levelup *Obj = ((Levelup *)(self->pDerivedObj));
    al_destroy_bitmap(Obj->img);
    free(Obj);
    free(self);
}
