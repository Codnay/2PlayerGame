#include "objects.h"
#include "physic.h"
#include "constans.h"


void resolve_player_key_down(int key, struct Player* player) {
    if (key == player->left_key) {
        player->left = true;
    }
    if (key == player->right_key) {
        player->right = true;
    }
    if (key == player->up_key) {
        player->up = true;
    }
    if (key == player->down_key) {
        player->down = true;
    }
    if (key == player->attack_key) {
        player->shoot = player->face;
    }
    if (key == player-> powerA_key){
        player->powerA= true;
    }
}

void resolve_player_key_up(int key, struct Player* player) {
    if (key == player->left_key) {
        player->left = false;
    }
    if (key == player->right_key) {
        player->right = false;
    }
    if (key == player->up_key) {
        player->up = false;
    }
    if (key == player->down_key) {
        player->down = false;
    }
    if (key == player->attack_key) {
        player->shoot = false;
    }
    if (key == player-> powerA_key){
        player->powerA= false;
    }
}


void resolve_keyboard(SDL_Event e, struct Player* player) {
    if (e.type == SDL_KEYDOWN) {
        resolve_player_key_down(e.key.keysym.sym, player);
    } 
    if (e.type == SDL_KEYUP) {
        resolve_player_key_up(e.key.keysym.sym, player);
    }
}

void set_player_pos(struct Player* player, float x, float y) {
    player->position.x = x;
    player->position.y = y;
}

struct Bullet init_bullet(int x, int y, int face) {
    struct Bullet b;
    b.position.x = x;
    b.position.y = y;
    b.position.w = BULLET_WIDTH;
    b.position.h = BULLET_HEIGHT;

    b.face = face;
    return b;
}

struct PowerUp init_powerup(int x, int y, int powerup_type){
    struct PowerUp p;
    p.position.x= x;
    p.position.y= y;
    p.position.w= POWERUP_HEIGHT;
    p.position.h= POWERUP_WIDTH;
    p.powerup_type= powerup_type;
    return p;
}
