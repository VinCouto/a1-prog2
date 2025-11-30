#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "obstacle.h"

obstacle* obstacle_create(float pos_x, float pos_y, unsigned short width, unsigned short height, const char* sprite_path, int damage, float knockback_force){
    obstacle* new_obstacle = (obstacle*) malloc(sizeof(obstacle));
    if (!new_obstacle) return NULL;
    new_obstacle->pos_x = pos_x;
    new_obstacle->pos_y = pos_y;
    new_obstacle->width = width;
    new_obstacle->height = height;
    new_obstacle->damage = damage;
    new_obstacle->knockback_force = knockback_force;
    new_obstacle->check_touch = 0; 

    if(sprite_path != NULL){
        new_obstacle->sprite = al_load_bitmap(sprite_path);
        if (!new_obstacle->sprite) {
            fprintf(stderr, "Failed to load obstacle sprite from %s\n", sprite_path);
            free(new_obstacle);
            return NULL;
        }
    } else {
        new_obstacle->sprite = NULL; 
    }
    return new_obstacle;
}

int check_collision_obstacle(square* player, obstacle* obs){
    float p_esq   = player->x - player->width/2.0f;
    float p_dir   = player->x + player->width/2.0f;
    float p_cima  = player->y - player->heigth/2.0f;
    float p_baixo = player->y + player->heigth/2.0f;

    float o_esq   = obs->pos_x - obs->width/2.0f;
    float o_dir   = obs->pos_x + obs->width/2.0f;
    float o_cima  = obs->pos_y - obs->height/2.0f;
    float o_baixo = obs->pos_y + obs->height/2.0f;


    if (p_dir > o_esq &&    
        p_esq < o_dir &&    
        p_baixo > o_cima && 
        p_cima < o_baixo) { 
        
        return 1; 
    }

    return 0; 
}

void apply_knockback(square* player, obstacle* obs) {

    float dx = player->x - obs->pos_x;
    float dy = player->y - obs->pos_y;
    
    float distance = sqrt(dx * dx + dy * dy);
    
    // Se a distância for muito pequena, evita divisão por zero
    if (distance < 1.0f) distance = 1.0f;
    
    float norm_x = dx / distance;
    float norm_y = dy / distance;
    
    // Aplica o knockback
    player->x += norm_x * obs->knockback_force;
    player->vy -= obs->knockback_force * 0.5f;  
}


int check_obstacles_with_map(square *p, obstacle **obstacles, int num_obstacles) {
    
    // Percorre todas os obstaculos existentes
    for (int i = 0; i < num_obstacles; i++) {
        if (obstacles[i] != NULL) {
            if (check_collision_obstacle(p, obstacles[i])) {
                return 1; // Bateu em alguém
            }
        }
    }
    
    return 0; 
}


void draw_obstacle(obstacle* obs, float camera_x, float camera_y) {
    float screen_x = obs->pos_x - camera_x;
    float screen_y = obs->pos_y - camera_y;

    if (obs->sprite) {
        al_draw_scaled_bitmap(obs->sprite, 0, 0, 
                              al_get_bitmap_width(obs->sprite), 
                              al_get_bitmap_height(obs->sprite),
                              screen_x - obs->width/2, 
                              screen_y - obs->height/2, 
                              obs->width, 
                              obs->height, 0);
    } else {
        al_draw_rectangle(screen_x - obs->width/2, screen_y - obs->height/2,
                          screen_x + obs->width/2, screen_y + obs->height/2, 
                          al_map_rgb(255, 0, 0), 3);
    }
}




void get_sprite_obstacle(obstacle* element, const char* sprite_path){
    element->sprite = al_load_bitmap(sprite_path);
    if (!element->sprite) {
        fprintf(stderr, "Failed to load obstacle sprite from %s\n", sprite_path);
    }
}


void obstacle_destroy(obstacle* element){
  if(element->sprite){
      al_destroy_bitmap(element->sprite);
  }
  free(element);
}