#ifndef __JOYSTICK__ 																										
#define __JOYSTICK__																										

typedef struct {																												
	unsigned char right;																											 
	unsigned char left;																												 
	unsigned char up;																												
	unsigned char down;																																																				
	unsigned char ctr;
	int ctr_pressed; 
} joystick;																																	

joystick* joystick_create();																							 
void joystick_destroy(joystick *element);														
void joystick_right(joystick *element);																			
void joystick_right_release(joystick *element);
void joystick_left(joystick *element);																			 
void joystick_left_press(joystick *element);
void joystick_left_release(joystick *element);
void joystick_up(joystick *element);																					
void joystick_up_press(joystick *element);
void joystick_up_release(joystick *element);
void joystick_down(joystick *element);																			
void joystick_down_press(joystick *element);
void joystick_down_release(joystick *element);
void joystick_ctr(joystick *element);

#endif																																