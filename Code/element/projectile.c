#include "projectile.h"
#include "explosion.h"
#include "../shapes/Circle.h"

#define M_PI 3.14159265358979323846
/*
   [Projectile function]
*/
Elements *New_Projectile(Elements *creator, int label, double damage, double x, double y, int v, double vdx, double vdy, bool chase)
{
    Projectile *pDerivedObj = (Projectile *)malloc(sizeof(Projectile));
    Elements *pObj = New_Elements(label);
    // setting derived object member
    pDerivedObj->creator = creator;
    if (creator->label == Character_L) {
        pDerivedObj->animation = algif_new_gif("assets/image/projectile.gif", -1);
    }
    else {
        pDerivedObj->animation = algif_new_gif("assets/image/projectile_enemy.gif", -1);
    }
    pDerivedObj->width = pDerivedObj->animation->width;
    pDerivedObj->height = pDerivedObj->animation->height;
    pDerivedObj->damage = damage;
    pDerivedObj->x = x;
    pDerivedObj->y = y;
    pDerivedObj->v = v;

    pDerivedObj->vdx = vdx;
    pDerivedObj->vdy = vdy;
    pDerivedObj->chase = chase;
    pDerivedObj->hitbox = New_Circle(pDerivedObj->x + pDerivedObj->width / 2,
                                     pDerivedObj->y + pDerivedObj->height / 2,
                                     min(pDerivedObj->width, pDerivedObj->height) / 2);

    // setting the interact object
    if (creator->label == Character_L)
    {
        pObj->inter_obj[pObj->inter_len++] = Enemy_L;
        pObj->inter_obj[pObj->inter_len++] = Boss_L;
    }
    else if (creator->label == Enemy_L || creator->label == Boss_L)
    {
        pObj->inter_obj[pObj->inter_len++] = Character_L;
    }
    pObj->inter_obj[pObj->inter_len++] = Tree_L;
    pObj->inter_obj[pObj->inter_len++] = Floor_L;
    pObj->inter_obj[pObj->inter_len++] = Projectile_L;

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
    if(everything_stop) return;
    Projectile *Obj = ((Projectile *)(self->pDerivedObj));
    _Projectile_update_position(self, Obj->v * Obj->vdx, Obj->v * Obj->vdy);

    if (Obj->chase)
    {
        if (Obj->creator->label == Boss_L || Obj->creator->label == Enemy_L)
        {
            Boss *boss = (Boss *)Obj->creator->pDerivedObj;
            Character *target = boss->target;

            double dx = target->x - Obj->x;
            double dy = target->y - Obj->y;

            if (dx <= 20 || dy <= 20)
                Obj->chase = false;

            NormalizeV(&dx, &dy);
            Obj->vdx = dx;
            Obj->vdy = dy;
        }
    }
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
        if (Obj->x > WORLD_WIDTH + Obj->width)
            self->dele = true;
        if (Obj->y < 0 - Obj->height)
            self->dele = true;
        if (Obj->y > WORLD_HEIGHT + Obj->height)
            self->dele = true;
    }
    else if (tar->label == Tree_L)
    {
        Tree *tree = ((Tree *)(tar->pDerivedObj));
        if (tree->hitbox->overlap(tree->hitbox, Obj->hitbox))
        {
            self->dele = true;
            Elements *explosion = New_Explosion(Explosion_L, Obj->x, Obj->y);
            _Register_elements(scene, explosion);
        }
    }
    else if (tar->label == Character_L)
    {
        Character *character = ((Character *)(tar->pDerivedObj));
        if (character->hitbox->overlap(character->hitbox, Obj->hitbox))
        {
            if (character->armor > Obj->damage)
            {
                character->blood -= 1;
            }
            else
            {
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
            if (enemy->armor > Obj->damage)
            {
                enemy->blood -= 1;
            }
            else
            {
                enemy->blood -= Obj->damage - enemy->armor;
            }
            Elements *explosion = New_Explosion(Explosion_L, Obj->x, Obj->y);
            _Register_elements(scene, explosion);
            self->dele = true;

            printf("enemy blood: %f\n", enemy->blood);
        }
    }
    else if (tar->label == Boss_L)
    {
        Boss *boss = ((Boss *)(tar->pDerivedObj));
        if (boss->hitbox->overlap(boss->hitbox, Obj->hitbox))
        {
            if (boss->armor > Obj->damage)
            {
                boss->blood -= 1;
            }
            else
            {
                boss->blood -= Obj->damage - boss->armor;
            }
            Elements *explosion = New_Explosion(Explosion_L, Obj->x, Obj->y);
            _Register_elements(scene, explosion);
            self->dele = true;

            printf("boss blood: %f\n", boss->blood);
        }
    }
    else if (tar->label == Projectile_L)
    {
        Projectile *pro = (Projectile *)(tar->pDerivedObj);
        if (Obj->creator->label == pro->creator->label) return;
        if (pro->hitbox->overlap(Obj->hitbox, pro->hitbox)) {
            tar->dele = true;
        }
    }
}
void Projectile_draw(Elements *self, float camera_offset_x, float camera_offset_y)
{
    Projectile *Obj = ((Projectile *)(self->pDerivedObj));
    ALLEGRO_BITMAP *frame = algif_get_bitmap(Obj->animation, al_get_time());
    if (frame) {
        if (Obj->v > 0)
        {
            // al_draw_bitmap(Obj->img, Obj->x, Obj->y, ALLEGRO_FLIP_HORIZONTAL);
            al_draw_bitmap(frame, Obj->x - camera_offset_x, Obj->y - camera_offset_y, ALLEGRO_FLIP_HORIZONTAL);
        }
        else
        {
            //al_draw_bitmap(Obj->img, Obj->x, Obj->y, 0);
            al_draw_bitmap(frame, Obj->x - camera_offset_x, Obj->y - camera_offset_y, 0);
        }
    }
}
void Projectile_destory(Elements *self)
{
    Projectile *Obj = ((Projectile *)(self->pDerivedObj));
    algif_destroy_animation(Obj->animation);
    free(Obj->hitbox);
    free(Obj);
    free(self);
}

