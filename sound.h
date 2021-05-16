#include <SDL.h>
#include <SDL_image.h>
// #include <SDL_mixer.h>
#include<SDL2/SDL_mixer.h>
#include <stdio.h>
// #include <string>

//The music that will be played
Mix_Music *gMusic;

//The sound effects that will be used
Mix_Chunk *gCar;

int init();
int loadMedia();
int loadMedia2();
void close_sound();
void start(SDL_Event e);
