#include <allegro5/allegro5.h>																																											
#include <allegro5/allegro_font.h>																																										
#include <allegro5/allegro_primitives.h>																																								
#include <allegro5/allegro_image.h>
#include <stdio.h>
#include <stdlib.h>

#include "mapa.h"																																											
#include "ChaoParede.h"
#include "menu.h"	
#include "obstacle.h"
#include "pit.h"

#define X_SCREEN 920
#define Y_SCREEN 640																																													

#define MAX_WALLS 50
#define MAX_OBSTACLES 20
#define MAX_PITS 10

#define X_BACKGROUND 4800																																											
#define Y_BACKGROUND 1280																																													


wall* check_collision_map(square *p, wall **walls, int num_walls) {
    for (int i = 0; i < num_walls; i++) {
        if (walls[i] != NULL) {
            if (check_collision_wall(p, walls[i])) {
                return walls[i]; 
            }
        }
    }
    return NULL; 
}


int check_collision_horizontal(square *p, wall **walls, int num_walls) {
    for (int i = 0; i < num_walls; i++) {
        if (walls[i] != NULL) {
            float p_esq   = p->x - p->width/2.0f;
            float p_dir   = p->x + p->width/2.0f;
            float p_cima  = p->y - p->heigth/2.0f;
            float p_baixo = p->y + p->heigth/2.0f;

            float w_esq   = walls[i]->pos_x - walls[i]->width/2.0f;
            float w_dir   = walls[i]->pos_x + walls[i]->width/2.0f;
            float w_cima  = walls[i]->pos_y - walls[i]->height/2.0f;
            float w_baixo = walls[i]->pos_y + walls[i]->height/2.0f;

            // Ignora colisão se player está apenas tocando o topo (chão)
            if (p_dir > w_esq && p_esq < w_dir && p_baixo > w_cima && p_cima < w_baixo) {
                float left_overlap = p_dir - w_esq;
                float right_overlap = w_dir - p_esq;
                float top_overlap = p_baixo - w_cima;
                float bottom_overlap = w_baixo - p_cima;
                float min_overlap = left_overlap;
                if (right_overlap < min_overlap) min_overlap = right_overlap;
                
                // Só detecta colisão se vier dos lados
                if ((left_overlap < right_overlap && left_overlap < top_overlap && left_overlap <= bottom_overlap) ||
                    (right_overlap < left_overlap && right_overlap < top_overlap && right_overlap <= bottom_overlap)) {
                    return 1; 
                }
            }
        }
    }
    return 0;
}

void update_life(square* victim, ALLEGRO_FONT* font, ALLEGRO_BITMAP* heart_container){
	if(victim->hp == 5){
		al_draw_bitmap(heart_container, 100, 75, 0);	
		al_draw_bitmap(heart_container, 140, 75, 0);
		al_draw_bitmap(heart_container, 180, 75, 0);
		al_draw_bitmap(heart_container, 220, 75, 0);
		al_draw_bitmap(heart_container, 260, 75, 0);														
	} else {	
		if(victim->hp == 4){
			al_draw_bitmap(heart_container, 100, 75, 0);
			al_draw_bitmap(heart_container, 140, 75, 0);
			al_draw_bitmap(heart_container, 180, 75, 0);
			al_draw_bitmap(heart_container, 220, 75, 0);
		} else {
			if(victim->hp == 3){
				al_draw_bitmap(heart_container, 100, 75, 0);
				al_draw_bitmap(heart_container, 140, 75, 0);
				al_draw_bitmap(heart_container, 180, 75, 0);
			} else {
				if(victim->hp == 2){
					al_draw_bitmap(heart_container, 100, 75, 0);
					al_draw_bitmap(heart_container, 140, 75, 0);
				} else {
					if(victim->hp == 1){
						al_draw_bitmap(heart_container, 100, 75, 0);
					}
				}
			}
		}
	}
}


int check_hitbox_collision(square *p1, Hitbox box1, square *p2, Hitbox box2) {
    if (!box1.active || !box2.active) return 0;

    // Calcular posição absoluta no mundo
    int p1_left   = (p1->x + box1.offset_x) - box1.width / 2;
    int p1_right  = (p1->x + box1.offset_x) + box1.width / 2;
    int p1_top    = (p1->y + box1.offset_y) - box1.height / 2;
    int p1_bottom = (p1->y + box1.offset_y) + box1.height / 2;

    int p2_left   = (p2->x + box2.offset_x) - box2.width / 2;
    int p2_right  = (p2->x + box2.offset_x) + box2.width / 2;
    int p2_top    = (p2->y + box2.offset_y) - box2.height / 2;
    int p2_bottom = (p2->y + box2.offset_y) + box2.height / 2;

    if (p1_right > p2_left && 
        p1_left < p2_right && 
        p1_bottom > p2_top && 
        p1_top < p2_bottom) {
        return 1;
    }
    return 0;
}


