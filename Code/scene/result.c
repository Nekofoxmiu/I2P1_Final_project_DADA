#include <allegro5/allegro_primitives.h>
#include "result.h"
#include <stdbool.h>
/*
   [Result function]
*/
Scene *New_Result(int label)
{
    Result *pDerivedObj = (Result *)malloc(sizeof(Result));
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

    // Load sound
    pDerivedObj->song_enter = al_load_sample("assets/sound/enter.wav");
    if (pDerivedObj->song_enter == NULL) printf("Error");
    al_reserve_samples(20);
    pDerivedObj->sample_instance_enter = al_create_sample_instance(pDerivedObj->song_enter);
    al_set_sample_instance_playmode(pDerivedObj->sample_instance_enter, ALLEGRO_PLAYMODE_ONCE);
    al_restore_default_mixer();
    al_attach_sample_instance_to_mixer(pDerivedObj->sample_instance_enter, al_get_default_mixer());
    // set the volume of instance
    al_set_sample_instance_gain(pDerivedObj->sample_instance_enter, 1);

    // title
    pDerivedObj->title_x = WIDTH / 2;
    pDerivedObj->title_y = HEIGHT / 6;
    // torch
    pDerivedObj->torch = algif_new_gif("assets/image/menu/torch.gif", -1);
    // charactor
    pDerivedObj->charactor = algif_new_gif("assets/image/menu/knight.gif", -1);
    pObj->pDerivedObj = pDerivedObj;
    // setting derived object function
    pObj->Update = result_update;
    pObj->Draw = result_draw;
    pObj->Destroy = result_destroy;
    return pObj;
}
void result_update(Scene *self)
{
    if (key_state[ALLEGRO_KEY_ENTER])
    {
        Result *Obj = ((Result *)(self->pDerivedObj));
        al_play_sample_instance(Obj->sample_instance_enter);
        self->scene_end = true;
        window = 1;
    }
    return;
}
void result_draw(Scene *self)
{
    Result *Obj = ((Result *)(self->pDerivedObj));
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
    
    al_draw_text(Obj->font_large, al_map_rgb(255, 255, 255), Obj->title_x, Obj->title_y, ALLEGRO_ALIGN_CENTRE, "Y O U  D I E D");
    char record_text[50];
    sprintf(record_text, "Highest Record: %d Your Score: %d", (int)highest_record, (int)score);
    al_draw_text(Obj->font_small, al_map_rgb(255, 255, 255),  14.5 * PIXEL_SIZE, HEIGHT / 1.3, ALLEGRO_ALIGN_CENTRE, record_text);
    al_draw_text(Obj->font_small, al_map_rgb(255, 255, 255),  14.5 * PIXEL_SIZE, HEIGHT / 1.2, ALLEGRO_ALIGN_CENTRE, "Press Enter to Restart");
    al_play_sample_instance(Obj->sample_instance);
}
void result_destroy(Scene *self)
{
    Result *Obj = ((Result *)(self->pDerivedObj));
    ALLEGRO_BITMAP *background = Obj->background;
    al_destroy_bitmap(background);
    algif_destroy_animation(Obj->torch);
    algif_destroy_animation(Obj->charactor);
    al_destroy_font(Obj->font_large);
    al_destroy_font(Obj->font_small);
    al_destroy_sample(Obj->song);
    al_destroy_sample_instance(Obj->sample_instance);
    al_destroy_sample(Obj->song_enter);
    al_destroy_sample_instance(Obj->sample_instance_enter);
    free(Obj);
    free(self);
}

