#include <stdlib.h>
#include <stdio.h>
#include "Joystick.h"
#include "Square.h"

joystick* joystick_create(){														//Implementação da função "joystick_create"

	joystick *element = (joystick*) malloc (sizeof(joystick));						//Aloca memória na heap para um novo controle
	if (!element) return NULL;														//Se a alocação não deu certo, retorna erro
	element->right = 0;																//Insere o estado de desligado para o botão de movimentação à esquerda
	element->left = 0;																//Insere o estado de desligado para o botão de movimentação à direita
	element->up = 0;																//Insere o estado de desligado para o botão de movimentação para cima
	element->down = 0;																//Insere o estado de desligado para o botão de movimentação para baixo
	element->ctr = 0;																//Insere o estado de desligado para o botão de disparo
	element->ctr_pressed = 0;														//NOVO: inicializa ctr_pressed como 0
	return element;																	//Retorna o novo controle
}

void joystick_destroy(joystick *element){ free(element);}							//Implementação da função "joystick_destroy"; libera a memória do elemento na heap

void joystick_left(joystick *element){ element->left = element->left ^ 1;}			//Implementação da função "joystick_left"; muda o estado do botão

void joystick_left_press(joystick *element){ element->left = 1; }

void joystick_left_release(joystick *element){ element->left = 0; }

void joystick_right(joystick *element){ element->right = element->right ^ 1;}		//Implementação da função "joystick_right"; muda o estado do botão

void joystick_right_press(joystick *element){ element->right = 1; }

void joystick_right_release(joystick *element){ element->right = 0; }

void joystick_up(joystick *element){ element->up = element->up ^ 1;}				//Implementação da função "joystick_up"; muda o estado do botão

void joystick_up_press(joystick *element){ element->up = 1; }

void joystick_up_release(joystick *element){ element->up = 0; }

void joystick_down(joystick *element){ element->down = element->down ^ 1;}			//Implementação da função "joystick_down"; muda o estado do botão

void joystick_down_press(joystick *element){ element->down = 1; }

void joystick_down_release(joystick *element){ element->down = 0; }

void joystick_ctr(joystick *element){ 							// Implementando agachar
	element->ctr = 1;
} 