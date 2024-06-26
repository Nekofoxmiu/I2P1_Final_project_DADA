#ifndef DROP_H_INCLUDED
#define DROP_H_INCLUDED
#include "element.h"
#include "charater.h"
#include "../scene/gamescene.h" // for element label
#include "../shapes/Shape.h"
#include "../algif5/src/algif.h"
/*
   [drop object]
*/
typedef enum _DropType {
    XP_L,
    HP_L,
    MP_L
} DropType;

typedef struct _Drop
{
    int x, y;          // the position of image
    int width, height; // the width and height of image
    ALLEGRO_BITMAP *img;
    ALLEGRO_SAMPLE_INSTANCE *pick_Sound;
    Shape *hitbox; // the hitbox of object
    Character *target; // the character to track
    DropType type;
    double attract_distance;
    double attract_speed;
    int life;
    double amount;
} Drop;

Elements *New_Drop(int label, DropType type, Character *target, int x, int y, double amount);
void Drop_update(Elements *self);
void Drop_interact(Elements *self, Elements *tar);
void Drop_draw(Elements *self, float camera_offset_x, float camera_offset_y);
void Drop_destory(Elements *self);
void HandleDrop(DropConfig dropConfig, Character *target, Scene *scene, double x, double y);
void HandleOneTypeDrop(DropConfig dropConfig, Character *target, Scene *scene, double x, double y, int type);
void _Drop_update_position(Elements *self, int dx, int dy);

#endif
