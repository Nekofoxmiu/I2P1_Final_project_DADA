#include <allegro5/allegro_primitives.h>
#include "gamescene.h"
#include "../global.h"

#define FONT_SIZE 24
/*
   [GameScene function]
*/
Scene *New_GameScene(int label)
{
    GameScene *pDerivedObj = (GameScene *)malloc(sizeof(GameScene));
    Scene *pObj = New_Scene(label);
    // setting derived object member
    pDerivedObj->font = al_load_ttf_font("assets/font/pirulen.ttf", FONT_SIZE, 0);
    pDerivedObj->background = al_load_bitmap("assets/image/stage.jpg");
    pObj->pDerivedObj = pDerivedObj;
    pDerivedObj->chara_blood_x = WIDTH / 9 > 120 ? WIDTH / 9 : 120;
    pDerivedObj->chara_blood_y = 30;
    // register element
    _Register_elements(pObj, New_Floor(Floor_L));
    _Register_elements(pObj, New_Teleport(Teleport_L));
    _Register_elements(pObj, New_Tree(Tree_L));
    Elements *character = New_Character(Character_L, default_chara_L);
    _Register_elements(pObj, character);
    _Register_elements(pObj, New_Enemy(Enemy_L, slime_L, (Character *)(character->pDerivedObj)));

    // setting derived object function
    pObj->Update = game_scene_update;
    pObj->Draw = game_scene_draw;
    pObj->Destroy = game_scene_destroy;
    return pObj;
}

void game_scene_update(Scene *self)
{
    // update every element
    ElementVec allEle = _Get_all_elements(self);

    if (spawn_enemy)
    {
        spawn_enemy = false; // set the flag indicating the key is pressed
        Elements *enemy = New_Enemy(Enemy_L, slime_L, (Character *)(_Get_all_elements(self).arr[Character_L]->pDerivedObj));
        _Register_elements(self, enemy);
    }

    if (spawn_boss)
    {
        spawn_boss = false; // set the flag indicating the key is pressed
        Elements *boss = New_Boss(Boss_L, (Character *)(_Get_all_elements(self).arr[Character_L]->pDerivedObj));
        _Register_elements(self, boss);
    }

    if (spawn_wall)
    {
        spawn_wall = false; // set the flag indicating the key is pressed
        Elements *wall = New_Wall(Wall_L, mouse.x, mouse.y);
        _Register_elements(self, wall);
    }

    for (int i = 0; i < allEle.len; i++)
    {
        allEle.arr[i]->Update(allEle.arr[i]);
    }

    // run interact for every element
    for (int i = 0; i < allEle.len; i++)
    {
        Elements *ele = allEle.arr[i];
        // run every interact object
        for (int j = 0; j < ele->inter_len; j++)
        {
            int inter_label = ele->inter_obj[j];
            ElementVec labelEle = _Get_label_elements(self, inter_label);
            for (int i = 0; i < labelEle.len; i++)
            {
                ele->Interact(ele, labelEle.arr[i]);
            }
        }
    }
    // remove element
    for (int i = 0; i < allEle.len; i++)
    {
        Elements *ele = allEle.arr[i];
        if (ele->dele)
            _Remove_elements(self, ele);
    }
}
void game_scene_draw(Scene *self)
{
    al_clear_to_color(al_map_rgb(0, 0, 0));
    GameScene *gs = ((GameScene *)(self->pDerivedObj));
    al_draw_bitmap(gs->background, 0, 0, 0);
    ElementVec allEle = _Get_all_elements(self);
    for (int i = 0; i < allEle.len; i++)
    {
        Elements *ele = allEle.arr[i];
        ele->Draw(ele);
    }
    Character *chara = allEle.arr[Character_L]->pDerivedObj;
    char blood[20];
    sprintf(blood, "Blood: %d", (int)chara->blood);
    al_draw_text(gs->font, al_map_rgb(255, 255, 255), gs->chara_blood_x, gs->chara_blood_y, ALLEGRO_ALIGN_CENTRE, blood);
}
void game_scene_destroy(Scene *self)
{
    GameScene *Obj = ((GameScene *)(self->pDerivedObj));
    ALLEGRO_BITMAP *background = Obj->background;
    al_destroy_bitmap(background);
    ElementVec allEle = _Get_all_elements(self);
    for (int i = 0; i < allEle.len; i++)
    {
        Elements *ele = allEle.arr[i];
        ele->Destroy(ele);
    }
    free(Obj);
    free(self);
}