unsigned char collision_2D(square *element_first, square *element_second){

	if ((((element_second->y-element_second->heigth/2 >= element_first->y-element_first->heigth/2) && (element_first->y+element_first->heigth/2 >= element_second->y-element_second->heigth/2)) || 	
		((element_first->y-element_first->heigth/2 >= element_second->y-element_second->heigth/2) && (element_second->y+element_second->heigth/2 >= element_first->y-element_first->heigth/2))) && 	 
		(((element_second->x-element_second->width/2 >= element_first->x-element_first->width/2) && (element_first->x+element_first->width/2 >= element_second->x-element_second->width/2)) || 
		((element_first->x-element_first->width/2 >= element_second->x-element_second->width/2) && (element_second->x+element_second->width/2 >= element_first->x-element_first->width/2)))) return 1;
	else return 0;
}

unsigned char check_kill(square *victim){																																					

	if(victim->hp <= 0)																																										
		return 1;																																											
	else	
		return 0;																					
}


void update_position(square *player, wall **walls, int num_walls) {
    
    if (player->control->left) {
        square_move(player, 1, 0, X_BACKGROUND, Y_BACKGROUND);
        
        if (check_collision_horizontal(player, walls, num_walls)) {
            square_move(player, 1, 1, X_BACKGROUND, Y_BACKGROUND);
        }
    }

    if (player->control->right) {
        square_move(player, 1, 1, X_BACKGROUND, Y_BACKGROUND);
        
        if (check_collision_horizontal(player, walls, num_walls)) {
          square_move(player, 1, 0, X_BACKGROUND, Y_BACKGROUND);
        }
    }

    if (player->control->ctr) {
        square_move(player, 1, 4, X_BACKGROUND, Y_BACKGROUND);
    }

    if (player->control->up && player->idle != 2) {
        // Se estava agachado, levanta antes de pular
        if (player->idle == 1) {
            player->heigth = player->heigth * 2; 
            player->y = player->y - player->heigth/4; 
            player->idle = 0;
        }

        // Aplica a força do pulo
        player->vy = FORCA_PULO; 
        player->idle = 2;		// Estado: No Ar
    }
}



void update_physics(square *player, wall **walls, int num_walls) {
    
  player->vy += GRAVIDADE;
  if (player->vy > 20.0f) player->vy = 20.0f;

	player->y += player->vy;

  wall* hit_wall = check_collision_map(player, walls, num_walls);
    
  if (hit_wall != NULL) {
        
    if (player->vy > 0) {
      // Fica exatamente em cima da parede
      player->y = (hit_wall->pos_y - hit_wall->height/2) - player->heigth/2;
      player->vy = 0; 
            
      if (player->idle == 2) {
        player->idle = 0; 
      }
    }
        
    else if (player->vy < 0) {
      // Fica exatamente abaixo da parede
      player->y = (hit_wall->pos_y + hit_wall->height/2) + player->heigth/2;   
      player->vy = 0;
    }
  }
  //verifica se está de pé ou no ar
  else {
    // Se estava de pé (idle != 2) e agora saiu do chão, vai para o ar
    if (player->idle == 0 || player->idle == 1) {
      player->idle = 2; // Estado: No ar
      }
  }
}



int update_enemy(square* enemy, square* player_1, ALLEGRO_FONT* font){																																//Função que atualiza o inimigo no jogo
		
	al_draw_text(font, al_map_rgb(255,255,255), 400, 75, 0, "Enemy HP:");
	char hp_str[4];
	sprintf(hp_str, "%d", enemy->hp);
	al_draw_text(font, al_map_rgb(255,255,255), 500, 75, 0, hp_str);

	if(collision_2D(enemy, player_1)){
		if (player_1->invincibility_timer == 0) {
			
			// Empurrão
			if (player_1->x < enemy->x) {
				player_1->x -= 20; 
				player_1->vy = -5; 
			} else {
				player_1->x += 20; 
				player_1->vy = -5;
			}
			
			player_1->hp--;
			player_1->invincibility_timer = 60; // Ativa cooldown
			}
	}
		

	if(check_kill(enemy))
		enemy->hp--;

	int steps = (rand()%6);
	int trajetory = (rand()%2);
	int move = rand()%11;
	if(!move)
		square_move(enemy,steps, trajetory, X_BACKGROUND, Y_BACKGROUND);

	if(enemy->hp == 0)
		return 0;
	else
		return 1;
}

