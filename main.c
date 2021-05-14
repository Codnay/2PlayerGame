#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <pthread.h>
//#include <iostream>
#include <time.h>
#include <stdint.h>
#include "objects.h"
#include "client_udp.h"
#include "server_udp.h"
#include "network.h"
#include "physic.h"
#include "constans.h"
#include "font.h"
#include "menu.h"

int number_of_players = 0;
int16_t my_id = -1;
int16_t bullets_client[256];
int check= 0;
int bullets_number = 0;
int powerup_number= 0;

SDL_Texture* load_texture(SDL_Renderer *renderer, char *file) {
    SDL_Surface *bitmap = NULL;
    SDL_Texture *texture = NULL;
    bitmap = SDL_LoadBMP(file);
    texture = SDL_CreateTextureFromSurface(renderer, bitmap);
    SDL_FreeSurface(bitmap);
    return texture;
}

void init_players() {
    int i;
    for (i = 0; i < MAX_PLAYERS; i++) {
        if (i == 0)
        {
            players[i].position.x = SPAWN_X;
            players[i].position.y = SPAWN_Y;
        }else{
            players[i].position.x = SPAWN_A;
            players[i].position.y = SPAWN_B;
        }
        // players[i].position.x = SPAWN_A;
        // players[i].position.y = SPAWN_B;
        players[i].position.w = PLAYER_WIDTH;
        players[i].position.h = PLAYER_HEIGHT;
        players[i].left_key = SDLK_LEFT;
        players[i].right_key = SDLK_RIGHT;
        players[i].up_key = SDLK_UP;
        players[i].down_key = SDLK_DOWN;
        players[i].attack_key = SDLK_z;
        //players[i].powerup_a= 0;
        players[i].face = 1;
        players[i].shoot = false;
        players[i].y_speed = 0;
        players[i].can_jump = false;
        players[i].reloading = false;
        players[i].kills = 0;
        players[i].deaths = 0;
        players[i].powerup_a= 0;
    }
}

void receive_new_id(int id) {
    my_id = id;
    number_of_players = id;
    printf("my_id is now: %d\n", my_id);
}

void check_if_its_new_player(int id){
    if (id > number_of_players) {
        number_of_players = id;
        printf("new max player, now %d\n", number_of_players + 1);
    }
}


void* client_loop(void *arg) {
    int socket = *((int *) arg);
    int16_t tab[BUF_MAX];
    //int tabx[MAX_POWERUP];
    //int taby[MAX_POWERUP];
    int length;
    int id, bullets_in_array;
    while (1) {
        length = client_listen(socket, tab);
        id = tab[0];
        if (id == -1) {
            receive_new_id(tab[1]);
        }
        if (id >= 0) {
            check_if_its_new_player(id);
            players[id].position.x = tab[1];
            players[id].position.y = tab[2];
            players[id].kills = tab[3];
            players[id].deaths = tab[4];
            players[id].powerup_a= tab[5];

            for(int k= 0; k<MAX_POWERUP; k++){
                int helper;
                helper= (int)tab[k+6];
                players[0].powerup_pos_arrx[k]= helper;
            }

            for(int k= 0; k<MAX_POWERUP; k++){
                int helper;
                helper=  (int)tab[k+6+MAX_POWERUP];
                players[0].powerup_pos_arry[k]= helper;
            }

            // for(int i=0; i< MAX_POWERUP; i++){
            //     players[0].powerup_pos_arrx[i]= tabx[i];
            //     players[0].powerup_pos_arry[i]= taby[i];
            // }
            
            //players[id].powerup_pos_arrx= tab[5];
            //players[id].powerup_pos_arry= tab[6];


        }
        if (id == -2) {
            bullets_in_array = (length - sizeof(int16_t)) / (sizeof(int16_t) * 2);
            memcpy(bullets_client, tab + 1, sizeof(int16_t) * 2 * bullets_in_array);
            //memcpy()
            bullets_number = bullets_in_array;
        }
        usleep(50);
    }
}

