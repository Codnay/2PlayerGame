#include <menu.h>
#include <SDL.h>
#include <SDL_ttf.h>

SDL_Surface *background;
SDL_Texture *background_texture;
TTF_Font *title;

void server_or_client(SDL_Renderer *renderer, char *menu, TTF_Font *font){
    SDL_Event e;
    int pressed = false;
    while (!pressed) {
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_c) {
                    *menu = 'c';
                    pressed = true;
                } else if (e.key.keysym.sym == SDLK_s) {
                    *menu = 's';
                    pressed = true;
                }else if (e.key.keysym.sym == SDLK_h){
                    *menu = 'h';
                    pressed = true;
                }
            }
        }
        usleep(200);
        SDL_RenderClear(renderer);
        background = SDL_LoadBMP("resources/car.bmp");
        background_texture = SDL_CreateTextureFromSurface(renderer, background);
        SDL_FreeSurface(background);
        SDL_RenderCopy(renderer,background_texture, NULL, NULL);
        title = TTF_OpenFont("resources/m5x7.ttf", 140);
        disp_text(renderer, "DeathRace", title, 450, 40);
        disp_text(renderer, "[s]erver or [c]lient?", font, 525, 500);
        disp_text(renderer, "[h]ow to play?", font, 575, 800);
        SDL_RenderPresent(renderer);
    }
}

void how_to_play(SDL_Renderer *renderer, char *menu, TTF_Font *font){
    SDL_Event e;
    int pressed = false;
    while (!pressed) {
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_r) {
                    *menu = 'r';
                    pressed = true;
                }
            }
        }
        usleep(200);
        SDL_RenderClear(renderer);
        background = SDL_LoadBMP("resources/car.bmp");
        background_texture = SDL_CreateTextureFromSurface(renderer, background);
        SDL_FreeSurface(background);
        SDL_RenderCopy(renderer,background_texture, NULL, NULL);
        title = TTF_OpenFont("resources/m5x7.ttf", 160);
        disp_text(renderer, "DeathRace", title, 400, 40);
        disp_text(renderer, "This is a two player game.", font, 400, 200);
        disp_text(renderer, "Players have to reach each other bases, whoever", font, 220, 300);
        disp_text(renderer, "reaches other's base first wins!", font, 340, 400);
        disp_text(renderer, "Each player has three bullets, fired with Z key, on hit forces ", font, 140, 500);
        disp_text(renderer, "the opponent to respawn from the starting.", font, 300, 600);
        disp_text(renderer, "There are some POWERUPS in the path which can increase the player's speed", font, 20, 700);
        disp_text(renderer, "or freeze the opponent for a short while.", font, 300, 800);
        disp_text(renderer, "[r]eturn", font, 560, 900);
        SDL_RenderPresent(renderer);
    }
}

void ask_for_ip(SDL_Renderer *renderer, TTF_Font *font, char *ip) {
    memset(ip, ' ', 15);
    SDL_Event e;
    int position = 0;
    int ok = false;
    while (!ok) {
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                if ((e.key.keysym.sym >= SDLK_0 && e.key.keysym.sym <= SDLK_9) || e.key.keysym.sym == SDLK_PERIOD) {
                    if (position > 14) {
                        position = 14;
                    }
                    ip[position] = e.key.keysym.sym;
                    position++;
                }
                if (e.key.keysym.sym == SDLK_BACKSPACE) {
                    position--;
                    if (position < 0) {
                        position = 0;
                    }
                    ip[position] = ' ';
                }
                if (e.key.keysym.sym == SDLK_RETURN) {
                    ip[position] = 0;
                    ok = true;
                }
            }
        }
        usleep(200);
        SDL_RenderClear(renderer);
        background = SDL_LoadBMP("resources/car.bmp");
        background_texture = SDL_CreateTextureFromSurface(renderer, background);
        SDL_FreeSurface(background);
        SDL_RenderCopy(renderer,background_texture, NULL, NULL);
        title = TTF_OpenFont("resources/m5x7.ttf", 160);
        disp_text(renderer, "DeathRace", title, 400, 40);
        disp_text(renderer, "ip addres", font, 480, 400);
        disp_text(renderer, ip, font, 480, 460);
        SDL_RenderPresent(renderer);
    }
}
