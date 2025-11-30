#include <stdio.h>
#include <stdlib.h>

#include "pit.h"

pit* pit_create(float pos_x, float pos_y, float width, float height, float respawn_x, float respawn_y, int damage) {
    pit* new_pit = (pit*) malloc(sizeof(pit));
    if (!new_pit) return NULL;
    new_pit->pos_x = pos_x;
    new_pit->pos_y = pos_y;
    new_pit->width = width;
    new_pit->height = height;
    new_pit->respawn_x = respawn_x;
    new_pit->respawn_y = respawn_y;
    new_pit->damage = damage;
    new_pit->check_touch = 0;  
    return new_pit;
}



int check_collision_pit(float player_x, float player_y, float player_width, float player_height, pit* p){
    float p_esq   = player_x - player_width/2.0f;
    float p_dir   = player_x + player_width/2.0f;
    float p_cima  = player_y - player_height/2.0f;
    float p_baixo = player_y + player_height/2.0f;

    float pit_esq   = p->pos_x - p->width/2.0f;
    float pit_dir   = p->pos_x + p->width/2.0f;
    float pit_cima  = p->pos_y - p->height/2.0f;
    float pit_baixo = p->pos_y + p->height/2.0f;

    if (p_dir > pit_esq && 
        p_esq < pit_dir && 
        p_baixo > pit_cima && 
        p_cima < pit_baixo) {
        return 1;
    }
    return 0;
}

void apply_pit_effect(square* player, pit* p) {

    player->x = p->respawn_x;
    player->y = p->respawn_y;
    
    player->hp -= p->damage;
    
    player->vy = 0;
    player->idle = 0;
    
    printf("Player caiu no buraco! Teleportado para (%.1f, %.1f). HP: %d\n", 
           p->respawn_x, p->respawn_y, player->hp);
}

void pit_destroy(pit* element){
    free(element);
}