void update_vanishing_platforms(square *player, wall **walls, int num_walls) {
	for (int i = 0; i < num_walls; i++) {
		if (walls[i] != NULL && walls[i]->active == 1 && walls[i]->type == WALL_VANISHING) {
				
			int touching = check_hitbox_vs_wall(player, player->body_box, walls[i]);
			
			if (touching) {
					// Player está tocando: incrementa timer
					walls[i]->step_timer++;
					
					// Se pisou por tempo suficiente
					if (walls[i]->step_timer >= STEP_DURATION) {
							walls[i]->was_stepped = 1;
					}
			} 
			else {
					// Player saiu da plataforma
					if (walls[i]->was_stepped == 1) {
							walls[i]->active = 0;
					}
					
					// Reset timer quando não está tocando
					walls[i]->step_timer = 0;
			}
	}
}
}


int main(){
	
	al_init();																																															
	al_init_primitives_addon();																																											
	al_init_font_addon();																																													
	al_init_image_addon();

	al_install_keyboard();																																													

	ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);																																				
	ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();																																					 
	ALLEGRO_FONT* font = al_create_builtin_font();														
	ALLEGRO_DISPLAY* disp = al_create_display(X_SCREEN, Y_SCREEN);										

	al_register_event_source(queue, al_get_keyboard_event_source());																																		
	al_register_event_source(queue, al_get_display_event_source(disp));																																		
	al_register_event_source(queue, al_get_timer_event_source(timer));																																	
	
	
	
	printf("antes do menu\n");
	int option = 0;																																				
	printf("passei do menu\n");
	

	while(1){
	switch (option){
		case 0:
		{
			printf("entrando menu\n");
			option = menu(disp, X_SCREEN, Y_SCREEN);
			break;
		}


	 	case 1:
		{
			printf("iniciando jogo\n");
			square* player_1 = square_create(20, 20, 1, 10, Y_SCREEN/2, X_SCREEN, Y_SCREEN);																												
			if (!player_1) return 1;																																												

			player_1->draw_width = 120;
			player_1->draw_height = 80;

			player_1->sprite = al_load_bitmap("player_spritesheet.png");
			if (!player_1->sprite) {
    		fprintf(stderr, "Failed to load player spritesheet!\n");
    		return -1;
			}
			
			
			ALLEGRO_EVENT event;																																								
			al_start_timer(timer);															
			unsigned char p1k = 0;																																										
			

			ALLEGRO_BITMAP* background = NULL;	
			background = map_create();	
			if (!background) {
				fprintf(stderr, "Failed to load background image!\n");
				return -1;
			}

			ALLEGRO_BITMAP* vitory_screen = NULL;
			vitory_screen = al_load_bitmap("vitoria.png");
			if (!vitory_screen) {
				fprintf(stderr, "Failed to load victory screen image!\n");
				return -1;
			}

			ALLEGRO_BITMAP* heart_container = NULL;
			heart_container = al_load_bitmap("coracao.png");
			if (!heart_container) {
				fprintf(stderr, "Failed to load heart container image!\n");
				return -1;
			}
			
			
			obstacle* map_obstacles[MAX_OBSTACLES]; 

			for(int i=0; i<MAX_OBSTACLES; i++) map_obstacles[i] = NULL;
			int obstacle_count = 0;

			map_obstacles[0] = obstacle_create(200, 900, 50, 50, "obstaculo1.png", 1, 15.0f);
			obstacle_count++;
			get_sprite_obstacle(map_obstacles[0], "obstaculo1.png");

			map_obstacles[1] = obstacle_create(700, 900, 50, 50, "obstaculo1.png", 1, 15.0f);
			obstacle_count++;
			get_sprite_obstacle(map_obstacles[1], "obstaculo1.png");

			map_obstacles[2] = obstacle_create(1035, 150 ,100 , 128, "obstaculo1.png", 1, 5.0f);
			obstacle_count++;
			get_sprite_obstacle(map_obstacles[2], "bomba.png");
		
			map_obstacles[3] = obstacle_create(1415, 200 ,120 , 156, "bomba.png", 1, 15.0f);
			obstacle_count++;
			get_sprite_obstacle(map_obstacles[3], "bomba.png");

			map_obstacles[4] = obstacle_create(2860, 225 ,120 , 156, "bomba.png", 1, 5.0f);
			obstacle_count++;
			get_sprite_obstacle(map_obstacles[4], "bomba.png");

			map_obstacles[5] = obstacle_create(3150, 1190 ,300 , 100, "varal.png", 1, 10.0f);
			obstacle_count++;
			get_sprite_obstacle(map_obstacles[5], "varal.png");

			map_obstacles[6] = obstacle_create(4050, 1050 , 300, 100, "varal.png", 1, 20.0f);
			obstacle_count++;
			get_sprite_obstacle(map_obstacles[6], "varal.png");

			map_obstacles[7] = obstacle_create(1475, 875 , 450, 100, "bixaogrande.png", 3, 30.0f);
			obstacle_count++;
			get_sprite_obstacle(map_obstacles[7], "bixaogrande.png");


			pit* map_pits[MAX_PITS];
			for(int i=0; i<MAX_PITS; i++)
				map_pits[i] = NULL;
			int pit_count = 0;

			// Criar buracos - formato: pit_create(pos_x, pos_y, width, height, respawn_x, respawn_y, damage)
			map_pits[0] = pit_create(2375, 1280, 50, 10, 2100, 1250, 1);  
			pit_count++;

			map_pits[1] = pit_create(2550, 1280, 100, 10, 2100, 1250, 1);  // Outro buraco
			pit_count++;

			map_pits[2] = pit_create(3925, 1280, 100, 10, 3700, 1250, 1);
			pit_count++;

			map_pits[3] = pit_create(4300, 1280, 300, 10, 3700, 1250, 1);
			pit_count++;

			map_pits[4] = pit_create(3450, 675, 2700, 50, 2150, 300, 2);
			pit_count++;


			wall* map_walls[MAX_WALLS];
			
			// Inicializa tudo como NULL para segurança
			for(int i=0; i<MAX_WALLS; i++) map_walls[i] = NULL;
			int wall_count = 0;

			// Cria as paredes e adiciona no array
			map_walls[0] = wall_create(1856, 65, 928, 957);  // Chão
			wall_count++;

			map_walls[1] = wall_create(200, 40, 650, 840);   // Obstáculo 1
			wall_count++;
			get_sprite_wall(map_walls[1], "plataforma3.png");

			map_walls[2] = wall_create(50, 50, 475, 725);  // Plataforma 2
			wall_count++;

			map_walls[3] = wall_create(50, 50, 375, 635);  // Plataforma 3
			wall_count++;

			map_walls[4] = wall_create(50, 50, 575, 635);  // Plataforma 4
			wall_count++;

			map_walls[5] = wall_create(50, 50, 500, 540);  // Plataforma 5
			wall_count++;

			map_walls[6] = wall_create(90, 25, 685, 562);
			wall_count++;
			get_sprite_wall(map_walls[6], "plataforma3.png");
			
			map_walls[7] = wall_create(50,350, 850,750);
			wall_count++;
			get_sprite_wall(map_walls[7], "plataforma2.png");
			
			map_walls[8] = wall_create(50,100, 850,450);
			wall_count++;
			get_sprite_wall(map_walls[8], "plataforma2.png");

			map_walls[9] = wall_create(50,50, 775,475);
			wall_count++;
			get_sprite_wall(map_walls[9], "plataforma1.png");
		
			map_walls[10] = wall_create(75,75, 912,342);
			wall_count++;
			get_sprite_wall(map_walls[10], "plataforma1.png");

			map_walls[11] = wall_create(50,50, 1125,362);
			wall_count++;
			get_sprite_wall(map_walls[11], "plataforma1.png");

			map_walls[12] = wall_create(200,40, 1300,362);
			wall_count++;
			get_sprite_wall(map_walls[12], "plataforma3.png");

			map_walls[13] = wall_create(50, 50, 1425,490);
			wall_count++;
			get_sprite_wall(map_walls[13], "plataforma1.png");

			map_walls[14] = wall_create(50, 50, 1500,370);
			wall_count++;
			get_sprite_wall(map_walls[14], "plataforma1.png");

			map_walls[15] = wall_create(130, 40, 1865, 310);
			wall_count++;
			get_sprite_wall(map_walls[15], "plataforma3.png");

			map_walls[16] = wall_create(38, 94, 1916, 222);
			wall_count++;
			get_sprite_wall(map_walls[16], "plataforma2.png");

			map_walls[17] = wall_create(50, 50, 2150,400);
			wall_count++;
			get_sprite_wall(map_walls[17], "plataforma1.png");

			map_walls[18] = wall_create(100, 100, 2500, 425);
			wall_count++;
			get_sprite_wall(map_walls[18], "plataforma1.png");

			map_walls[19] = wall_create(50, 50, 1210, 875);
			wall_count++;
			get_sprite_wall(map_walls[19], "plataforma1.png");

			map_walls[20] = wall_create(50, 50, 1475, 675);
			wall_count++;
			get_sprite_wall(map_walls[20], "plataforma1.png");

			map_walls[21] = wall_create(50, 50, 1750, 875);
			wall_count++;
			get_sprite_wall(map_walls[21], "plataforma1.png");
			
			map_walls[22] = wall_create(120, 40, 1335, 770);
			wall_count++;
			map_walls[22]->type = WALL_VANISHING; 
			get_sprite_wall(map_walls[22], "plataformaquebravel.png");

			map_walls[23] = wall_create(550, 50, 2075, 1275);
			wall_count++;

			map_walls[24] = wall_create(100, 100, 3150, 450);
			wall_count++;
			get_sprite_wall(map_walls[24], "plataforma1.png");

			map_walls[25] = wall_create(32, 100, 3216, 350);
			wall_count++;
			get_sprite_wall(map_walls[25], "plataforma2.png");

			map_walls[26] = wall_create(50, 50, 3375, 300);
			wall_count++;
			get_sprite_wall(map_walls[26], "plataforma1.png");

			map_walls[27] = wall_create(50, 50, 3350, 475);
			wall_count++;
			get_sprite_wall(map_walls[27], "plataforma1.png");

			map_walls[28] = wall_create(50, 50, 3525, 400);
			wall_count++;
			get_sprite_wall(map_walls[28], "plataforma1.png");

			map_walls[29] = wall_create(100, 50, 2450, 1275);
			wall_count++;

			map_walls[30] = wall_create(1300, 50, 3250, 1275);
			wall_count++;

			map_walls[31] = wall_create(225, 50, 4087, 1275);
			wall_count++;

			map_walls[32] = wall_create(400, 50, 4600, 1275);
			wall_count++;

			map_walls[33] = wall_create(130, 40, 1640, 770);
			wall_count++;
			map_walls[33]->type = WALL_VANISHING; 
			get_sprite_wall(map_walls[33], "plataformaquebravel.png");

			map_walls[34] = wall_create(130, 40, 2340, 450);
			wall_count++;
			map_walls[34]->type = WALL_VANISHING; 
			get_sprite_wall(map_walls[34], "plataformaquebravel.png");

			map_walls[35] = wall_create(130, 40, 2660, 450);
			wall_count++;
			map_walls[35]->type = WALL_VANISHING; 
			get_sprite_wall(map_walls[35], "plataformaquebravel.png");

			map_walls[36] = wall_create(130, 40, 2860, 450);
			wall_count++;
			map_walls[36]->type = WALL_VANISHING; 
			get_sprite_wall(map_walls[36], "plataformaquebravel.png");

			map_walls[37] = wall_create(90, 40, 3000, 450);
			wall_count++;
			map_walls[37]->type = WALL_VANISHING; 
			get_sprite_wall(map_walls[37], "plataformaquebravel.png");

			map_walls[38] = wall_create(130, 40, 3690, 500);
			wall_count++;
			map_walls[38]->type = WALL_VANISHING; 
			get_sprite_wall(map_walls[38], "plataformaquebravel.png");

			map_walls[39] = wall_create(130, 40, 3840, 450);
			wall_count++;
			map_walls[39]->type = WALL_VANISHING; 
			get_sprite_wall(map_walls[39], "plataformaquebravel.png");

			map_walls[40] = wall_create(130, 40, 3990, 500);
			wall_count++;
			map_walls[40]->type = WALL_VANISHING; 
			get_sprite_wall(map_walls[40], "plataformaquebravel.png");

			map_walls[41] = wall_create(130, 40, 3990, 370);
			wall_count++;
			map_walls[41]->type = WALL_VANISHING; 
			get_sprite_wall(map_walls[41], "plataformaquebravel.png");
			
			map_walls[42] = wall_create(130, 40, 4165, 450);
			wall_count++;
			map_walls[42]->type = WALL_VANISHING; 
			get_sprite_wall(map_walls[42], "plataformaquebravel.png");

			map_walls[43] = wall_create(130, 40, 4340, 500);
			wall_count++;
			map_walls[43]->type = WALL_VANISHING; 
			get_sprite_wall(map_walls[43], "plataformaquebravel.png");

			map_walls[44] = wall_create(130, 40, 4565, 525);
			wall_count++;
			map_walls[44]->type = WALL_VANISHING; 
			get_sprite_wall(map_walls[44], "plataformaquebravel.png");

			map_walls[45] = wall_create(130, 40, 4565, 425);
			wall_count++;
			map_walls[45]->type = WALL_VANISHING; 
			get_sprite_wall(map_walls[45], "plataformaquebravel.png");

			map_walls[46] = wall_create(130, 40, 4565, 325);
			wall_count++;
			map_walls[46]->type = WALL_VANISHING; 
			get_sprite_wall(map_walls[46], "plataformaquebravel.png");


			map_walls[47] = wall_create(100, 100, 4750, 450);
			wall_count++;
			get_sprite_wall(map_walls[47], "plataforma1.png");

			map_walls[48] = wall_create(100, 300, 1795, 1100);
			wall_count++;


			for(int i = 2; i<= 5; i++){
				if(map_walls[i]){
					get_sprite_wall(map_walls[i], "plataforma1.png");
				}
			}
			

			int map_width = al_get_bitmap_width(background); // Pega a largura real do mapa
			int map_height = al_get_bitmap_height(background); // Pega a altura real do mapa
			
			float camera_x = 0; // Inicializa a posição da câmera
			float camera_y = 0; // Inicializa a posição da câmera


			typedef struct { float x, y, w, h; unsigned char triggered; } SpawnTrigger;
			SpawnTrigger enemy_spawn = { 0.0f, 930.0f, 50.0f, 100.0f, 0 }; 
			square* enemy = NULL; 

			SpawnTrigger victory_trigger = { 4775.0f, 1250.0f, 100.0f, 100.0f, 0 }; 
			SpawnTrigger victory_trigger2 = { 4775.0f, 350.0f, 100.0f, 100.0f, 0 }; 

		while(1){																																															
			
			al_wait_for_event(queue, &event);																																								
				
			float px = player_1->x;
			float py = player_1->y;
			
			// verifica se jogador entrou na área de vitória
			if ((px + player_1->width/2.0f > victory_trigger.x - victory_trigger.w/2.0f &&
				px - player_1->width/2.0f < victory_trigger.x + victory_trigger.w/2.0f &&
				py + player_1->heigth/2.0f > victory_trigger.y - victory_trigger.h/2.0f &&
				py - player_1->heigth/2.0f < victory_trigger.y + victory_trigger.h/2.0f) ||
				(px + player_1->width/2.0f > victory_trigger2.x - victory_trigger2.w/2.0f &&
				px - player_1->width/2.0f < victory_trigger2.x + victory_trigger2.w/2.0f &&
				py + player_1->heigth/2.0f > victory_trigger2.y - victory_trigger2.h/2.0f &&
				py - player_1->heigth/2.0f < victory_trigger2.y + victory_trigger2.h/2.0f)) {
				
					
				al_draw_bitmap(vitory_screen, 0, 0, 0);
				al_draw_text(font, al_map_rgb(0, 255, 0), X_SCREEN/2 - 100, Y_SCREEN/2 - 15, 0, "VOCÊ VENCEU!");
				al_draw_text(font, al_map_rgb(255, 255, 255), X_SCREEN/2 - 110, Y_SCREEN/2 + 15, 0, "Pressione ESPAÇO para sair");
				al_flip_display();
				
				// Espera por ESPAÇO
				while(1) {
						al_wait_for_event(queue, &event);
						if ((event.type == 10) && (event.keyboard.keycode == 75)) {
								// Limpar recursos e retornar
								al_destroy_bitmap(background);
								al_destroy_bitmap(vitory_screen);
								al_destroy_bitmap(heart_container);
								al_destroy_font(font);
								al_destroy_display(disp);
								al_destroy_timer(timer);
								al_destroy_event_queue(queue);
								for(int i = 0; i < pit_count; i++){
										if (map_pits[i]) pit_destroy(map_pits[i]);
								}
								for(int i = 0; i < obstacle_count; i++){
										if (map_obstacles[i]) obstacle_destroy(map_obstacles[i]);
								}
								for(int i = 0; i < wall_count; i++){
										if (map_walls[i]) wall_destroy(map_walls[i]);
								}
								square_destroy(player_1);
								if (enemy) {
										if (enemy->sprite) al_destroy_bitmap(enemy->sprite);
										free(enemy);
								}
								return 0;
						}
						else if (event.type == 42){ 
							al_destroy_bitmap(background);
							al_destroy_bitmap(vitory_screen);
							al_destroy_font(font);
							al_destroy_display(disp);
							al_destroy_timer(timer);
							al_destroy_event_queue(queue);
							for(int i = 0; i < pit_count; i++){
									if (map_pits[i]) pit_destroy(map_pits[i]);
							}
							for(int i = 0; i < obstacle_count; i++){
									if (map_obstacles[i]) obstacle_destroy(map_obstacles[i]);
							}
							for(int i = 0; i < wall_count; i++){
									if (map_walls[i]) wall_destroy(map_walls[i]);
							}
							square_destroy(player_1);
							if (enemy) {
								if (enemy->sprite) al_destroy_bitmap(enemy->sprite);
								free(enemy);
							}	
							
							return 0;  // Finaliza o jogo
						}
					}		
			}

			
			
			if (p1k){																																																																																
					al_draw_bitmap(vitory_screen, 0, 0, 0);	
					if (p1k) al_draw_text(font, al_map_rgb(255, 255, 255), X_SCREEN/2 - 50, Y_SCREEN/2-15, 0, "VOCE PERDEU!");																					
					al_draw_text(font, al_map_rgb(255, 255, 255), X_SCREEN/2 - 110, Y_SCREEN/2+5, 0, "PRESSIONE ESPAÇO PARA SAIR");																				
					al_flip_display();																																												

					if ((event.type == 10) && (event.keyboard.keycode == 75)) break;																																
					else if (event.type == 42) break; 																																								
				}
				else{																																																
					if (event.type == 30){																																									
						
						camera_x = (player_1->x - X_SCREEN / 2);
						camera_y = (player_1->y - Y_SCREEN / 2) - 150 ;

						// Limite Esquerdo e Superior
						if (camera_x < 0) camera_x = 0;
						if (camera_y < 0) camera_y = 0;

						// Limite Direito e Inferior
						if (camera_x > map_width - X_SCREEN) camera_x = map_width - X_SCREEN;
						if (camera_y > map_height - Y_SCREEN) camera_y = map_height - Y_SCREEN;
										
						al_draw_bitmap_region(
											background, 
											camera_x,   // X de onde começar a cortar o mapa
											camera_y,   // Y de onde começar a cortar o mapa
											X_SCREEN,   // Largura da tela (o quanto cortar)
											Y_SCREEN,   // Altura da tela (o quanto cortar)
											0,          // X de destino na tela (sempre 0, desenha no canto superior esquerdo da tela)
											0,          // Y de destino na tela (sempre 0)
											0           
										);
															
						
						// COLISÃO COM OBSTÁCULOS
						for(int i = 0; i < obstacle_count; i++){
						// Verifica se colidiu especificamente com o obstáculo 'i'
							if (map_obstacles[i] != NULL && check_collision_obstacle(player_1, map_obstacles[i])) {
								
								// Verifica invencibilidade (para não morrer instantaneamente)
								if (player_1->invincibility_timer == 0) {
									printf("Colidiu com obstáculo %d\n", i);
									
									apply_knockback(player_1, map_obstacles[i]);
									player_1->hp = player_1->hp - map_obstacles[i]->damage;
									
									// Ativa invencibilidade por ~2 segundos (60 frames)
									player_1->invincibility_timer = 60; 
								}
							}
						}

						for(int i = 0; i < obstacle_count; i++){
							draw_obstacle(map_obstacles[i], camera_x, camera_y);
						}


						for(int i = 1; i < wall_count; i++){
							if((i != 23) && (i != 29) && (i !=30) && (i != 31) && (i != 32) && (i != 48)){ // Pula as paredes do chão
								wall_draw(map_walls[i], camera_x, camera_y);
							}
						}

						for(int i = 0; i < pit_count; i++){
								if (map_pits[i] != NULL && check_collision_pit(player_1->x, player_1->y, player_1->width, player_1->heigth, map_pits[i])) {
										
										if (player_1->invincibility_timer == 0) {
												printf("Player caiu em buraco %d!\n", i);
												apply_pit_effect(player_1, map_pits[i]);
												player_1->invincibility_timer = 60; // Invencibilidade após cair
										}
								}
						}

						
						if (!enemy && !enemy_spawn.triggered) {
							float px = player_1->x;
							float py = player_1->y;
							// verifica se jogador entrou na área de trigger
							if (px + player_1->width/2.0f > enemy_spawn.x - enemy_spawn.w/2.0f &&
								px - player_1->width/2.0f < enemy_spawn.x + enemy_spawn.w/2.0f &&
								py + player_1->heigth/2.0f > enemy_spawn.y - enemy_spawn.h/2.0f &&
								py - player_1->heigth/2.0f < enemy_spawn.y + enemy_spawn.h/2.0f) {
							// spawn do inimigo
								enemy = square_create(40, 40, 0, enemy_spawn.x+500, enemy_spawn.y - 30.0f, X_BACKGROUND, Y_BACKGROUND);
								if (enemy) {
									enemy->sprite = al_load_bitmap("enemy.png");
									if (!enemy->sprite) {
										fprintf(stderr, "Aviso: Não foi possível carregar enemy.png. Usando desenho padrão.\n");
									}
								}
								enemy_spawn.triggered = 1; 
							}
						}



						int debug_mode = 0;
						if(event.type == 10 && event.keyboard.keycode == ALLEGRO_KEY_H){
							debug_mode = !debug_mode;
						}

						if (debug_mode) {
						// Desenha Hitboxdo Corpo em VERDE
						al_draw_rectangle(
							(player_1->x + player_1->body_box.offset_x) - player_1->body_box.width/2,
							(player_1->y + player_1->body_box.offset_y) - player_1->body_box.height/2,
							(player_1->x + player_1->body_box.offset_x) + player_1->body_box.width/2,
							(player_1->y + player_1->body_box.offset_y) + player_1->body_box.height/2,
							al_map_rgb(0, 255, 0), 1
						);
						}

						
						update_life(player_1, font, heart_container);
						update_position(player_1, map_walls, MAX_WALLS);            			
						update_physics(player_1, map_walls, MAX_WALLS);
						
						update_vanishing_platforms(player_1, map_walls, MAX_WALLS);
						if (player_1->invincibility_timer > 0) {
							player_1->invincibility_timer--;
						}

					p1k = check_kill(player_1);																																						

					if(enemy && update_enemy(enemy, player_1, font)){
						
						// Calcula posição em tela do inimigo
						float enemy_screen_x = enemy->x - camera_x;
						float enemy_screen_y = enemy->y - camera_y;
						
						// Desenha sprite PNG se existir, senão usa retângulo
						if (enemy->sprite) {
							al_draw_scaled_bitmap(enemy->sprite, 0, 0, 
												al_get_bitmap_width(enemy->sprite), 
												al_get_bitmap_height(enemy->sprite),
												enemy_screen_x - enemy->width/2, 
												enemy_screen_y - enemy->heigth/2, 
												enemy->width, 
												enemy->heigth, 0);
						} else {
							al_draw_rectangle(enemy_screen_x - enemy->width/2, enemy_screen_y - enemy->heigth/2, 
											  enemy_screen_x + enemy->width/2, enemy_screen_y + enemy->heigth/2, 
											  al_map_rgb(0, 255, 0), 3);
						}
					}
					
					float player_screen_x = player_1->x - camera_x;
					float player_screen_y = player_1->y - camera_y;

					update_animation(player_1);

						/*
						al_draw_filled_rectangle(
							player_screen_x - player_1->width/2, 
							player_screen_y - player_1->heigth/2, 
							player_screen_x + player_1->width/2, 
							player_screen_y + player_1->heigth/2, 
							al_map_rgb(255, 0, 0)
						);
						*/

					draw_player_animated(player_1, camera_x, camera_y);
						
					al_flip_display();																												
					}
					else if ((event.type == 10) || (event.type == 12)){																																				
						if (event.keyboard.keycode == 1) joystick_left(player_1->control);																												
						else if (event.keyboard.keycode == 4) joystick_right(player_1->control);																													
						else if (event.keyboard.keycode == 23) joystick_up(player_1->control);																													
						else if (event.keyboard.keycode == 19) joystick_down(player_1->control);																												
						if (event.keyboard.keycode == 217) joystick_ctr(player_1->control);
					}																																			
					else if (event.type == 42) break;																										
				}
			}
			
			al_destroy_bitmap(background);
			al_destroy_bitmap(vitory_screen);
			al_destroy_font(font);
			al_destroy_display(disp);
			al_destroy_timer(timer);
			al_destroy_event_queue(queue);
			for(int i = 0; i < pit_count; i++){
 			   if (map_pits[i]) pit_destroy(map_pits[i]);
			}
			for(int i = 0; i < obstacle_count; i++){
			   if (map_obstacles[i]) obstacle_destroy(map_obstacles[i]);
			}
			for(int i = 0; i < wall_count; i++){
			   if (map_walls[i]) wall_destroy(map_walls[i]);
			}
			square_destroy(player_1);
			if (enemy) {
				if (enemy->sprite) al_destroy_bitmap(enemy->sprite);
				free(enemy);
			}

			return 0;
		}
		case 2:
		{
			while(1){
				printf("iniciando controles\n");

				ALLEGRO_EVENT event;																																									
				al_wait_for_event(queue, &event);														

				ALLEGRO_BITMAP* control_image = NULL;
				control_image = al_load_bitmap("controles.png");
				al_draw_bitmap(control_image, 0, 0, 0);
				if (!control_image) {
					fprintf(stderr, "Failed to load control image!\n");
					return -1;
				}
				if(event.type == 10 && event.keyboard.keycode == ALLEGRO_KEY_SPACE){
					option = 0;
					printf("%d\n", option);
					break;
				}
				else if (event.type == 42) return 0; 																																						
				
				al_flip_display();
			}
			break;
		}
		case 3:
		{
			printf("saindo do jogo\n");
			return 0;

		}	
	return 0;
	}
	}
}

