#include "projectile.h"
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
    pDerivedObj->img = al_load_bitmap("assets/image/projectile.png");
    pDerivedObj->hit_explosion = algif_new_gif("assets/image/hit_explosion.gif", -1);
    pDerivedObj->explode = false;
    pDerivedObj->explosion_start_time = 0;
    pDerivedObj->width = al_get_bitmap_width(pDerivedObj->img);
    pDerivedObj->height = al_get_bitmap_height(pDerivedObj->img);
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

    // setting derived object function
    pObj->pDerivedObj = pDerivedObj;
    pObj->Update = Projectile_update;
    pObj->Interact = Projectile_interact;
    pObj->Draw = Projectile_draw;
    pObj->Destroy = Projectile_destory;

    return pObj;
}
void _Projectile_explode(Elements *self) {
    Projectile *Obj = ((Projectile *)(self->pDerivedObj));
    Obj->explode = true;
    Obj->explosion_start_time = al_get_time();
}
void _Projectile_draw_explosion(Projectile *Obj) {
    double current_time = al_get_time();
    double elapsed_time = current_time - Obj->explosion_start_time;
    ALLEGRO_BITMAP* frame = algif_get_bitmap(Obj->hit_explosion, elapsed_time);

    if (frame) {
        al_draw_bitmap(frame, Obj->x, Obj->y, 0);
    }
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
            Obj->explosion_start_time = al_get_time();
            _Projectile_explode(self);
            self->dele = true;
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
            self->dele = true;

            printf("boss blood: %f\n", boss->blood);
        }
    }
}
void Projectile_draw(Elements *self, float camera_offset_x, float camera_offset_y)
{
    Projectile *Obj = ((Projectile *)(self->pDerivedObj));
    if (Obj->v > 0)
    {
        // al_draw_bitmap(Obj->img, Obj->x, Obj->y, ALLEGRO_FLIP_HORIZONTAL);
        al_draw_bitmap(Obj->img, Obj->x - camera_offset_x, Obj->y - camera_offset_y, ALLEGRO_FLIP_HORIZONTAL);
    }
    else
    {
        //al_draw_bitmap(Obj->img, Obj->x, Obj->y, 0);
        al_draw_bitmap(Obj->img, Obj->x - camera_offset_x, Obj->y - camera_offset_y, 0);
    }
}
void Projectile_destory(Elements *self)
{
    Projectile *Obj = ((Projectile *)(self->pDerivedObj));
    al_destroy_bitmap(Obj->img);
    algif_destroy_animation(Obj->hit_explosion);
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

    double spacing = 80.0;

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