#ifndef __OBSTACLE__
#define __OBSTACLE__

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include "Square.h"

typedef struct{
    float pos_x;
    float pos_y;
    unsigned short width;
    unsigned short height;
    short int check_touch; //flag para checar se o obstáculo foi tocado para aplicar o dano ao personagem
    float knockback_force;         // Força do knockback
    ALLEGRO_BITMAP* sprite;
    int damage;
  }obstacle;

// cria obstaculo
obstacle* obstacle_create(float pos_x, float pos_y, unsigned short width, unsigned short height, const char* sprite_path, int damage, float knockback_force);

// destroi obstaculo
void obstacle_destroy(obstacle* element);

// checa colisao entre obstaculo e personagem
int check_collision_obstacle(square* player, obstacle* obs);

// checa colisao entre varios obstaculos e o personagem
int check_obstacles_with_map(square *p, obstacle **obstacles, int num_obstacles);

// desenha obstaculo
void draw_obstacle(obstacle* obs, float camera_x, float camera_y);

// carrega sprite do obstaculo
void get_sprite_obstacle(obstacle* element, const char* sprite_path);

// aplica knockback ao personagem
void apply_knockback(square* player, obstacle* obs);





#endif