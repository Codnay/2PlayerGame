#include <menu.h>
#include <SDL.h>
#include <SDL_ttf.h>

SDL_Surface *background;
SDL_Texture *background_texture;
TTF_Font *title_deathrace;

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
        // background = SDL_LoadBMP("resources/car.bmp");
        // background_texture = SDL_CreateTextureFromSurface(renderer, background);
        // SDL_FreeSurface(background);
        SDL_RenderCopy(renderer,background_texture, NULL, NULL);
        // title_deathrace = TTF_OpenFont("resources/m5x7.ttf", 160);
        disp_text(renderer, "DeathRace", title_deathrace, 275, 20);
        disp_text(renderer, "[s]Host a Game", font, 375, 340);
        disp_text(renderer, "OR", font, 480, 400);
        disp_text(renderer, "[c]Join a Game", font, 375, 460);
        disp_text(renderer, "[h]ow to play?", font, 375, 600);
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
        // background = SDL_LoadBMP("resources/car.bmp");
        // background_texture = SDL_CreateTextureFromSurface(renderer, background);
        // SDL_FreeSurface(background);
        SDL_RenderCopy(renderer,background_texture, NULL, NULL);
        // title = TTF_OpenFont("resources/m5x7.ttf", 160);
        disp_text(renderer, "DeathRace", title_deathrace, 275, 20);
        disp_text(renderer, "This is a two player game.", font, 300, 140);
        disp_text(renderer, "Players have to reach each other bases, whoever", font, 110, 220);
        disp_text(renderer, "reaches other's base first 3 times wins!", font, 180, 270);
        disp_text(renderer, "Each player can fire bullets, fired with Z key, on hit forces ", font, 70, 370);
        disp_text(renderer, "the opponent to respawn from the starting.", font, 150, 420);
        disp_text(renderer, "There are some POWERUPS in the path which can increase the player's", font, 5, 520);
        disp_text(renderer, "speed, make it invincible, or freeze the opponent for a short while.", font, 20, 570);
        disp_text(renderer, "[r]eturn", font, 450, 640);
        SDL_RenderPresent(renderer);
    }
}

void fix_or_random(SDL_Renderer *renderer, char *menu, TTF_Font *font){
    SDL_Event e;
    int pressed = false;
    while (!pressed) {
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_d) {
                    *menu = 'd';
                    pressed = true;
                } else if (e.key.keysym.sym == SDLK_n) {
                    *menu = 'n';
                    pressed = true;
                }
            }
        }
        usleep(200);
        SDL_RenderClear(renderer);
        // background = SDL_LoadBMP("resources/car.bmp");
        // background_texture = SDL_CreateTextureFromSurface(renderer, background);
        // SDL_FreeSurface(background);
        SDL_RenderCopy(renderer,background_texture, NULL, NULL);
        // title_deathrace = TTF_OpenFont("resources/m5x7.ttf", 160);
        disp_text(renderer, "DeathRace", title_deathrace, 275, 20);
        disp_text(renderer, "[D]oomTrack", font, 390, 340);
        disp_text(renderer, "OR", font, 450, 400);
        disp_text(renderer, "[N]ew Map", font, 390, 460);
        // disp_text(renderer, "[h]ow to play?", font, 375, 600);
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
        // background = SDL_LoadBMP("resources/car.bmp");
        // background_texture = SDL_CreateTextureFromSurface(renderer, background);
        // SDL_FreeSurface(background);
        SDL_RenderCopy(renderer,background_texture, NULL, NULL);
        // title = TTF_OpenFont("resources/m5x7.ttf", 160);
        disp_text(renderer, "DeathRace", title_deathrace, 275, 20);
        disp_text(renderer, "ip address", font, 375, 400);
        disp_text(renderer, ip, font, 375, 460);
        SDL_RenderPresent(renderer);
    }
}
