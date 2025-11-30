#ifndef __CHAO_PAREDE__
#define __CHAO_PAREDE__

#define WALL_NORMAL 0
#define WALL_VANISHING 1
#define STEP_DURATION 5

#include "Square.h"
#include <allegro5/allegro5.h>

struct wall {
    unsigned short width;
    unsigned short height;
    unsigned short pos_x;
    unsigned short pos_y;
    ALLEGRO_BITMAP *sprite;
    int type;           // 0 = Normal, 1 = Vanishing 
    int active;         // 1 = Existe, 0 = Desapareceu
    int was_stepped;    // 0 = Não foi pisada, 1 = Foi pisada
    int step_timer; 
};

// cria parede
wall* wall_create(unsigned short width, unsigned short height, unsigned short pos_x, unsigned short pos_y);

// carrega sprite para parede
void get_sprite_wall(wall* element, const char* sprite_path);

// desenha parede
void wall_draw(wall* element, float camera_x, float camera_y);

// Função para verificar colisão entre PLAYER e PAREDE
int check_collision_wall(square *player, wall *w);

// Verifica colisão entre hitbox e parede
int check_hitbox_vs_wall(square *p, Hitbox box, wall *w);

// Verifica colisão entre hitbox e todas as paredes do mapa
int check_collision_with_map(square *p, Hitbox box, wall **walls, int num_walls);

// destrói parede
void wall_destroy(wall* element);

#endif