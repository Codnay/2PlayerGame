#include "physic.h"
#include "constans.h"
//------------
#include <SDL_image.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

//------------
// #include "maze.c"

// int map[15][20];

// maze_generator(map);

//------------------------------------------------------------------------------------------------------------------------------

#define UP 0     //-y
#define DOWN 1   //+y
#define LEFT 2   //-x
#define RIGHT 3  //+x
#define OUTFILE "MAZE"
#define WHITE fprintf(outfile, "%c%c%c", 255,255,255)
#define BLACK fprintf(outfile, "%c%c%c", 0,0,0)
#define RED   fprintf(outfile, "%c%c%c", 0,0,255)
#define xsize 9
#define ysize 11

#define nodeadend//generate a maze without any dead ends! (consequently, many solutions to maze)
//#define prim    //enable this to generate mazes using prim's algorithm.
#define backtrack//enable this to generate mazes using depth-first search. Don't enable both.
//#define movie   //this option spams bitmaps to illustrate each step of generation.

long numin=1;     //Number of cells in the maze.
// const int xsize=15;
// const int ysize=20;


void initialize();
void generate();
// void savebmp(int xspecial, int yspecial);

struct cell MAZE[xsize][ysize];

void initialize(){
    //Initialize the maze!
    for(int x=0;x<xsize;x++){
        for(int y=0;y<ysize;y++){
            //The maze cells on the edges of the maze are "in" to provide padding. Otherwise, all maze cells are not in.
            MAZE[x][y].in   = (x==0||x==xsize-1||y==0||y==ysize-1)?1:0;
            //All maze cells have all walls existing by default, except the perimeter cells.
            MAZE[x][y].up   = (x==0||x==xsize-1||y==0)?0:1;
            MAZE[x][y].left = (x==0||y==0||y==ysize-1)?0:1;
        }
    }
    return;
}

void generate(){
    int xcur=1, ycur=1;//start growing from the corner. It could theoretically start growing from anywhere, doesn't matter.
    MAZE[xcur][ycur].in = 1;
    int whichway;
    bool success;
    do{
#ifdef movie
        savebmp(xcur,ycur);
#endif
#ifdef nodeadend
        if( MAZE[xcur][ycur-1].in&&MAZE[xcur][ycur+1].in&&
               MAZE[xcur-1][ycur].in&&MAZE[xcur+1][ycur].in ){
                   //If at a dead end, randomly destroy a wall to make it not a dead end!
            do{
                success=0;
                whichway=rand()%4;
                switch(whichway){
                case UP:
                    if(MAZE[xcur][ycur].up&&ycur!=1){
                        success=1;
                        MAZE[xcur][ycur].up=0;
                    }
                    break;
                case DOWN:
                    if(MAZE[xcur][ycur+1].up&&ycur!=ysize-2){
                        success=1;
                        MAZE[xcur][ycur+1].up=0;
                    }
                    break;
                case LEFT:
                    if(MAZE[xcur][ycur].left&&xcur!=1){
                        success=1;
                        MAZE[xcur][ycur].left=0;
                    }
                    break;
                case RIGHT:
                    if(MAZE[xcur+1][ycur].left&&xcur!=xsize-2){
                        success=1;
                        MAZE[xcur+1][ycur].left=0;
                    }
                    break;
                }
            }while(!success);
        }
#endif
#ifdef backtrack
        while( MAZE[xcur][ycur-1].in&&MAZE[xcur][ycur+1].in&&
               MAZE[xcur-1][ycur].in&&MAZE[xcur+1][ycur].in ){
                   //If all the neighbourhood cells are in, backtrack.
                int xcur2=MAZE[xcur][ycur].prevx;
                ycur=MAZE[xcur][ycur].prevy;
                xcur=xcur2;
        }
#endif
#ifdef prim
        do{
            //randomly find a cell that's in the maze
            xcur=rand()%(xsize-2)+1;
            ycur=rand()%(ysize-2)+1;
        }while(!MAZE[xcur][ycur].in ||
            MAZE[xcur][ycur-1].in&&MAZE[xcur][ycur+1].in&&
            MAZE[xcur-1][ycur].in&&MAZE[xcur+1][ycur].in);
#endif
        do{
            //Randomly grow the maze if possible.
            success=0;
            whichway=rand()%4;
            switch(whichway){
            case UP:
                if(!MAZE[xcur][ycur-1].in){
                    success=1;
                    MAZE[xcur][ycur].up=0;
                    MAZE[xcur][ycur-1].prevx=xcur;
                    MAZE[xcur][ycur-1].prevy=ycur;
                    ycur--;
                }
                break;
            case DOWN:
                if(!MAZE[xcur][ycur+1].in){
                    success=1;
                    MAZE[xcur][ycur+1].up=0;
                    MAZE[xcur][ycur+1].prevx=xcur;
                    MAZE[xcur][ycur+1].prevy=ycur;
                    ycur++;
                }
                break;
            case LEFT:
                if(!MAZE[xcur-1][ycur].in){
                    success=1;
                    MAZE[xcur][ycur].left=0;
                    MAZE[xcur-1][ycur].prevx=xcur;
                    MAZE[xcur-1][ycur].prevy=ycur;
                    xcur--;
                }
                break;
            case RIGHT:
                if(!MAZE[xcur+1][ycur].in){
                    success=1;
                    MAZE[xcur+1][ycur].left=0;
                    MAZE[xcur+1][ycur].prevx=xcur;
                    MAZE[xcur+1][ycur].prevy=ycur;
                    xcur++;
                }
                break;
            }
        }while(!success);
        MAZE[xcur][ycur].in=1;
        numin++; //Every iteration of this loop, one maze cell is added to the maze.
    }while(numin<(xsize-2)*(ysize-2));
#ifdef movie
    savebmp(xcur,ycur);
#endif
    return;
}