void Attack_Normal(Elements *creator, int num_bullets, int speed, bool chase)
{

    double dx = 0;
    double dy = 0;

    double startx = 0;
    double starty = 0;

    double damage = 0;

    if (creator->label == Boss_L)
    {
        Boss *boss = (Boss *)(creator->pDerivedObj);
        Character *target = boss->target;
        startx = boss->x;
        starty = boss->y;
        dx = target->x - startx;
        dy = target->y - starty;
        damage = boss->damage;
    }
    else if (creator->label == Enemy_L)
    {
        Enemy *enemy = (Enemy *)(creator->pDerivedObj);
        Character *target = enemy->target;
        startx = enemy->x;
        starty = enemy->y;
        dx = target->x - startx;
        dy = target->y - starty;
        damage = enemy->damage;
    }
    else if (creator->label == Character_L)
    {
        Character *chara = creator->pDerivedObj;
        startx = chara->weapon_x;
        starty = chara->weapon_y;
        dx = mouse.x + camera_x - startx;
        dy = mouse.y + camera_y - starty;
        damage = chara->damage;
    }

    NormalizeV(&dx, &dy);

    double spacing = 100.0;

    for (int i = 0; i < num_bullets; i++)
    {
        double offset = i * spacing;
        double bullet_x = startx + dx * offset;
        double bullet_y = starty + dy * offset;

        Elements *pro = New_Projectile(creator, Projectile_L, damage, bullet_x, bullet_y, speed, dx, dy, chase);
        _Register_elements(scene, pro);
    }
}

void Attack_Radial(Elements *creator, int num_bullets, int speed, int rounds)
{

    double center_x = 0;
    double center_y = 0;
    double radius = 50.0;
    double damage = 0;

    if (creator->label == Boss_L)
    {
        Boss *boss = (Boss *)(creator->pDerivedObj);
        center_x = boss->x;
        center_y = boss->y;
        damage = boss->damage;
    }
    else if (creator->label == Enemy_L)
    {
        Enemy *enemy = (Enemy *)(creator->pDerivedObj);
        center_x = enemy->x;
        center_y = enemy->y;
        damage = enemy->damage;
    }
    else if (creator->label == Character_L)
    {
        Character *chara = creator->pDerivedObj;
        center_x = chara->x;
        center_y = chara->y;
        damage = chara->damage;
    }

    for (int r = 0; r < rounds; r++)
    {
        double offset = r * radius;
        for (int i = 0; i < num_bullets; i++)
        {
            double angle = (2 * M_PI / num_bullets) * i;
            double dx = cos(angle);
            double dy = sin(angle);
            double bullet_x = center_x + radius * cos(angle) + dx * offset;
            double bullet_y = center_y + radius * sin(angle) + dy * offset;

            Elements *pro = New_Projectile(creator, Projectile_L, damage, bullet_x, bullet_y, speed, dx, dy, false);
            _Register_elements(scene, pro);
        }
    }
}