#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdlib.h>

#include "ChaoParede.h"

#define X_SCREEN 920
#define Y_SCREEN 640



wall* wall_create(unsigned short width, unsigned short height, unsigned short pos_x, unsigned short pos_y){
    wall* new_wall = (wall*) malloc(sizeof(wall));
    if (!new_wall) return NULL;
    new_wall->width = width;
    new_wall->height = height;
    new_wall->pos_x = pos_x;
    new_wall->pos_y = pos_y;
    new_wall->sprite = NULL; 
    new_wall->type = WALL_NORMAL; 
    new_wall->active = 1;         
    new_wall->was_stepped = 0;   
    new_wall->step_timer = 0; 
    return new_wall;
} 


void get_sprite_wall(wall* element, const char* sprite_path){
    element->sprite = al_load_bitmap(sprite_path);
    if (!element->sprite) {
        fprintf(stderr, "Failed to load wall sprite from %s\n", sprite_path);
    }
}

void wall_draw(wall* element, float camera_x, float camera_y){
    if (element->active == 0) return; // SE NÃO ESTIVER ATIVA, NÃO DESENHA!
    
    // Desenha parede com sprite se existir
    float wall1_screen_x = element->pos_x - camera_x;
    float wall1_screen_y = element->pos_y - camera_y;

    if (element->sprite) {
        al_draw_scaled_bitmap(element->sprite, 0, 0, 
                                al_get_bitmap_width(element->sprite), 
                                al_get_bitmap_height(element->sprite),
                                wall1_screen_x - element->width/2, 
                                wall1_screen_y - element->height/2, 
                                element->width, 
                                element->height, 0);
    } else {
        al_draw_rectangle(wall1_screen_x - element->width/2, wall1_screen_y - element->height/2, 
                            wall1_screen_x + element->width/2, wall1_screen_y + element->height/2, 
                            al_map_rgb(255, 255, 0), 3);
    }
}

// Função para verificar colisão entre PLAYER e PAREDE 
int check_collision_wall(square *player, wall *w) {
    if (w->active == 0) return 0;   // se nao estiver ativa o player atravessa


    float p_esq   = player->x - player->width/2.0f;
    float p_dir   = player->x + player->width/2.0f;
    float p_cima  = player->y - player->heigth/2.0f;
    float p_baixo = player->y + player->heigth/2.0f;

    float w_esq   = w->pos_x - w->width/2.0f;
    float w_dir   = w->pos_x + w->width/2.0f;
    float w_cima  = w->pos_y - w->height/2.0f;
    float w_baixo = w->pos_y + w->height/2.0f;

    // Se todas as condições forem verdadeiras, existe sobreposição (colisão)
    if (p_dir > w_esq &&    
        p_esq < w_dir &&    
        p_baixo > w_cima && 
        p_cima < w_baixo) { 
        
        return 1; 
    }

    return 0; 
}

int check_hitbox_vs_wall(square *p, Hitbox box, wall *w) {
    if (!box.active) return 0; // Se a hitbox estiver desligada, ignora

    float hb_esq   = (p->x + box.offset_x) - box.width / 2.0f;
    float hb_dir   = (p->x + box.offset_x) + box.width / 2.0f;
    float hb_cima  = (p->y + box.offset_y) - box.height / 2.0f;
    float hb_baixo = (p->y + box.offset_y) + box.height / 2.0f;

    float w_esq   = w->pos_x - w->width / 2.0f;
    float w_dir   = w->pos_x + w->width / 2.0f;
    float w_cima  = w->pos_y - w->height / 2.0f;
    float w_baixo = w->pos_y + w->height / 2.0f;

    if (hb_dir >= w_esq && 
        hb_esq <= w_dir && 
        hb_baixo >= w_cima && 
        hb_cima <= w_baixo) {
        printf("Collision detected between hitbox and wall!\n");
        return 1; // Colidiu
    }

    return 0;
}


int check_collision_with_map(square *p, Hitbox box, wall **walls, int num_walls) {
    
    // Percorre todas as paredes existentes
    for (int i = 0; i < num_walls; i++) {
        if (walls[i] != NULL) {
            if (check_hitbox_vs_wall(p, box, walls[i])) {
                return 1; // Bateu em alguém
            }
        }
    }
    return 0; 
}

void wall_destroy(wall* element){
    free(element);
}