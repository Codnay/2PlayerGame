#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_image.h>
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
#include <sound.h>

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
        players[i].powerA_key = SDLK_x;
        //players[i].powerup_a= 0;
        //players[i].powerupA_active= 0;
        players[i].face = 1;
        players[i].shoot = false;
        players[i].y_speed = 0;
        players[i].can_jump = false;
        players[i].reloading = false;
        players[i].kills = 0;
        players[i].deaths = 0;
        players[i].win= 0;
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
            players[id].powerupA_start_time= tab[6];
            players[id].powerup_b= tab[7];
            players[id].powerupB_start_time= tab[8];
            players[0].win= tab[9];
            players[0].rand_num= tab[10];
            players[id].powerup_c= tab[11];
            players[id].powerupC_start_time= tab[12];
            players[id].face = tab[13];


            for(int k= 0; k<MAX_POWERUP; k++){
                int helper;
                helper= (int)tab[k+14];
                players[0].powerup_pos_arrx[k]= helper;
            }

            for(int k= 0; k<MAX_POWERUP; k++){
                int helper;
                helper=  (int)tab[k+14+MAX_POWERUP];
                players[0].powerup_pos_arry[k]= helper;
            }

            for(int k=0; k<15; k++){
                for(int j=0; j<20; j++){
                    int helper;
                    helper=  (int)tab[14+2*MAX_POWERUP+ k*20+j];
                    players[0].map_array[k][j]= helper;
                }
                
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
    //int seed = atoi(argv[1]);
    struct sockaddr_in server_addr, client_addr;
    int sock_server, sock_client;
    char *server_ip_addr = NULL;


    srand((unsigned int)time(NULL));
    int seed= rand();

    char menu = 's';
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Texture *tex1_up = NULL;
    SDL_Texture *tex1_down = NULL;
    SDL_Texture *tex1_right = NULL;
    SDL_Texture *tex1_left = NULL;
    SDL_Texture *tex2_up = NULL;
    SDL_Texture *tex2_down = NULL;
    SDL_Texture *tex2_right = NULL;
    SDL_Texture *tex2_left = NULL;
    // SDL_Texture *tex2 = NULL;
    SDL_Texture *bullet = NULL;
    // SDL_Texture *powerup= NULL;
    SDL_Texture *powerA= NULL;
    SDL_Texture *powerB= NULL;
    SDL_Texture *powerC= NULL;
    SDL_Texture *flag_server= NULL;
    SDL_Texture *flag_client= NULL;
    SDL_Texture *mapper = NULL;
    TTF_Init();


    TTF_Font *font;
    font = TTF_OpenFont("resources/m5x7.ttf", 40);
    TTF_Font *font2;
    font2 = TTF_OpenFont("resources/m5x7.ttf", 32);
    init_players();

    //srand((unsigned int)time(NULL));

    // players[0].rand_num= rand();
    // for(int i=0; i<MAX_PLAYERS; i++){
    //     players[i].rand_num= players[0].rand_num;
    // }


    window = SDL_CreateWindow(
            "DeathRace",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            960,
            720,
            0);

    if (window == NULL) {
        printf("Could not create window: %s\n", SDL_GetError());
        return 1;
    }

    //int imgFlags = IMG_INIT_PNG;

    renderer = SDL_CreateRenderer(window, -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        printf("Could not create renderer: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    //seed= players[0].rand_num;

    map_modify(seed);

    for(int i=0; i<MAX_PLAYERS; i++){
        for(int k=0; k<15; k++){
            for(int j=0; j<20; j++){
                players[i].map_array[k][j]= map[k][j];
            }
            
        }
    }

    //srand((unsigned int)time(NULL));

    //srand()

    while(check<MAX_POWERUP-2){
        
        int a= rand()%20;
        int b= rand()%15;
        //printf("This is the enter a: %d\n", a);
        //printf("This is the enter b: %d\n", b);
        if(check_empty(b,a)==1){
            //printf("This is the exit a: %d\n", a);
            for (int i = 0; i < MAX_PLAYERS; i++) {
                //printf("This is the exit b: %d\n", a);
                //printf("This is the exit b: %d\n", b);
                players[i].powerup_pos_arrx[check]= a*48+12;
                players[i].powerup_pos_arry[check]= b*48+12;
                

            }
            //printf("This is the exit a: %d\n", players[0].powerup_pos_arrx[check]);
            //printf("This is the exit b: %d\n", players[0].powerup_pos_arrx[check]);
            check+=1;

        }
    }
    for (int i = 0; i < MAX_PLAYERS; i++) {
        players[i].powerup_pos_arrx[MAX_POWERUP-2]= SPAWN_X;
        players[i].powerup_pos_arry[MAX_POWERUP-2]= SPAWN_Y;
        players[i].powerup_pos_arrx[MAX_POWERUP-1]= SPAWN_A;
        players[i].powerup_pos_arry[MAX_POWERUP-1]= SPAWN_B;
    }



    //srand(seed);

    tex1_up = load_texture(renderer, "resources/player1_up.bmp");
    tex1_down = load_texture(renderer, "resources/player1_down.bmp");
    tex1_right = load_texture(renderer, "resources/player1_right.bmp");
    tex1_left = load_texture(renderer, "resources/player1_left.bmp");
    tex2_up = load_texture(renderer, "resources/player2_up.bmp");
    tex2_down = load_texture(renderer, "resources/player2_down.bmp");
    tex2_right = load_texture(renderer, "resources/player2_right.bmp");
    tex2_left = load_texture(renderer, "resources/player2_left.bmp");
    bullet = load_texture(renderer, "resources/canon.bmp");
    // powerup= load_texture(renderer, "resources/bullet.bmp");
    // powerA= load_texture(renderer, "resources/powerA.bmp");
    flag_server= load_texture(renderer, "resources/flag.bmp");
    flag_client= load_texture(renderer, "resources/flag.bmp");
    // flag_client= load_texture(renderer, "resources/client.bmp");
    // SDL_Surface *FLGCLNT = IMG_Load("resources/client.png");
    // flag_client = SDL_CreateTextureFromSurface(renderer, FLGCLNT);
    // SDL_FreeSurface(FLGCLNT);

    SDL_Surface *POWERA = IMG_Load("resources/powerA.png");
    powerA = SDL_CreateTextureFromSurface(renderer, POWERA);
    SDL_FreeSurface(POWERA);

    SDL_Surface *POWERB = IMG_Load("resources/powerB.png");
    powerB = SDL_CreateTextureFromSurface(renderer, POWERB);
    SDL_FreeSurface(POWERB);

    // SDL_Surface *POWERC = IMG_Load("resources/shield.bmp");
    // powerC = SDL_CreateTextureFromSurface(renderer, POWERC);
    // SDL_FreeSurface(POWERC);
    powerC = load_texture(renderer, "resources/shield.bmp");


    init();
    loadMedia();

    int i;
    background = SDL_LoadBMP("resources/car.bmp");
    background_texture = SDL_CreateTextureFromSurface(renderer, background);
    SDL_FreeSurface(background);
    title_deathrace = TTF_OpenFont("resources/m5x7.ttf", 140);
    server_or_client(renderer, &menu, font);
    while(menu != 's' && menu != 'c'){
        if (menu == 'h') {
            // server_ip_addr = malloc(16 * sizeof(char));
            how_to_play(renderer, &menu, font);
            usleep(500);
        }else if(menu == 'r'){
            server_or_client(renderer, &menu, font);
            usleep(500);
        }
    }
    
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

    
    // int helper=0;
    //unsigned int last_time, current_time;
    int temp=0;
    
    int who_won= 0;

    while (1) {

        //printf("powerup_a: %d\n", players[0].powerup_a);
        if (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                break;
            }
            resolve_keyboard(e, &players[my_id]);
            start(e);
        }

        if(temp<10){
            for(int i=0; i<15; i++){
                for(int j=0; j<20; j++){
                    map[i][j]= players[0].map_array[i][j];
                }
            }
            mapper= get_map_texture_arr(renderer, map);
            temp= temp+1;
        }

        //current_time= SDL_GetTicks();
        //printf("This is the time stored: %d\n", players[my_id].powerupA_start_time);
        //if(current_time- players[my_id].powerupA_start_time>3000){

        //    players[my_id].powerupA_active= 0;
        //     players[my_id].powerupA_start_time= current_time;
        // }
        // if(helper<5){
        //     for(int k=0; k<15; k++){
        //         for(int j=0; j<20; j++){
        //             printf("%d ", players[0].map_array[k][j]);
        //         }
                
        //     }
        //     helper=helper+1;
        // }

        //printf("This iser value: %d\n", players[my_id].powerup_a);
        if(players[0].win%5==3){
            //THE CLIENT WON
            who_won = 1;
            break;
            // printf("Player CLIENT won \n");


        }

        if(players[0].win/5==3){
            //THE SERVER WON
            who_won = 0;
            break;
            // printf("Player SERVER won \n");
        }
        //printf("The id is: %hd", my_id);
        send_to_server(sock_client, server_addr, my_id, key_state_from_player(&players[my_id]));
        usleep(30);
        SDL_RenderClear(renderer);
        //printf("This is the powerup id: %d\n", my_id);
        // printf("This is the powerup value: %d\n", players[my_id].powerup_c);
        SDL_RenderCopy(renderer, mapper, NULL, NULL);
        //for (i = 0; i <= number_of_players; i++) {
            //if(players[i].power_c!=1)


            // if(my_id==1&& players[1].powerup_c==1){
            //     printf("Here only client sees himself ");

            //     SDL_RenderCopy(renderer, tex, NULL, &players[1].position);
            // }
            
            // else if(my_id==0 && players[0].powerup_c==1){
            //     printf("Here only player sees himself ");
            //     SDL_RenderCopy(renderer, tex, NULL, &players[0].position);
            // }

            // else{
        
            
        //}
        char host_wins[10] = {};
        char participant_wins[10] = {};
        sprintf(host_wins, "%d",players[0].win/5 );
        sprintf(participant_wins, "%d",players[0].win%5 );
        disp_text(renderer, "HOST WINS", font2, 48, 670);
        disp_text(renderer, host_wins, font2, 70, 690);
        disp_text(renderer, "PARTICIPANT WINS", font2, 750, 670);
        disp_text(renderer, participant_wins, font2, 900,690);
        for (i = 0; i <= number_of_players; i++) {
            char kills[10] = {};
            sprintf(kills, "%d", players[i].kills);
            // disp_text(renderer, kills, font, 400, 30 + i * 20);
        }

        // disp_text(renderer, "deaths", font, 460, 10);
        for (i = 0; i <= number_of_players; i++) {
            char deaths[10] = {};
            sprintf(deaths, "%d", players[i].deaths);
            // disp_text(renderer, deaths, font, 460, 30 + i * 20);
        }

        for (i = 0; i < bullets_number; i++) {
            bullet_pos.x = bullets_client[i*2];
            bullet_pos.y = bullets_client[i*2 + 1];
            SDL_RenderCopy(renderer, bullet, NULL, &bullet_pos);
        }


        for(int i=0; i< MAX_POWERUP; i++){

            
                //printf("This is wakanda %d\n", powerup_pos_arrx[i]);
                powerup_pos.x= players[0].powerup_pos_arrx[i];
                powerup_pos.y= players[0].powerup_pos_arry[i];

                if(i== MAX_POWERUP-2){
                    //SERVER WALA 
                    SDL_RenderCopy(renderer, flag_server, NULL, &powerup_pos);
                }

                else if(i== MAX_POWERUP-1){
                    //CLIENT
                    SDL_RenderCopy(renderer, flag_client, NULL, &powerup_pos);
                }

                else if(i%3==0){
                    //SPEEDUP
                    SDL_RenderCopy(renderer, powerA, NULL, &powerup_pos);
                }

                else if(i%3==1){
                    //SPEEDDOWN
                    SDL_RenderCopy(renderer, powerB, NULL, &powerup_pos);
                }

                else if(i%3==2){
                    //Invisibilty
                    SDL_RenderCopy(renderer, powerC, NULL, &powerup_pos);
                }


            
        }

        for(int i=0; i<=number_of_players; i++){
            if (i == 0)
            {   if(players[i].face == -1){
                //left
                    SDL_RenderCopy(renderer, tex1_left, NULL, &players[i].position);
                }else if(players[i].face == 1){
                    //right
                    SDL_RenderCopy(renderer, tex1_right, NULL, &players[i].position);
                }else if(players[i].face == -2){
                    //up
                    SDL_RenderCopy(renderer, tex1_up, NULL, &players[i].position);
                }else{
                    //down
                    SDL_RenderCopy(renderer, tex1_down, NULL, &players[i].position);
                }
                // SDL_RenderCopy(renderer, tex1, NULL, &players[i].position);
            }else{
                if(players[i].face == -1){
                    SDL_RenderCopy(renderer, tex2_left, NULL, &players[i].position);
                //left
                }else if(players[i].face == 1){
                    //right
                    SDL_RenderCopy(renderer, tex2_right, NULL, &players[i].position);
                }else if(players[i].face == -2){
                    //up
                    SDL_RenderCopy(renderer, tex2_up, NULL, &players[i].position);
                }else{
                    //down
                    SDL_RenderCopy(renderer, tex2_down, NULL, &players[i].position);
                }
                // SDL_RenderCopy(renderer, tex2, NULL, &players[i].position);
            }
            
        }

        for (i = 0; i <= number_of_players; i++) {
            char powers[10] = {};
            sprintf(powers, "%d", players[i].powerup_a);
            // disp_text(renderer, powers, font, 520, 30 + i * 20);
        }

        SDL_RenderPresent(renderer);
    }

    Mix_HaltMusic();
    loadMedia2();
    SDL_Surface *finish_bg = SDL_LoadBMP("resources/end.bmp");
    SDL_Texture *finish_texture = SDL_CreateTextureFromSurface(renderer, finish_bg);
    SDL_FreeSurface(finish_bg);
    TTF_Font *title = TTF_OpenFont("resources/m5x7.ttf", 72);
    if (who_won)
    {
        SDL_RenderClear(renderer);
        
        SDL_RenderCopy(renderer,finish_texture, NULL, NULL);
        
        disp_text(renderer, "PARTICIPANT WON!", title, 250, 310);
        SDL_RenderPresent(renderer);
    }else{
        SDL_RenderClear(renderer);
        // SDL_Surface *finish_bg = SDL_LoadBMP("resources/end.bmp");
        // SDL_Texture *finish_texture = SDL_CreateTextureFromSurface(renderer, finish_bg);
        // SDL_FreeSurface(finish_bg);
        SDL_RenderCopy(renderer,finish_texture, NULL, NULL);
        // TTF_Font title = TTF_OpenFont("resources/m5x7.ttf", 92);
        disp_text(renderer, "HOST WON!", title, 350, 310);
        SDL_RenderPresent(renderer);
    }
    usleep(5000000);

    close(sock_client);
    close(sock_server);
    pthread_cancel(thread_id_client);
    pthread_cancel(thread_id_server);
    pthread_cancel(thread_id_server_send);
    close_sound();
    
    SDL_DestroyTexture(tex1_up);
    SDL_DestroyTexture(tex1_down);
    SDL_DestroyTexture(tex1_right);
    SDL_DestroyTexture(tex1_left);
    SDL_DestroyTexture(tex2_up);
    SDL_DestroyTexture(tex2_down);
    SDL_DestroyTexture(tex2_right);
    SDL_DestroyTexture(tex2_left);
    SDL_DestroyTexture(bullet);
    // SDL_DestroyTexture(powerup);
    SDL_DestroyTexture(powerA);
    SDL_DestroyTexture(powerB);
    SDL_DestroyTexture(powerC);
    SDL_DestroyTexture(mapper);
    SDL_DestroyTexture(flag_server);
    SDL_DestroyTexture(flag_client);
    SDL_DestroyTexture(background_texture);
    SDL_DestroyTexture(finish_texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
