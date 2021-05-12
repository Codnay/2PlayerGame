#ifndef PHYSIC_H
#define PHYSIC_H

#include <SDL.h>
#include "objects.h"
#include "list.h"

struct cell{
    int in;  //Is this cell in the maze?
    int up;  //Does the wall above this cell exist?
    int left;//Does the wall to the left of this cell exist?
    int prevx, prevy; //The coordinates of the previous cell, used for backtracking.
};
void move_player(struct Player *player); 
SDL_Texture* get_map_texture(SDL_Renderer *renderer);
void move_bullets(struct node **bullets);
int check_if_player_dies(struct Player *player, struct node **bullets, int *killer);

#endif