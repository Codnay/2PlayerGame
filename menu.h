#ifndef MENU_H
#define MENU_H

#include <SDL.h>
#include <SDL_ttf.h>
#include <unistd.h>
#include "font.h"
#include "constans.h"

SDL_Surface *background;
SDL_Texture *background_texture;
TTF_Font *title_deathrace;

void server_or_client(SDL_Renderer *renderer, char *menu, TTF_Font *font);
void ask_for_ip(SDL_Renderer *renderer, TTF_Font *font, char *ip);
void how_to_play(SDL_Renderer *renderer, char *menu, TTF_Font *font);
void fix_or_random(SDL_Renderer *renderer, char *menu, TTF_Font *font);

#endif
