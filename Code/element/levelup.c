#include "levelup.h"

#define FONT_SIZE 40
#define SPACE 200

const char *descriptions[] = {
    "Max HP +20%",
    "ATK +20%",
    "Defense +20%",
    "Speed +20%",
    "MAX MP +20%",
    "Bullet +1",
    "Poison ATK +20%"};

ALLEGRO_BITMAP *icons[7];

void load_icons()
{
    icons[0] = al_load_bitmap("assets/image/upgrade/icon/blood.png");
    if (!icons[0])
    {
        printf("Failed to load blood.png\n");
    }
    icons[1] = al_load_bitmap("assets/image/upgrade/icon/damage.png");
    if (!icons[1])
    {
        printf("Failed to load damage.png\n");
    }
    icons[2] = al_load_bitmap("assets/image/upgrade/icon/armor.png");
    if (!icons[2])
    {
        printf("Failed to load armor.png\n");
    }
    icons[3] = al_load_bitmap("assets/image/upgrade/icon/speed.png");
    if (!icons[3])
    {
        printf("Failed to load speed.png\n");
    }
    icons[4] = al_load_bitmap("assets/image/upgrade/icon/mp.png");
    if (!icons[4])
    {
        printf("Failed to load mp.png\n");
    }
    icons[5] = al_load_bitmap("assets/image/upgrade/icon/bullet.png");
    if (!icons[5])
    {
        printf("Failed to load bullet.png\n");
    }
    icons[6] = al_load_bitmap("assets/image/upgrade/icon/poison.png");
    if (!icons[6])
    {
        printf("Failed to load poison.png\n");
    }
}

Elements *New_Levelup(int label, Character *player)
{
    Levelup *pDerivedObj = (Levelup *)malloc(sizeof(Levelup));
    Elements *pObj = New_Elements(label);
    // setting derived object member
    pDerivedObj->font = al_load_ttf_font("assets/font/pixel_font.ttf", FONT_SIZE, 0);
    pDerivedObj->background = al_load_bitmap("assets/image/upgrade/background.jpg");
    if (!pDerivedObj->background)
    {
        printf("Failed to load background.jpg\n");
    }
    load_icons();
    pDerivedObj->width = al_get_bitmap_width(pDerivedObj->background);
    pDerivedObj->height = al_get_bitmap_height(pDerivedObj->background);
    pDerivedObj->x = 0;
    pDerivedObj->y = 0;
    pDerivedObj->player = player;
    pDerivedObj->mouse_button_pressed = true;

    // Initialize enhancements (randomly select 3 enhancements)
    for (int i = 0; i < 3; i++)
    {
        int randIndex = rand() % 7;
        pDerivedObj->enhancements[i].description = descriptions[randIndex];
        pDerivedObj->enhancements[i].icon = icons[randIndex];
        pDerivedObj->enhancements[i].width = al_get_bitmap_width(icons[randIndex]);
        pDerivedObj->enhancements[i].height = al_get_bitmap_height(icons[randIndex]);
        if (!icons[randIndex])
        {
            printf("Failed to load icon for enhancement %d\n", randIndex);
        }
    }

    // setting derived object function
    pObj->pDerivedObj = pDerivedObj;
    pObj->Update = Levelup_update;
    pObj->Interact = Levelup_interact;
    pObj->Draw = Levelup_draw;
    pObj->Destroy = Levelup_destory;
    return pObj;
}

void Levelup_update(Elements *self)
{
    static bool everRelease = false;
    Levelup *Obj = ((Levelup *)(self->pDerivedObj));

    if (mouse_state[1] && !Obj->mouse_button_pressed && everRelease)
    {
        Levelup_handle_mouse_click(Obj, self, mouse.x, mouse.y);
        Obj->mouse_button_pressed = true; // Set to true to indicate the button was pressed
    }
    else if (!mouse_state[1])
    {
        everRelease = true;
        Obj->mouse_button_pressed = false; // Reset when the button is released
        Levelup_handle_mouse_hover(Obj, mouse.x, mouse.y);
    }
}

