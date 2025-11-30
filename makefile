TARGET = AS
SOURCES = AggressiveSquares.c Square.c Joystick.c ChaoParede.c  menu.c mapa.c obstacle.c pit.c
ALLEGRO_FLAGS = $(shell pkg-config allegro-5 allegro_main-5 allegro_font-5 allegro_primitives-5 allegro_image-5 --libs --cflags)

all:
	gcc $(SOURCES) -o $(TARGET) $(ALLEGRO_FLAGS) -lm
clean:
	rm -f $(TARGET)