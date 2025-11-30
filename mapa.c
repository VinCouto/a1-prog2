#include "mapa.h"


ALLEGRO_BITMAP* map_create(){
    
    ALLEGRO_BITMAP* background = al_load_bitmap("background.png");
    
    if (!background) {
        fprintf(stderr, "Failed to load background image!\n");
        return NULL; // Retorna NULL em caso de erro
    } else {
        int w = al_get_bitmap_width(background);
        int h = al_get_bitmap_height(background);
        fprintf(stderr, "Background carregado: Dimensões %dx%d\n", w, h);
        return background; // Retorna o bitmap
    }
}