int main(int argc, char** argv){
    int seed = atoi(argv[1]);
    struct sockaddr_in server_addr, client_addr;
    int sock_server, sock_client;
    char *server_ip_addr = NULL;

    char menu = 's';
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Texture *tex = NULL;
    SDL_Texture *bullet = NULL;
    SDL_Texture *powerup= NULL;
    SDL_Texture *mapper = NULL;
    TTF_Init();


    TTF_Font *font;
    font = TTF_OpenFont("resources/m5x7.ttf", 24);
    init_players();



    window = SDL_CreateWindow(
            "game",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            640,
            480,
            0);

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        printf("Could not create renderer: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    mapper = get_map_texture(renderer, seed);

    //srand((unsigned int)time(NULL));

    while(check<MAX_POWERUP){
        
        int a= rand()%20;
        int b= rand()%15;
        //printf("This is the enter a: %d\n", a);
        //printf("This is the enter b: %d\n", b);
        if(check_empty(b,a)==1){
            //printf("This is the exit a: %d\n", a);
            for (int i = 0; i < MAX_PLAYERS; i++) {
                //printf("This is the exit b: %d\n", a);
                //printf("This is the exit b: %d\n", b);
                players[i].powerup_pos_arrx[check]= a*32+11;
                players[i].powerup_pos_arry[check]= b*32+11;
                

            }
            //printf("This is the exit a: %d\n", players[0].powerup_pos_arrx[check]);
            //printf("This is the exit b: %d\n", players[0].powerup_pos_arrx[check]);
            check+=1;

        }
    }

    srand(seed);

    tex = load_texture(renderer, "resources/player.bmp");
    bullet = load_texture(renderer, "resources/bullet.bmp");
    powerup= load_texture(renderer, "resources/bullet.bmp");
    int i;
    server_or_client(renderer, &menu, font);
    if (menu == 'c') {
        server_ip_addr = malloc(16 * sizeof(char));
        ask_for_ip(renderer, font, server_ip_addr);
    }
    pthread_t thread_id_server, thread_id_client, thread_id_server_send;
    server_addr = server_sock_addr(server_ip_addr);
    client_addr = client_sock_addr();
    if(menu == 's') {
        prepare_server(&sock_server, &server_addr);
        pthread_create(&thread_id_server, NULL, server_receive_loop, &sock_server);
        pthread_create(&thread_id_server_send, NULL, server_send_loop, &sock_server);
    }
    prepare_client(&sock_client, &client_addr);
    pthread_create(&thread_id_client, NULL, client_loop, &sock_client);

    while (my_id < 0) {
        send_to_server(sock_client, server_addr, my_id, 0);
        usleep(100);
    }

    SDL_Rect bullet_pos;
    bullet_pos.w = BULLET_HEIGHT;
    bullet_pos.h = BULLET_HEIGHT;

    SDL_Rect powerup_pos;
    powerup_pos.w= POWERUP_WIDTH;
    powerup_pos.h= POWERUP_HEIGHT;

    SDL_Event e;

    /*

    unsigned int last_time= 0;

    */


    while (1) {

        //printf("powerup_a: %d\n", players[0].powerup_a);
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                break;
            }
            resolve_keyboard(e, &players[my_id]);
        }
        send_to_server(sock_client, server_addr, my_id, key_state_from_player(&players[my_id]));
        usleep(30);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, mapper, NULL, NULL);
        for (i = 0; i <= number_of_players; i++) {
            SDL_RenderCopy(renderer, tex, NULL, &players[i].position);
        }

        disp_text(renderer, "kills", font, 400, 10);
        for (i = 0; i <= number_of_players; i++) {
            char kills[10] = {};
            sprintf(kills, "%d", players[i].kills);
            disp_text(renderer, kills, font, 400, 30 + i * 20);
        }

        disp_text(renderer, "deaths", font, 460, 10);
        for (i = 0; i <= number_of_players; i++) {
            char deaths[10] = {};
            sprintf(deaths, "%d", players[i].deaths);
            disp_text(renderer, deaths, font, 460, 30 + i * 20);
        }

        for (i = 0; i < bullets_number; i++) {
            bullet_pos.x = bullets_client[i*2];
            bullet_pos.y = bullets_client[i*2 + 1];
            SDL_RenderCopy(renderer, bullet, NULL, &bullet_pos);
        }
        
        /*
        unsigned int current_time= SDL_GetTicks();

        
        
             if((current_time- last_time)>POWERUP_SPAWNTIME){

                last_time= current_time;
                check =check+1;
                int a= (rand()%15)+1;
                powerup_pos_arr[(check-1)%MAX_POWERUP]= a;
            }
        */
        /*
             for(int i=0; i<MAX_POWERUP; i++){
                int a= rand()%15;
                int b= rand()%20;
                // for(int j=0; j<15; j++ ){
                //     for(int k=0; k<20; k++){

                //     }
                // }
                if(powerup_pos_arr[i]!=0){
                    powerup_pos.x= 23+ powerup_pos_arr[i%MAX_POWERUP] *10;
                    powerup_pos.y= 23+ powerup_pos_arr[i%MAX_POWERUP] *10;
                    SDL_RenderCopy(renderer, powerup, NULL, &powerup_pos);
                }

                //else {break;}
            }

        */

        for(int i=0; i< MAX_POWERUP; i++){

            if(players[0].powerup_pos_arrx[i]!=-1){
                //printf("This is wakanda %d\n", powerup_pos_arrx[i]);
                powerup_pos.x= players[0].powerup_pos_arrx[i];
                powerup_pos.y= players[0].powerup_pos_arry[i];
                SDL_RenderCopy(renderer, powerup, NULL, &powerup_pos);
            }
        }

        disp_text(renderer, "power", font, 520, 10);
        for (i = 0; i <= number_of_players; i++) {
            char powers[10] = {};
            sprintf(powers, "%d", players[i].powerup_a);
            disp_text(renderer, powers, font, 520, 30 + i * 20);
        }

        SDL_RenderPresent(renderer);
    }

    close(sock_client);
    close(sock_server);
    pthread_cancel(thread_id_client);
    pthread_cancel(thread_id_server);
    pthread_cancel(thread_id_server_send);
    SDL_DestroyTexture(tex);
    SDL_DestroyTexture(bullet);
    SDL_DestroyTexture(mapper);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
