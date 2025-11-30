#include <stdlib.h>
#include <stdio.h>
#include <allegro5/allegro5.h>																																								
#include <allegro5/allegro_font.h>																																									
#include <allegro5/allegro_primitives.h>																																								
#include <allegro5/allegro_image.h>

#include "Square.h"
#include "ChaoParede.h"


#define FRAME_WIDTH 120     // Largura de cada frame 
#define FRAME_HEIGHT 80     // Altura de cada frame

#define ROW_IDLE        0
#define ROW_WALK        1
#define ROW_CROUCH      2
#define ROW_CROUCH_WALK 3
#define ROW_JUMP        4


#define SPRITE_COLS 10
#define SPRITE_ROWS 5
#define TOTAL_FRAMES (SPRITE_COLS * SPRITE_ROWS)


square* square_create(int heigth, int width, unsigned char face, float x, float y, float max_x, float max_y){		

	if ((x - width/2 < 0) || (x + width/2 > max_x) || (y - heigth/2 < 0) || (y + heigth/2 > max_y)) return NULL;											
	if (face > 3) return NULL;																														

	square *new_square = (square*) malloc(sizeof(square));																								
	if (!new_square) return NULL;																																										
	new_square->heigth = heigth;																														
	new_square->width = width;																															
	
    
    new_square->body_box.width = width / 2;    
    new_square->body_box.height = heigth;      
    new_square->body_box.offset_x = 0;         
    new_square->body_box.offset_y = 0;         
    new_square->body_box.active = 1;           

    new_square->attack_box.width = 40;
    new_square->attack_box.height = 20;
    new_square->attack_box.offset_x = 30; 
    new_square->attack_box.offset_y = 0;
    new_square->attack_box.active = 0;    
    
	new_square->face = face;																														
	new_square->hp = 5;																																	
	new_square->x = x;																																	
	new_square->y = y;												
	new_square->idle = 0;
	new_square->vy = 0;
	new_square->sprite = NULL; 
	new_square->control = joystick_create();									
	new_square->invincibility_timer = 0; 
    new_square->original_height = heigth;
    new_square->draw_width = width;
    new_square->draw_height = heigth;

    new_square->sprite = NULL;
    new_square->anim_frame = 0;
    new_square->anim_counter = 0;
    new_square->current_anim = ANIM_IDLE;
    
    return new_square;									
}


void update_animation(square *player) {
    int new_anim = ANIM_IDLE;
    printf("idle: %d, left: %d, right: %d\n", player->idle, player->control->left, player->control->right);

    // Determina qual animação deve estar tocando
    if (player->idle == 2) {
        new_anim = ANIM_JUMP;
    }
    else if (player->control->ctr_pressed == 1) {
        if (player->control->left || player->control->right) {
            new_anim = ANIM_CROUCH_WALK;
        } else {
            new_anim = ANIM_CROUCH;
        }
    }
    else {
        if (player->control->left || player->control->right) {
            new_anim = ANIM_WALK;
        } else {
            new_anim = ANIM_IDLE;
        }
    }
    
    // Se mudou de animação, reinicia o frame e o contador
    if (new_anim != player->current_anim) {
        player->current_anim = new_anim;
        player->anim_frame = 0;
        player->anim_counter = 0;
    }
    
    player->anim_counter++;
    if (player->anim_counter >= 5) { // Velocidade da animação
        player->anim_counter = 0;
        player->anim_frame++;
        if(player->current_anim == ANIM_IDLE) {
            if (player->anim_frame >= 10) player->anim_frame = 0; // Idle tem 10 frames
        } else if (player->current_anim == ANIM_WALK) {
            if (player->anim_frame >= 10) player->anim_frame = 0; // Walk tem 10 frames
        } else if (player->current_anim == ANIM_CROUCH) {
            if (player->anim_frame >= 3) player->anim_frame = 0; // Crouch tem 3 frames
        } else if (player->current_anim == ANIM_CROUCH_WALK) {
            if (player->anim_frame >= 8) player->anim_frame = 0; // Crouch Walk tem 8 frames
        } else if (player->current_anim == ANIM_JUMP) {
            if (player->anim_frame >= 3) player->anim_frame = 0; // Jump tem 3 frames
        }
        if (player->anim_frame >= (SPRITE_COLS * SPRITE_ROWS)) {
            player->anim_frame = 0; // Reinicia a animação 
        }
    printf("Current Anim: %d, Frame: %d\n", player->current_anim, player->anim_frame);
    }
}

