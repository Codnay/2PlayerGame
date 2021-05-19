#ifndef OBJECTS_H
#define OBJECTS_H

#include <SDL.h>
#include "constans.h"

struct Player {
    SDL_Rect position;
    SDL_Texture *texture;
    int left, right, up, down, powerA;
    int powerup_pos_arrx[MAX_POWERUP];
    int powerup_pos_arry[MAX_POWERUP];
    int map_array[15][20];
    int can_jump, y_speed;
    int face, shoot, reloading;
    int rand_num;
    //int powerupA_active;
    //int powerupB_active;

    //int powerupA_time_elapsed;
    //int powerupB_time_elapsed;

    unsigned int powerupA_start_time;
    unsigned int powerupB_start_time;
    unsigned int powerupC_start_time;
    //unsigned int 

    int left_key, right_key, up_key, down_key, attack_key, powerA_key;
    int powerup_a;
    int powerup_b;
    int powerup_c;
    int win;
    int kills, deaths;
};

struct Bullet {
    SDL_Rect position;
    int face;
    int player_id;
};

struct PowerUp {
    SDL_Rect position;
    int powerup_type;
    int player_id;  

};

struct Player players[MAX_PLAYERS];
void resolve_player_key_up(int key, struct Player* player);
void resolve_player_key_down(int key, struct Player* player);
void resolve_keyboard(SDL_Event e, struct Player* player);
void set_player_pos(struct Player* player, float x, float y);
struct Bullet init_bullet(int x, int y, int face);
struct PowerUp init_powerup(int x, int y, int taken);
#endif
