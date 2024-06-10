#include <allegro5/allegro_primitives.h>
#include "menu.h"
#include <stdbool.h>
/*
   [Menu function]
*/
Scene *New_Menu(int label)
{
    Menu *pDerivedObj = (Menu *)malloc(sizeof(Menu));
    Scene *pObj = New_Scene(label);
    // setting derived object member
    pDerivedObj->font_large = al_load_ttf_font("assets/font/pixel_font.ttf", 110, 0);
    pDerivedObj->font_small = al_load_ttf_font("assets/font/pixel_font.ttf", 40, 0);
    pDerivedObj->background = al_load_bitmap("assets/image/menu/background.jpg");
    // Load sound
    pDerivedObj->song = al_load_sample("assets/sound/menu.wav");
    al_reserve_samples(20);
    pDerivedObj->sample_instance = al_create_sample_instance(pDerivedObj->song);
    // Loop the song until the display closes
    al_set_sample_instance_playmode(pDerivedObj->sample_instance, ALLEGRO_PLAYMODE_LOOP);
    al_restore_default_mixer();
    al_attach_sample_instance_to_mixer(pDerivedObj->sample_instance, al_get_default_mixer());
    // set the volume of instance
    al_set_sample_instance_gain(pDerivedObj->sample_instance, 0.5);
    // title
    pDerivedObj->title_x = WIDTH / 2;
    pDerivedObj->title_y = HEIGHT / 6;
    // torch
    pDerivedObj->torch = algif_new_gif("assets/image/menu/torch.gif", -1);
    // charactor
    pDerivedObj->charactor = algif_new_gif("assets/image/menu/knight.gif", -1);
    pObj->pDerivedObj = pDerivedObj;
    // setting derived object function
    pObj->Update = menu_update;
    pObj->Draw = menu_draw;
    pObj->Destroy = menu_destroy;
    return pObj;
}
void menu_update(Scene *self)
{
    if (key_state[ALLEGRO_KEY_ENTER])
    {
        self->scene_end = true;
        window = 1;
    }
    return;
}
void menu_draw(Scene *self)
{
    Menu *Obj = ((Menu *)(self->pDerivedObj));
    // frame width 4 * 32 pixel
    ALLEGRO_BITMAP* frame_torch = algif_get_bitmap(Obj->torch, al_get_time());
    ALLEGRO_BITMAP* frame_chara = algif_get_bitmap(Obj->charactor, al_get_time());
    
    al_draw_bitmap(Obj->background, 0, 0, 0);

    if (frame_torch)
    {
        int offset = PIXEL_SIZE;
        for (int i = 0; i < 5; i++) {
            al_draw_bitmap(frame_torch, offset, HEIGHT / 3, 0);
            offset += PIXEL_SIZE * 6;
        }
    }
    if (frame_chara)
    {
        al_draw_bitmap(frame_chara, 13 * PIXEL_SIZE, HEIGHT / 1.6, 0);
    }
    
    al_draw_text(Obj->font_large, al_map_rgb(255, 255, 255), Obj->title_x, Obj->title_y, ALLEGRO_ALIGN_CENTRE, "G o o d  K n i g h t");
    al_draw_text(Obj->font_small, al_map_rgb(255, 255, 255),  14.5 * PIXEL_SIZE, HEIGHT / 1.2, ALLEGRO_ALIGN_CENTRE, "Press Enter to Start");
    al_play_sample_instance(Obj->sample_instance);
}
void menu_destroy(Scene *self)
{
    Menu *Obj = ((Menu *)(self->pDerivedObj));
    ALLEGRO_BITMAP *background = Obj->background;
    al_destroy_bitmap(background);
    algif_destroy_animation(Obj->torch);
    algif_destroy_animation(Obj->charactor);
    al_destroy_font(Obj->font_large);
    al_destroy_font(Obj->font_small);
    al_destroy_sample(Obj->song);
    al_destroy_sample_instance(Obj->sample_instance);
    free(Obj);
    free(self);
}