//------------------------------------------------------------------------------------------------------------------------------


int map[15][20];
// int map[15][20] = {
//     {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//     {0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,0,0,0,0,0},
//     {0,0,0,0,0,0,0,1,1,1,1,1,1,0,0,0,0,0,0,0},
//     {0,1,1,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,1,0},
//     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
//     {0,0,0,1,1,1,0,0,0,0,0,0,0,0,1,1,1,0,0,0},
//     {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
//     {1,1,0,0,0,1,1,1,0,0,0,0,1,1,1,0,0,0,1,1},
//     {1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1},
//     {1,1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1},
//     {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}};

int sign(int a) {
    if (a < 0) {
        return -1;
    } else if (a > 0) {
        return 1;
    } else {
        return 0;
    }
}

void decrement_abs(int *a) {
    *a -= sign(*a);
}

int check_collisions(SDL_Rect *rect) {
    if (map[rect->y/TILE_SIZE][rect->x/TILE_SIZE] ||
            map[(rect->y + rect->h)/TILE_SIZE][rect->x/TILE_SIZE] ||
            map[(rect->y)/TILE_SIZE][(rect->x + rect->w)/TILE_SIZE] ||
            map[(rect->y + rect->h)/TILE_SIZE][(rect->x + rect->w)/TILE_SIZE] ||
            rect->x <= 0 || (rect->x + rect->w >= SCREEN_WIDTH)) {
        return true;
    } else {
        return false;
    }
}


int move_and_check_collisions(SDL_Rect *position, int axis, int mov) {
    SDL_Rect temp = *position;

    if (axis == X_AXIS) {
        temp.x += sign(mov);
    }

    if (axis == Y_AXIS) {
        temp.y += sign(mov);
    }

    if (check_collisions(&temp)) {
        return 0;
    } else {
        *position = temp;
        return 1;
    }
}
 
