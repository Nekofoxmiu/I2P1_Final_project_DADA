#include "floor.h"
#include <stdio.h>
/*
   [floor function]
*/
Elements *New_Floor(int label)
{
    Floor *pDerivedObj = (Floor *)malloc(sizeof(Floor));
    Elements *pObj = New_Elements(label);
    // setting derived object member
    pDerivedObj->img = al_load_bitmap("assets/image/floor.png");
    pDerivedObj->width = al_get_bitmap_width(pDerivedObj->img);
    pDerivedObj->height = al_get_bitmap_height(pDerivedObj->img);
    _Floor_load_map(pDerivedObj);
    _Floor_build_map(pDerivedObj, 0, 0);
    pDerivedObj->x = 0;
    pDerivedObj->y = 0;
    // setting the interact object
    pObj->inter_obj[pObj->inter_len++] = Character_L;
    // setting derived object function
    pObj->pDerivedObj = pDerivedObj;
    pObj->Draw = Floor_draw;
    pObj->Update = Floor_update;
    pObj->Interact = Floor_interact;
    pObj->Destroy = Floor_destory;
    return pObj;
}
void _Floor_load_map(Floor *floor)
{
    FILE *data;
    data = fopen("assets/map/gamescene_map.txt", "r");
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            fscanf(data, "%d", &floor->map_data[i][j]);
        }
    }
    fclose(data);
}
void Floor_update(Elements *ele) {}
void Floor_interact(Elements *self, Elements *tar)
{
    if (tar->label == Character_L)
    {
        Character *chara = (Character *)(tar->pDerivedObj);
        int right_limit = WORLD_WIDTH - chara->width / 2;
        int left_limit = 0 - chara->width / 2;
        int up_limit = 0 - chara->height / 2;
        int down_limit = WORLD_HEIGHT - chara->height / 2;

        if (chara->x < left_limit)
        {
            _Character_update_position(tar, (left_limit - chara->x), 0);
        }
        if (chara->x > right_limit)
        {
            _Character_update_position(tar, (right_limit - chara->x), 0);
        }
        if (chara->y < up_limit) {
            _Character_update_position(tar, 0, (up_limit - chara->y));
        }
        if (chara->y > down_limit) {
            _Character_update_position(tar, 0, (down_limit - chara->y));
        }
    }
}

//這個是給不用建立實體只需要畫上去的物件
void Floor_draw(Elements *self, float camera_offset_x, float camera_offset_y)
{
    Floor *Obj = ((Floor *)(self->pDerivedObj));
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            if (Obj->map_data[i][j] == 1)
            {
                //al_draw_bitmap(Obj->img, Obj->x + j * Obj->width, Obj->y + i * Obj->height, 0);
                al_draw_bitmap(Obj->img, j * Obj->width - camera_offset_x, i * Obj->height - camera_offset_y, 0);          
            }
        }
    }
}

void _Floor_build_map(Floor *Obj, float camera_offset_x, float camera_offset_y)
{
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            if (Obj->map_data[i][j] == 1)
            {
                Elements *wall = New_Wall(Wall_L, j * Obj->width, i * Obj->height);
                _Register_elements(scene, wall);  
            }
        }
    }
}

void Floor_destory(Elements *self)
{
    Floor *Obj = ((Floor *)(self->pDerivedObj));
    al_destroy_bitmap(Obj->img);
    free(Obj);
    free(self);
}
