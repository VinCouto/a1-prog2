#ifndef __PIT_H__
#define __PIT_H__

#include <allegro5/allegro5.h>
#include "Square.h"

typedef struct {
    float pos_x;
    float pos_y;
    float width;
    float height;
    float respawn_x;     
    float respawn_y;
    int damage;           
    int check_touch;      
} pit;

pit* pit_create(float pos_x, float pos_y, float width, float height, float respawn_x, float respawn_y, int damage);
void pit_destroy(pit* element);
int check_collision_pit(float player_x, float player_y, float player_width, float player_height, pit* p);
void apply_pit_effect(square* player, pit* p);

#endif