void move_bullets(struct node **bullets) {
    struct node *next = *bullets;
    struct Bullet *b = NULL;
    int i = 0;
    while (next != NULL) {
        b = (struct Bullet*) next->data;
        //----------------------made a change here
        if(b->face == 1 || b->face == -1){
            b->position.x += PLAYER_SPEED * b->face * 1;
        }else{
            b->position.y += PLAYER_SPEED * (b->face/2) * 1;
        }
        next = next->next;
        if (check_collisions(&b->position)) {
            erase_element(bullets, i);
        } else {
            i++;
        }
    }
}

int check_if_player_dies(struct Player *player, struct node **bullets, int *killer) {
    struct node *next = *bullets;
    struct SDL_Rect b; 
    struct SDL_Rect p = player->position;
    int i = 0;
    while (next != NULL) {
        b = ((struct Bullet*) next->data)->position;
        if (p.x < (b.x + b.w) &&
                (p.x + p.w) > b.x &&
                p.y < (b.y + b.h) &&
                (p.y + p.h) > b.y) {
            *killer = ((struct Bullet*) next->data)->player_id;
            erase_element(bullets, i);
            return true;
        }
        next = next->next;
        i++;
    }
    return false;
}

int check_if_powerup_collect(struct Player *player, struct node **powerup) {
    struct node *next = *powerup;
    struct SDL_Rect b; 
    struct SDL_Rect p = player->position;
    int i = -1;
    while (next != NULL) {
        //printf("wow \n");
        i++;
        b = ((struct PowerUp*) next->data)->position;
        //printf("wow %d\n", b.x);
        //printf("wow %d\n", b.y);
        if (p.x < (b.x + b.w) &&
                (p.x + p.w) > b.x &&
                p.y < (b.y + b.h) &&
                (p.y + p.h) > b.y) {
            //*killer = ((struct Bullet*) next->data)->player_id;
            //erase_element(powerup, i);
            return i;
        }
        
        next = next->next;
    }
    return -1;
}



void move_player(struct Player *player) {
    int x_movement = 0;
    int y_movement = 0;
    if (player->left) {
        if(player->powerup_a){
            x_movement -= POWER_PLAYER_SPEED;
            player->face = -1;
        }

        else if(player->powerup_b){
            x_movement -= SLOW_PLAYER_SPEED;
            player->face = -1;
        }

        else{
            x_movement -= PLAYER_SPEED;
            player->face = -1;
        }
    }
    if (player->right) {
        if(player->powerup_a){
            x_movement += POWER_PLAYER_SPEED;
            player->face = 1;
        }

        else if(player->powerup_b){
            x_movement += SLOW_PLAYER_SPEED;
            player->face = 1;
        }
        else{
            x_movement += PLAYER_SPEED;
            player->face = 1;
        }
    }
    //-------------------------------------------------------
    if (player->up) {
        if(player->powerup_a){
            y_movement -= POWER_PLAYER_SPEED;
            player->face = -2;
        }

        else if(player->powerup_b){
            y_movement -= SLOW_PLAYER_SPEED;
            player->face = -2;
        }

        else{
            y_movement -= PLAYER_SPEED;
            player->face = -2;
        }
    }
    if (player->down) {
        if(player->powerup_a){
            y_movement += POWER_PLAYER_SPEED;
            player->face = 2;
        }
        if(player->powerup_b){
            y_movement += SLOW_PLAYER_SPEED;
            player->face = 2;
        }

        else{
            y_movement += PLAYER_SPEED;
            player->face = 2;
        }
    }

    // if(player->powerA){


    //     if(player->powerup_a>0){
    //         player->powerupA_active= 1;
    //         player->powerupA_time_elapsed+=1;
    //     }

    //     if(player->powerupA_time_elapsed>WAIT_FRAME){
    //         player->powerup_a -=1;
    //         player->powerupA_time_elapsed= 0;
    //     }

    //     if(player->powerupA_time_elapsed== WAIT_FRAME-1){
    //         player->powerupA_start_time= POWERUPA_APPLY_TIME;
    //     }


    //     //player.powerup_a-=1;
    // }

    //-------------------------------------------------------
    // if (player->up) {
    //     if (player->can_jump) {
    //         player->can_jump = false;
    //         player->y_speed = -PLAYER_JUMP_POWER;
    //     }
    // }
    
    // y_movement = player->y_speed / 3;
    // if (player->y_speed < MAX_VERTICAL_SPEED) {
    //     player->y_speed += GRAVITY;
    // }

    while (x_movement != 0 || y_movement != 0) {
        if (x_movement != 0 && move_and_check_collisions(&player->position, X_AXIS, x_movement)) {
            decrement_abs(&x_movement);
        } else {
            x_movement = 0;
        }

        if (y_movement != 0 && move_and_check_collisions(&player->position, Y_AXIS, y_movement)) {
            decrement_abs(&y_movement);
            // player->can_jump = false;
        } else {
            // if(y_movement > 0) {
            //     player->can_jump = true;
            //     player->y_speed = 0;
            // }
            // if(y_movement < 0) {
            //     player->y_speed = 0;
            // }
            y_movement = 0;
        }
    }
}

