#include "wall.h"
#include <stdio.h>
#include "../shapes/Rectangle.h"
#include <allegro5/allegro_image.h>


/*
   [wall function]
*/
Elements *New_Wall(int label, int x, int y)
{
    Wall *pDerivedObj = (Wall *)malloc(sizeof(Wall));
    Elements *pObj = New_Elements(label);
    // setting derived object member
    pDerivedObj->img = al_load_bitmap("assets/image/wall.png");
    pDerivedObj->width = al_get_bitmap_width(pDerivedObj->img);
    pDerivedObj->height = al_get_bitmap_height(pDerivedObj->img);
    pDerivedObj->x = x;
    pDerivedObj->y = y;
    // setting the interact object
    pObj->inter_obj[pObj->inter_len++] = Character_L;
    pObj->inter_obj[pObj->inter_len++] = Enemy_L;
    pObj->inter_obj[pObj->inter_len++] = Boss_L;
    pObj->inter_obj[pObj->inter_len++] = Projectile_L;    
    pDerivedObj->hitbox = New_Rectangle(pDerivedObj->x,
                                        pDerivedObj->y,
                                        pDerivedObj->x + pDerivedObj->width,
                                        pDerivedObj->y + pDerivedObj->height);

    // setting derived object function
    pObj->pDerivedObj = pDerivedObj;
    pObj->Draw = Wall_draw;
    pObj->Update = Wall_update;
    pObj->Interact = Wall_interact;
    pObj->Destroy = Wall_destroy;
    return pObj;
}

void Wall_update(Elements *ele) {}

void _handle_collision(Elements *self, Elements *tar, void (*update_position)(Elements*, int, int))
{
    Wall *wall = (Wall *)(self->pDerivedObj);

    // 取得角色或敵人的位置和尺寸
    int chara_x, chara_y, chara_width, chara_height;

    if (tar->label == Character_L)
    {
        Character *chara = (Character *)(tar->pDerivedObj);
        chara_x = chara->x;
        chara_y = chara->y;
        chara_width = chara->width;
        chara_height = chara->height;
    }
    else if (tar->label == Enemy_L)
    {
        Enemy *enemy = (Enemy *)(tar->pDerivedObj);
        chara_x = enemy->x;
        chara_y = enemy->y;
        chara_width = enemy->width;
        chara_height = enemy->height;
    }
    else if (tar->label == Boss_L)
    {
        Boss *boss = (Boss *)(tar->pDerivedObj);
        chara_x = boss->x;
        chara_y = boss->y;
        chara_width = boss->width;
        chara_height = boss->height;
    }
    else
    {
        return; // 如果不是預期的對象則返回
    }

    // 計算角色和牆體的位置和尺寸
    int chara_left = chara_x + (chara_width >> 2);
    int chara_right = chara_x + chara_width - (chara_width >> 2);
    int chara_top = chara_y + (chara_height >> 2);
    int chara_bottom = chara_y + chara_height - (chara_height >> 2);

    int wall_left = wall->x;
    int wall_right = wall->x + wall->width;
    int wall_top = wall->y;
    int wall_bottom = wall->y + wall->height;

    // 檢查角色是否碰到牆體並調整位置
    if (chara_right > wall_left && chara_left < wall_right && chara_bottom > wall_top && chara_top < wall_bottom)
    {
        // 計算重疊部分
        int overlap_left = chara_right - wall_left;
        int overlap_right = wall_right - chara_left;
        int overlap_top = chara_bottom - wall_top;
        int overlap_bottom = wall_bottom - chara_top;

        // 找出最小重疊部分並調整位置
        if (overlap_left < overlap_right && overlap_left < overlap_top && overlap_left < overlap_bottom)
        {
            update_position(tar, -overlap_left, 0);
        }
        else if (overlap_right < overlap_left && overlap_right < overlap_top && overlap_right < overlap_bottom)
        {
            update_position(tar, overlap_right, 0);
        }
        else if (overlap_top < overlap_left && overlap_top < overlap_right && overlap_top < overlap_bottom)
        {
            update_position(tar, 0, -overlap_top);
        }
        else if (overlap_bottom < overlap_left && overlap_bottom < overlap_right && overlap_bottom < overlap_top)
        {
            update_position(tar, 0, overlap_bottom);
        }
    }
}


void Wall_interact(Elements *self, Elements *tar)
{
    if (tar->label == Character_L)
    {
        _handle_collision(self, tar, _Character_update_position);
    }
    else if (tar->label == Enemy_L)
    {
        _handle_collision(self, tar, _Enemy_update_position);
    }
    else if (tar->label == Boss_L)
    {
        _handle_collision(self, tar, _Boss_update_position);
    }
    else if (tar->label == Projectile_L)
    {
        Projectile *proj = (Projectile *)(tar->pDerivedObj);
        Wall *wall = (Wall *)(self->pDerivedObj);

        // 計算敵人和牆體的位置和尺寸
        int proj_left = proj->x + (proj->width >> 2);
        int proj_right = proj->x + proj->width - (proj->width >> 2);
        int proj_top = proj->y + (proj->height >> 2);
        int proj_bottom = proj->y + proj->height - (proj->height >> 2);

        int wall_left = wall->x;
        int wall_right = wall->x + wall->width;
        int wall_top = wall->y;
        int wall_bottom = wall->y + wall->height;

        // 檢查投射物是否碰到牆體，如果是的話刪除投射物
        if (proj_right > wall_left && proj_left < wall_right && proj_bottom > wall_top && proj_top < wall_bottom)
        {
            tar->dele = true;
        }
    }
    else
    {
        return;
    }

}

void Wall_draw(Elements *self)
{
    Wall *Obj = ((Wall *)(self->pDerivedObj));
    al_draw_bitmap(Obj->img, Obj->x, Obj->y, 0);
}

void Wall_destroy(Elements *self)
{
    Wall *Obj = ((Wall *)(self->pDerivedObj));
    al_destroy_bitmap(Obj->img);
    free(Obj);
    free(self);
}
