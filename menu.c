#include "menu.h"



int menu(ALLEGRO_DISPLAY* disp, unsigned int X_SCREEN, unsigned int Y_SCREEN){

  printf("to no menu\n");

  int state_menu = 0;
  
  ALLEGRO_BITMAP *menu_image = NULL;
  menu_image = al_load_bitmap("menu.png");
  al_draw_bitmap(menu_image, 0, 0, 0);
  al_flip_display();
  if(!menu_image){
    fprintf(stderr, "Failed to load menu image!\n");
    return 0;
  }
  

  if(!disp){
    fprintf(stderr, "Failed to create display!\n");
    al_destroy_bitmap(menu_image);
    return 0;
  }
  
  
  ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
  if(!queue){
    fprintf(stderr, "Failed to create event queue!\n");
    al_destroy_bitmap(menu_image); 
    return 0;
  }
  
  
  al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_display_event_source(disp));
  
  
  ALLEGRO_EVENT event;
  
  while(1){
    
    al_wait_for_event(queue, &event);

    al_draw_bitmap(menu_image, 0, 0, 0);


    
    
    if(event.type == 10 && event.keyboard.keycode == 85){
      state_menu = state_menu + 1;
    }

    if(event.type == 10 && event.keyboard.keycode == 84){
      state_menu = state_menu - 1;
    }
  
    printf("state_menu: %d\n", state_menu);
    
    if(state_menu == 1){
      //al_draw_text(al_create_builtin_font(), al_map_rgb(255, 0, 0), X_SCREEN / 2, Y_SCREEN - 100, ALLEGRO_ALIGN_CENTRE, "PRESS ENTER TO START");
      al_draw_rectangle(20, 320, 350, 380, al_map_rgb(255, 0, 0),5);
      al_flip_display();
      al_draw_rectangle(20, 320, 350, 380, al_map_rgb(255, 117, 20),5);
      al_flip_display();
    }


    if(state_menu == 2){
      //al_draw_text(al_create_builtin_font(), al_map_rgb(255, 0, 0), X_SCREEN / 2, Y_SCREEN - 50, ALLEGRO_ALIGN_CENTRE, "PRESS ESCAPE TO EXIT");
      al_draw_rectangle(20, 420, 350, 480, al_map_rgb(255, 0, 0),5);
      al_flip_display();
      al_draw_rectangle(20, 420, 350, 480, al_map_rgb(255, 117, 20),5);
      al_flip_display();
    }

    if(state_menu == 3){
      al_draw_rectangle(20, 550, 350, 620, al_map_rgb(255, 0, 0),5);
      al_flip_display();
      al_draw_rectangle(20, 550, 350, 620, al_map_rgb(255, 0, 0),5);
      al_flip_display();
    }

    if(state_menu > 3){
      state_menu = 1;
    }
    
    if(state_menu < 1){
      state_menu = 3;
    }


    if(event.type == 10 && (event.keyboard.keycode == ALLEGRO_KEY_ENTER || event.keyboard.keycode == ALLEGRO_KEY_PAD_ENTER)){ //ALLEGRO_EVENT_KEY_DOWN 59 é o código para ALLEGRO_KEY_ENTER 
        printf("APERTEI ENTER\n"); 
        // 1 é o código para ALLEGRO_KEY_ESCAPE
        return state_menu;
     }else if (event.keyboard.keycode == 59){
      printf("APERTEI ESCAPE\n");
      break;
     }else if (event.type == 42) { // ALLEGRO_EVENT_DISPLAY_CLOSE
        break;
    }
    
    al_flip_display();

  } 

  al_destroy_bitmap(menu_image);
  al_destroy_event_queue(queue);


  return state_menu;
}
  

