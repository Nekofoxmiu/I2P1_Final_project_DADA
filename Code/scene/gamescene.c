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
    pDerivedObj->background = al_load_bitmap("assets/image/map.png");
    pObj->pDerivedObj = pDerivedObj;
    pDerivedObj->chara_blood_x = 20;
    pDerivedObj->chara_blood_y = 20;
    pDerivedObj->chara_mp_x = 20;
    pDerivedObj->chara_mp_y = 35;
    pDerivedObj->chara_exp_x = 20;
    pDerivedObj->chara_exp_y = 50;
    // register element
    _Register_elements(pObj, New_Floor(Floor_L));
    //_Register_elements(pObj, New_Teleport(Teleport_L));
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

void update_camera(Character *chara){
    camera_x = chara->x - WIDTH / 2;
    camera_y = chara->y - HEIGHT / 2;

    // prevent camera from exceeding world width/height
    if (camera_x < 0) camera_x = 0;
    if (camera_y < 0) camera_y = 0;
    if (camera_x >= WORLD_WIDTH - WIDTH) camera_x = WORLD_WIDTH - WIDTH;
    if (camera_y >= WORLD_HEIGHT - HEIGHT) camera_y = WORLD_HEIGHT - HEIGHT;
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
        Elements *wall = New_Wall(Wall_L, mouse.x + camera_x, mouse.y + camera_y);
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

    // update camera
    update_camera((Character *)(_Get_all_elements(self).arr[Character_L]->pDerivedObj));

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
    /*
    al_draw_scaled_bitmap(gs->background,
                          0, 0, WIDTH, HEIGHT,
                          0, 0, WORLD_WIDTH, WORLD_HEIGHT,
                          0);
    */
    al_draw_bitmap(gs->background, 0 - camera_x, 0 - camera_y, 0);
    
    ElementVec allEle = _Get_all_elements(self);
    Character *chara = allEle.arr[Character_L]->pDerivedObj;
    for (int i = 0; i < allEle.len; i++)
    {
        Elements *ele = allEle.arr[i];
        ele->Draw(ele, camera_x, camera_y);
    }
    
    char blood[20];
    char mp[20];
    char exp[20];
    sprintf(blood, "Blood: %d", (int)chara->blood);
    sprintf(mp, "MP: %d", (int)chara->mp);
    sprintf(exp, "EXP: %d", (int)chara->xp);
    al_draw_text(gs->font, al_map_rgb(255, 255, 255), gs->chara_blood_x, gs->chara_blood_y, ALLEGRO_ALIGN_LEFT, blood);
    al_draw_text(gs->font, al_map_rgb(255, 255, 255), gs->chara_mp_x, gs->chara_mp_y + FONT_SIZE, ALLEGRO_ALIGN_LEFT, mp);
    al_draw_text(gs->font, al_map_rgb(255, 255, 255), gs->chara_exp_x, gs->chara_exp_y + FONT_SIZE * 2, ALLEGRO_ALIGN_LEFT, exp);


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