SDL_Texture* get_map_texture(SDL_Renderer *renderer, int seed) {
    SDL_Surface *bitmap = NULL;
    SDL_Texture *map_texture;
    SDL_Rect rect;
    rect.w = TILE_SIZE;
    rect.h = TILE_SIZE;

    bitmap = SDL_LoadBMP("resources/skull.bmp");
    SDL_Texture *tex = NULL;
    tex = SDL_CreateTextureFromSurface(renderer, bitmap);
    SDL_FreeSurface(bitmap);
    map_texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRenderTarget(renderer, map_texture);

    //---------------------------------------------------------

    srand(seed);
    initialize();
    generate();



    // for (int x = 1; x < xsize-1; ++x)
    // {
    //     for (int y = 1; y < ysize-1; ++y)
    //     {
    //         if (MAZE[x][y].up == 1)
    //         {
    //             map[x-1][y] = 1;
    //         }else{
    //             map[x-1][y] = 0;
    //         }
    //         if (MAZE[x][y].left == 1)
    //         {
    //             map[x][y-1] = 1;
    //         }else{
    //             map[x][y-1] = 0;
    //         }
    //     }
    // }
    int height = 19;
    int width = 15;
    for(int y = 0; y <= height - 1; y++){
        for(int x = 0; x <= width - 1; x++){
            if(x%2 == 1 && y%2 == 1){
                if(MAZE[x/2+1][y/2+1].in) map[x][y] = 0; else map[x][y] = 1;
            }else if(x%2 == 0 && y%2 == 0){
                map[x][y] = 1;
            }else if(x%2 == 0 && y%2 == 1){
                if(MAZE[x/2+1][y/2+1].left) map[x][y] = 1; else map[x][y] = 0;
            }else if(x%2 == 1 && y%2 == 0){
                if(MAZE[x/2+1][y/2+1].up) map[x][y] = 1; else map[x][y] = 0;
            }
        }
    }
    for (int i = 0; i < 15; ++i)
    {
        map[i][19] = 1;
    }
    //---------------------------------------------------------

    int i, j;
    for (i = 0; i < SCREEN_HEIGHT / TILE_SIZE; i++) {
        for (j = 0; j < SCREEN_WIDTH / TILE_SIZE; j++) {
            if (map[i][j]) {
                rect.x = TILE_SIZE * j;
                rect.y = TILE_SIZE * i;
                SDL_RenderCopy(renderer, tex, NULL, &rect);
            }
        }
    }
    SDL_SetRenderTarget(renderer, NULL);
    return map_texture;
}

int check_empty(int x_coor, int y_coor){
    if(map[x_coor][y_coor]== 1){
        return 0;
    }

    else{
        return 1;
    }
}
    

