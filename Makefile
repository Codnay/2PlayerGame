build: main.c objects.c	client_udp.c server_udp.c network.c physic.c list.c font.c menu.c
	gcc -o game main.c objects.c client_udp.c server_udp.c network.c physic.c list.c font.c menu.c sound.c `sdl2-config --cflags --libs` -lSDL2_ttf -lSDL2_image -lSDL2_mixer -lpthread -I. -Wall

