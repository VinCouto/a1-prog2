#ifndef __SQUARE__
#define __SQUARE__

#include "Joystick.h"
#include <allegro5/allegro5.h>

typedef struct wall wall;

#define SQUARE_STEP 10
#define GRAVIDADE 0.8f
#define FORCA_PULO -15.0f
#define CHAO_Y 500

#define ANIM_IDLE 0
#define ANIM_WALK 1
#define ANIM_CROUCH 2
#define ANIM_CROUCH_WALK 3
#define ANIM_JUMP 4

typedef struct {
    short offset_x;
    short offset_y;
    unsigned short width;
    unsigned short height;
    unsigned char active;
} Hitbox;

typedef struct {
    int heigth;
    int width;
    unsigned char face;
    unsigned char hp;
    float x; 
    float y; 
    unsigned short idle;
    float vy;
    Hitbox body_box;
    Hitbox attack_box;
    joystick *control;
    ALLEGRO_BITMAP *sprite;  
    int draw_width;
    int draw_height;
    int anim_frame;        
    int anim_counter;      
    int current_anim;      
    int invincibility_timer; 
    float original_height; 

} square;

//cria um novo quadrado
square* square_create(int heigth, int width, unsigned char face, float x, float y, float max_x, float max_y);

//atualiza a animação do quadrado
void update_animation(square *player);

//desenha o quadrado com animação
void draw_player_animated(square *player, float camera_x, float camera_y);

//move o quadrado
void square_move(square *element, char steps, unsigned char trajectory, float max_x, float max_y);

//destroi o quadrado
void square_destroy(square *element);

#endif
