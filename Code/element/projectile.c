#include "projectile.h"
#include "../shapes/Circle.h"
/*
   [Projectile function]
*/
Elements *New_Projectile(Elements *creator, int label, double damage, double x, double y, int v, double vdx, double vdy)
{
    Projectile *pDerivedObj = (Projectile *)malloc(sizeof(Projectile));
    Elements *pObj = New_Elements(label);
    // setting derived object member
    pDerivedObj->img = al_load_bitmap("assets/image/projectile.png");
    pDerivedObj->width = al_get_bitmap_width(pDerivedObj->img);
    pDerivedObj->height = al_get_bitmap_height(pDerivedObj->img);
    pDerivedObj->damage = damage;
    pDerivedObj->x = x;
    pDerivedObj->y = y;
    pDerivedObj->v = v;

    pDerivedObj->vdx = vdx;
    pDerivedObj->vdy = vdy;
    // make sure it's a unit vector
    // if (vdx * vdx + vdy * vdy > 1.0) {
    //     fprintf(stderr, "Error: The vector is not a unit vector.");
    //     exit(EXIT_FAILURE);
    // }

    pDerivedObj->hitbox = New_Circle(pDerivedObj->x + pDerivedObj->width / 2,
                                     pDerivedObj->y + pDerivedObj->height / 2,
                                     min(pDerivedObj->width, pDerivedObj->height) / 2);
    
    // setting the interact object
    if (creator->label == Character_L) {
        pObj->inter_obj[pObj->inter_len++] = Enemy_L;
        pObj->inter_obj[pObj->inter_len++] = Boss_L;
    }
    else if (creator->label == Enemy_L || creator->label == Boss_L) {
        pObj->inter_obj[pObj->inter_len++] = Character_L;
    }
    pObj->inter_obj[pObj->inter_len++] = Tree_L;
    pObj->inter_obj[pObj->inter_len++] = Floor_L;

    // setting derived object function
    pObj->pDerivedObj = pDerivedObj;
    pObj->Update = Projectile_update;
    pObj->Interact = Projectile_interact;
    pObj->Draw = Projectile_draw;
    pObj->Destroy = Projectile_destory;

    return pObj;
}
void Projectile_update(Elements *self)
{
    Projectile *Obj = ((Projectile *)(self->pDerivedObj));
    _Projectile_update_position(self, Obj->v * Obj->vdx, Obj->v * Obj->vdy);
}
void _Projectile_update_position(Elements *self, double dx, double dy)
{
    Projectile *Obj = ((Projectile *)(self->pDerivedObj));
    Obj->x += dx;
    Obj->y += dy;
    Shape *hitbox = Obj->hitbox;
    hitbox->update_center_x(hitbox, dx);
    hitbox->update_center_y(hitbox, dy);
}
void Projectile_interact(Elements *self, Elements *tar)
{
    Projectile *Obj = ((Projectile *)(self->pDerivedObj));
    if (tar->label == Floor_L)
    {
        if (Obj->x < 0 - Obj->width)
            self->dele = true;
        else if (Obj->x > WIDTH + Obj->width)
            self->dele = true;
    }
    else if (tar->label == Tree_L)
    {
        Tree *tree = ((Tree *)(tar->pDerivedObj));
        if (tree->hitbox->overlap(tree->hitbox, Obj->hitbox))
        {
            self->dele = true;
        }
    }
    else if (tar->label == Character_L)
    {
        Character *character = ((Character *)(tar->pDerivedObj));
        if (character->hitbox->overlap(character->hitbox, Obj->hitbox))
        {
            if (character->armor > Obj->damage) {
                character->blood -= 1;
            }
            else {
                character->blood -= Obj->damage - character->armor;
            }
            self->dele = true;
        }
    }
    else if (tar->label == Enemy_L)
    {
        Enemy *enemy = ((Enemy *)(tar->pDerivedObj));
        if (enemy->hitbox->overlap(enemy->hitbox, Obj->hitbox))
        {
            if(enemy->armor > Obj->damage) {
                enemy->blood -= 1;
            }
            else {
                enemy->blood -= Obj->damage - enemy->armor;
            }
            self->dele = true;

            printf("enemy blood: %f\n", enemy->blood);
        }
    }
    else if (tar->label == Boss_L)
    {
        Boss *boss = ((Boss *)(tar->pDerivedObj));
        if (boss->hitbox->overlap(boss->hitbox, Obj->hitbox))
        {
            boss->blood -= 1;
            self->dele = true;
        }
    }
}
void Projectile_draw(Elements *self)
{
    Projectile *Obj = ((Projectile *)(self->pDerivedObj));
    if (Obj->v > 0)
        al_draw_bitmap(Obj->img, Obj->x, Obj->y, ALLEGRO_FLIP_HORIZONTAL);
    else
        al_draw_bitmap(Obj->img, Obj->x, Obj->y, 0);
}
void Projectile_destory(Elements *self)
{
    Projectile *Obj = ((Projectile *)(self->pDerivedObj));
    al_destroy_bitmap(Obj->img);
    free(Obj->hitbox);
    free(Obj);
    free(self);
}