void draw_player_animated(square *player, float camera_x, float camera_y) {
    
    if (!player->sprite) {
        float player_screen_x = player->x - camera_x;
        float player_screen_y = player->y - camera_y;
        al_draw_filled_rectangle(
            player_screen_x - player->width/2,
            player_screen_y - player->heigth/2,
            player_screen_x + player->width/2,
            player_screen_y + player->heigth/2,
            al_map_rgb(255, 0, 0)
        );
        return;
    }

    int current_row = 0;

    if (player->current_anim == ANIM_IDLE) {
        current_row = ROW_IDLE; 
    }
    else if (player->current_anim == ANIM_WALK) {
        current_row = ROW_WALK; 
    }
    else if (player->current_anim == ANIM_CROUCH) {
        current_row = ROW_CROUCH; 
    }
    else if (player->current_anim == ANIM_JUMP) {
        current_row = ROW_JUMP; 
    }
    else if (player->current_anim == ANIM_CROUCH_WALK) {
        current_row = ROW_CROUCH_WALK; 
    }

    // frame_x = Coluna * Largura do Frame
    // frame_y = Linha * Altura do Frame
    int frame_x = player->anim_frame * FRAME_WIDTH;
    int frame_y = current_row * FRAME_HEIGHT;

    float player_screen_x = player->x - camera_x;
    float player_screen_y = player->y - camera_y;

    // Assumindo que face 0 é esquerda e 1 é direita
    int flags = 0;
    if (player->control->left == 1) { 
        flags = ALLEGRO_FLIP_HORIZONTAL;
    }

    // 6. Desenha!
    al_draw_scaled_bitmap(
        player->sprite,
        frame_x, frame_y,           
        FRAME_WIDTH, FRAME_HEIGHT,  // Tamanho do recorte (120x80)
        player_screen_x - player->draw_width / 2, 
        player_screen_y - player->draw_height + 10,
        player->draw_width,         
        player->draw_height,        
        flags                       // Espelhamento
    );
}

void square_move(square *element, char steps, unsigned char trajectory, float max_x, float max_y){									//Implementação da função "square_move"

    if (!trajectory){ if ((element->x - steps*SQUARE_STEP) - element->width/2 >= 0) element->x = element->x - steps*SQUARE_STEP;} 						//Verifica se a movimentação para a esquerda é desejada e possível; se sim, efetiva a mesma
    else if (trajectory == 1){ if ((element->x + steps*SQUARE_STEP) + element->width/2 <= max_x) element->x = element->x + steps*SQUARE_STEP;}			//Verifica se a movimentação para a direita é desejada e possível; se sim, efetiva a mesma
    else if (trajectory == 2){ if ((element->y - steps*SQUARE_STEP) - element->heigth/2 >= 0) element->y = element->y - steps*SQUARE_STEP;}				//Verifica se a movimentação para cima é desejada e possível; se sim, efetiva a mesma
    else if (trajectory == 3){ if ((element->y + steps*SQUARE_STEP) + element->heigth/2 <= max_y) element->y = element->y + steps*SQUARE_STEP;}			//Verifica se a movimentação para baixo é desejada e possível; se sim, efetiva a mesma
    else if (trajectory == 4 ){

        // Só permite alterar agachamento se NÃO estiver no ar (idle != 2)
        if (element->idle != 2) {

            // Se está em pé (0) e apertou o botão, agacha
            if(element->control->ctr == 1 && element->idle == 0 && element->control->ctr_pressed == 0){
                printf("agachando\n");
                element->heigth = element->original_height / 2;  
                element->body_box.height = element->heigth;
                element->idle = 1;
                element->control->ctr = 0; // Reseta o estado do botão
                element->control->ctr_pressed = 1; 
            } 

            else if(element->control->ctr == 1 && element->control->ctr_pressed == 1){
                printf("levantando\n");
                element->heigth = element->original_height;  
                element->body_box.height = element->heigth;
                element->idle = 0;
                element->control->ctr = 0; // Reseta o estado do botão
                element->control->ctr_pressed = 0; // Reseta o processamento do botão
            }  

        }
    }

}

void square_destroy(square *element){																													//Implementação da função "square_destroy"
    if (element->sprite) {
        al_destroy_bitmap(element->sprite);
    }
    if (element->control) {
        free(element->control);
    }
    free(element);																																//Libera a memória do quadrado na heap
}