void Levelup_handle_mouse_click(Levelup *Obj, Elements *self, int mouse_x, int mouse_y)
{
    int startX = (WIDTH - (3 * Obj->enhancements[0].width + 2 * SPACE)) / 2;
    int y = HEIGHT / 2;

    for (int i = 0; i < 3; i++)
    {
        int x = startX + i * (Obj->enhancements[i].width + SPACE);
        if (mouse_x >= x && mouse_x <= x + Obj->enhancements[i].width && mouse_y >= y && mouse_y <= y + Obj->enhancements[i].width)
        {
            // Enhancement selected, apply the enhancement
            // For example, if it increases health, add to player's health
            if (strcmp(Obj->enhancements[i].description, "Max HP +20%") == 0)
            {
                Obj->player->max_blood += Obj->player->max_blood * 0.2;
            }
            else if (strcmp(Obj->enhancements[i].description, "ATK +20%") == 0)
            {
                Obj->player->damage += Obj->player->damage * 0.2;
            }
            else if (strcmp(Obj->enhancements[i].description, "Defense +20%") == 0)
            {
                Obj->player->armor += Obj->player->armor * 0.2;
            }
            else if (strcmp(Obj->enhancements[i].description, "Speed +20%") == 0)
            {
                Obj->player->speed += Obj->player->speed * 0.2;
            }
            else if (strcmp(Obj->enhancements[i].description, "MAX MP +20%") == 0)
            {
                Obj->player->max_mp += Obj->player->max_mp * 0.2;
            }
            else if (strcmp(Obj->enhancements[i].description, "Bullet +1") == 0)
            {
                Obj->player->bullet_num += 1;
            }
            else if (strcmp(Obj->enhancements[i].description, "Poison ATK +20%") == 0)
            {
                Obj->player->poison_damage += Obj->player->poison_damage * 0.2;
            }
            everything_stop = false;
            al_set_system_mouse_cursor(displayptr, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
            self->dele = true;
        }
    }
}

void Levelup_handle_mouse_hover(Levelup *Obj, int mouse_x, int mouse_y)
{
    int startX = (WIDTH - (3 * Obj->enhancements[0].width + 2 * SPACE)) / 2;
    int y = HEIGHT / 2;

    bool hovering = false;

    for (int i = 0; i < 3; i++)
    {
        int x = startX + i * (Obj->enhancements[i].width + SPACE);
        if (mouse_x >= x && mouse_x <= x + Obj->enhancements[i].width && mouse_y >= y && mouse_y <= y + Obj->enhancements[i].width)
        {
            hovering = true;
            break;
        }
    }

    if (hovering)
    {
        al_set_system_mouse_cursor(displayptr, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    }
    else
    {
        al_set_system_mouse_cursor(displayptr, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    }
}

void Levelup_interact(Elements *self, Elements *tar) {}

void Levelup_draw(Elements *self, float camera_offset_x, float camera_offset_y)
{
    Levelup *Obj = ((Levelup *)(self->pDerivedObj));
    al_draw_bitmap(Obj->background, Obj->x, Obj->y, 0);

    // Draw the three enhancement options
    int startX = (WIDTH - (3 * Obj->enhancements[0].width + 2 * SPACE)) / 2; // Center the enhancements
    int y = HEIGHT / 2;

    for (int i = 0; i < 3; i++)
    {
        int x = startX + i * (Obj->enhancements[i].width + SPACE);
        al_draw_bitmap(Obj->enhancements[i].icon, x, y, 0);
        al_draw_text(Obj->font, al_map_rgb(255, 255, 255), x + Obj->enhancements[i].width / 2, y + Obj->enhancements[i].height + 10, ALLEGRO_ALIGN_CENTER, Obj->enhancements[i].description);
    }
}

void Levelup_destory(Elements *self)
{
    Levelup *Obj = ((Levelup *)(self->pDerivedObj));
    al_destroy_bitmap(Obj->background);
    for (int i = 0; i < 7; i++)
    {
        al_destroy_bitmap(icons[i]);
    }
    free(Obj);
    free(self);
}
