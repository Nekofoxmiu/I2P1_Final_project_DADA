#include <allegro5/allegro_primitives.h>
#include "gamescene.h"
#include "../global.h"

#define FONT_SIZE 48
/*
   [GameScene function]
*/
Scene *New_GameScene(int label)
{
    GameScene *pDerivedObj = (GameScene *)malloc(sizeof(GameScene));
    Scene *pObj = New_Scene(label);
    // setting derived object member
    pDerivedObj->font = al_load_ttf_font("assets/font/pixel_font.ttf", FONT_SIZE, 0);
    pDerivedObj->background = al_load_bitmap("assets/image/background.png");
    pObj->pDerivedObj = pDerivedObj;
    // enemy status enhance (initial = 1)
    pDerivedObj->ene_hp_enhance = 1;
    pDerivedObj->ene_def_enhance = 1;
    pDerivedObj->ene_atk_enhance = 1;
    pDerivedObj->ene_chasedis_enhance = 1;
    pDerivedObj->ene_atkdis_enhance = 1;
    pDerivedObj->ene_spd_enhance = 1;

    // set timer
    pDerivedObj->start_time = al_get_time();
    pDerivedObj->start_time_spawn = al_get_time();
    pDerivedObj->start_time_boss = al_get_time();
    pDerivedObj->elapsed_time = 0;
    pDerivedObj->elapsed_time_spawn = 0;
    pDerivedObj->elapsed_time_boss = 0;
    // spawn rate
    pDerivedObj->ene_spawn_rate = 1;
    pDerivedObj->ene_spawn_acc = 0;
    pDerivedObj->boss_spawn_rate = 0.05;
    pDerivedObj->boss_spawn_acc = 0;
    // spawn type proportion
    pDerivedObj->slime_proportion = 1;
    pDerivedObj->dog_proportion = 0;
    // character status
    pDerivedObj->chara_blood_x = 20;
    pDerivedObj->chara_blood_y = 20;
    pDerivedObj->chara_mp_x = 20;
    pDerivedObj->chara_mp_y = 35;
    pDerivedObj->chara_exp_x = 20;
    pDerivedObj->chara_exp_y = 50;
    pDerivedObj->chara_cool_x = 20;
    pDerivedObj->chara_cool_y = 65;

    // sound
    pDerivedObj->song = al_load_sample("assets/sound/game.wav");
    if (pDerivedObj->song == NULL) printf("Error");
    al_reserve_samples(20);
    pDerivedObj->sample_instance = al_create_sample_instance(pDerivedObj->song);
    // Loop the song until the display closes
    al_set_sample_instance_playmode(pDerivedObj->sample_instance, ALLEGRO_PLAYMODE_LOOP);
    al_restore_default_mixer();
    al_attach_sample_instance_to_mixer(pDerivedObj->sample_instance, al_get_default_mixer());
    // set the volume of instance
    al_set_sample_instance_gain(pDerivedObj->sample_instance, 0.7);
    
    // register element
    _Register_elements(pObj, New_Floor(Floor_L));
    // _Register_elements(pObj, New_Teleport(Teleport_L));
    _Register_elements(pObj, New_Tree(Tree_L));
    Elements *character = New_Character(Character_L, default_chara_L);
    _Register_elements(pObj, character);
    _Register_elements(pObj, New_Enemy(Enemy_L, slime_L, (Character *)(character->pDerivedObj), 
        pDerivedObj->ene_atk_enhance, pDerivedObj->ene_def_enhance, pDerivedObj->ene_hp_enhance, pDerivedObj->ene_chasedis_enhance, pDerivedObj->ene_atkdis_enhance, pDerivedObj->ene_spd_enhance));

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
    // update timer
    GameScene *gs = ((GameScene *)(self->pDerivedObj));
    Character* chara = (Character *)(_Get_all_elements(self).arr[Character_L]->pDerivedObj);
    if (!everything_stop)
    {
        double current_time = al_get_time();
        gs->elapsed_time = current_time - gs->start_time;
        gs->elapsed_time_spawn = current_time - gs->start_time_spawn;
        gs->elapsed_time_boss = current_time - gs->start_time_boss;

    // enhance every half minute
    if(gs->elapsed_time > 30){
        gs->ene_atk_enhance *= 1.1;
        gs->ene_def_enhance *= 1.1;
        gs->ene_hp_enhance *= 1.1;
        gs->ene_chasedis_enhance *= 1.1;
        gs->ene_atkdis_enhance *= 1;
        gs->ene_spd_enhance *= 1.1;

            // enhance spawn rate
            gs->ene_spawn_rate *= 1.1;
            gs->boss_spawn_rate *= 1.01;

        // redistribute spawn type proportion
        if(gs->slime_proportion > 0.6){
            gs->slime_proportion *= 0.95;
            gs->dog_proportion = 1 - gs->slime_proportion;
        }
        else if(gs->dog_proportion > 0.3){
            gs->slime_proportion *= 0.98;
            gs->dog_proportion *= 0.95;
        }
        else{
            gs->slime_proportion *= 0.98;
            gs->dog_proportion *= 0.98;
        }

            // reset the start time
            gs->start_time = current_time;
        }

        // enemy spawn
        if(gs->elapsed_time_spawn > 1){
            gs->ene_spawn_acc += gs->ene_spawn_rate;
            gs->start_time_spawn = current_time;
        }
        if(gs->ene_spawn_acc > 1){
            gs->ene_spawn_acc--;
            spawn_enemy = true;
        }

        // boss spawn
        if(gs->elapsed_time_boss > 1){
            gs->boss_spawn_acc += gs->boss_spawn_rate;
            gs->start_time_boss = current_time;
        }
        if(gs->boss_spawn_acc > 1){
            gs->boss_spawn_acc--;
            spawn_boss = true;
        }
    }

    // update every element
    ElementVec allEle = _Get_all_elements(self);

    if (spawn_enemy)
    {
        spawn_enemy = false; // set the flag indicating the key is pressed
        double x = (double) rand() / (RAND_MAX + 1.0);
        if(x <= gs->slime_proportion){
            Elements *enemy = New_Enemy(Enemy_L, slime_L, chara, 
                gs->ene_atk_enhance, gs->ene_def_enhance, gs->ene_hp_enhance, gs->ene_chasedis_enhance, gs->ene_atkdis_enhance, gs->ene_spd_enhance);
            _Register_elements(self, enemy);
        }
        else if(x <= gs->slime_proportion + gs->dog_proportion){
            Elements *enemy = New_Enemy(Enemy_L, dog_L, chara, 
                gs->ene_atk_enhance, gs->ene_def_enhance, gs->ene_hp_enhance, gs->ene_chasedis_enhance, gs->ene_atkdis_enhance, gs->ene_spd_enhance);
            _Register_elements(self, enemy);
        }
        else{
            Elements *enemy = New_Enemy(Enemy_L, dragon_L, chara, 
                gs->ene_atk_enhance, gs->ene_def_enhance, gs->ene_hp_enhance, gs->ene_chasedis_enhance, gs->ene_atkdis_enhance, gs->ene_spd_enhance);
            _Register_elements(self, enemy);
        }
    }

    if (spawn_boss)
    {
        spawn_boss = false; // set the flag indicating the key is pressed
        Elements *boss = New_Boss(Boss_L, chara);
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
    update_camera(chara);

    // remove element
    for(int i = 0; i < allEle.len; i++)
    {
        Elements *ele = allEle.arr[i];
        if (ele->dele)
            _Remove_elements(self, ele);
    }

    // character death
    if(chara -> blood <= 0){
        score = chara->xp;
        if(score > highest_record){
            highest_record = score;
        }
        self->scene_end = true;
        window = 2;
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
    char cool[20];
    sprintf(blood, "Blood: %d / %d", (int)chara->blood, (int)chara->max_blood);
    sprintf(mp, "MP: %d / %d", (int)chara->mp, (int)chara->max_mp);
    sprintf(exp, "EXP: %d / %d", (int)chara->xp, (int)chara->levelExpNeed);
    sprintf(cool, "SKILL: %ds / %ds", (int)chara->aura_cool - (int)chara->aura_elapsed_time, (int)chara->aura_cool);
    al_draw_text(gs->font, al_map_rgb(255, 255, 255), gs->chara_blood_x, gs->chara_blood_y, ALLEGRO_ALIGN_LEFT, blood);
    al_draw_text(gs->font, al_map_rgb(255, 255, 255), gs->chara_mp_x, gs->chara_mp_y + FONT_SIZE, ALLEGRO_ALIGN_LEFT, mp);
    al_draw_text(gs->font, al_map_rgb(255, 255, 255), gs->chara_exp_x, gs->chara_exp_y + FONT_SIZE * 2, ALLEGRO_ALIGN_LEFT, exp);
    al_draw_text(gs->font, al_map_rgb(255, 255, 255), gs->chara_cool_x, gs->chara_cool_y + FONT_SIZE * 3, ALLEGRO_ALIGN_LEFT, cool);
    al_play_sample_instance(gs->sample_instance);
}
void game_scene_destroy(Scene *self)
{
    GameScene *Obj = ((GameScene *)(self->pDerivedObj));
    ALLEGRO_BITMAP *background = Obj->background;
    al_destroy_bitmap(background);
    al_destroy_sample_instance(Obj->sample_instance);
    ElementVec allEle = _Get_all_elements(self);
    for (int i = 0; i < allEle.len; i++)
    {
        Elements *ele = allEle.arr[i];
        ele->Destroy(ele);
    }
    free(Obj);
    free(self);
}
