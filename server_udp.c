#include "server_udp.h"
#include "network.h"
#include "objects.h"
#include "list.h"
#include "physic.h"
#include "constans.h"
#include "time.h"
#include "sys/time.h"

struct sockaddr_in clients_addresses[MAX_PLAYERS];
struct Player players_server[MAX_PLAYERS];
struct node *bullets_server = NULL;
struct node *powerup_list= NULL;
int number_of_connected_clients = 0;

void init_powerups(){
    for(int i=MAX_POWERUP-1; i>=0; i--){
        struct PowerUp temp;

        temp.position.x = players[0].powerup_pos_arrx[i];
        temp.position.y = players[0].powerup_pos_arry[i];
        //printf("wow %d\n", temp.position.x);
        //printf("wow %d\n", temp.position.y);

        temp.position.w = POWERUP_WIDTH;
        temp.position.h = POWERUP_HEIGHT;

        if(i%2==0){
            temp.powerup_type= 0;
        }

        else{
            temp.powerup_type=1;
        }

        push_element(&powerup_list, &temp,sizeof(struct PowerUp));
    }
}

void prepare_server(int *sock, struct sockaddr_in *server_sock) {
    memset(clients_addresses, 0, sizeof(struct sockaddr_in) * MAX_PLAYERS);
    if ((*sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        perror("socket failed");
    }
    if (bind(*sock, (struct sockaddr*) server_sock, sizeof(struct sockaddr)) < 0) {
        perror("bind server error");
    }
}

struct sockaddr_in receive_data(int sock, int16_t data[]) {
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(struct sockaddr);
    recvfrom(sock, data, sizeof(int16_t) * 4, 0, (struct sockaddr*)&addr, &addr_size);
    return addr;
}

void send_data(int sock, struct sockaddr_in client, int16_t data[], int size) {
    socklen_t addr_size = sizeof(struct sockaddr);
    sendto(sock, data, sizeof(int16_t) * size, 0, (struct sockaddr*)&client, addr_size);
}

void init_players_tab() {
    int i;
    for (i = 0; i < MAX_PLAYERS; i++) {
        players_server[i].position.w = PLAYER_WIDTH;
        players_server[i].position.h = PLAYER_HEIGHT;
        // players_server[i].position.x = SPAWN_A;
        // players_server[i].position.y = SPAWN_B;
        if (i == 0)
        {
            players_server[i].position.x = SPAWN_X;
            players_server[i].position.y = SPAWN_Y;
        }else{
            players_server[i].position.x = SPAWN_A;
            players_server[i].position.y = SPAWN_B;
        }
    }
}

void* server_receive_loop(void *arg) {
    int socket = *((int *) arg);
    int client_pos = 0;
    struct sockaddr_in client_addr;
    int16_t tab[4];
    init_powerups();
    init_players_tab();
    while (1) {
        client_addr = receive_data(socket, tab);
        client_pos = addr_pos_in_tab(client_addr, clients_addresses, number_of_connected_clients);
        if (its_an_old_client(client_pos)) {
            int16_t keys = tab[1];
            player_from_key_state(&players_server[client_pos], keys);
            if(players_server[client_pos].shoot && !players_server[client_pos].reloading) {
                struct Bullet temp;
                temp.position.x = players_server[client_pos].position.x;
                temp.position.y = players_server[client_pos].position.y;
                temp.position.w = BULLET_WIDTH;
                temp.position.h = BULLET_HEIGHT;
                temp.face = players_server[client_pos].face;

                //------------------here 
                if (temp.face == 1) {
                    temp.position.x += PLAYER_WIDTH;
                } else if(temp.face == -1){
                    temp.position.x -= BULLET_WIDTH;
                }else if(temp.face == 2){
                    temp.position.y += PLAYER_HEIGHT;
                }else{
                    temp.position.y -= BULLET_HEIGHT;
                }
                temp.player_id = client_pos;
                push_element(&bullets_server, &temp, sizeof(struct Bullet));
            }
            players_server[client_pos].reloading = players_server[client_pos].shoot;
        }
        if (tab[0] == -1 && client_pos < MAX_PLAYERS) {
            add_adr_to_list(client_pos, &client_addr);
            int16_t tab[3];
            tab[0] = -1;
            tab[1] = client_pos;
            send_data(socket, clients_addresses[client_pos], tab, 3);
        }
        usleep(50);
    }
}

int get_bullet_array(struct node *list, int16_t **array) {
    int n = 0;
    struct node *temp = list;
    while (temp != NULL) {
        n++;
        temp = temp->next;
    }
    *array = malloc(sizeof(int16_t) + (n * 2 * sizeof(int16_t)));
    (*array)[0] = -2;
    int i = 0; 
    temp = list;
    while (temp != NULL && i < n) {
        (*array)[1 + (i * 2)] = ((struct Bullet*) temp->data)->position.x;
        (*array)[2 + (i * 2)] = ((struct Bullet*) temp->data)->position.y;
        i++;
        temp = temp->next;
    }
    return n;
}

void* server_send_loop(void *arg) {
    int socket = *((int *) arg);
    int16_t tab[320+2*MAX_POWERUP];
    //int 
    //int tabx[MAX_POWERUP];
    //int taby[MAX_POWERUP];
    struct timeval start, stop;
    double time_interval;
    int killer;
    while (1) {
        gettimeofday(&start, NULL);
        int i, j;
        move_bullets(&bullets_server);
        for (i = 0; i < number_of_connected_clients; i++) {
            move_player(&players_server[i]);
            if (check_if_player_dies(&players_server[i], &bullets_server, &killer)) {
                if (i == 0 && players_server[0].powerup_c!=1)
                {
                    players_server[i].position.x = SPAWN_X;
                    players_server[i].position.y = SPAWN_Y;
                }
                else if(i == 1 && players_server[1].powerup_c!=1)
                {
                    players_server[i].position.x = SPAWN_A;
                    players_server[i].position.y = SPAWN_B;
                }
                // players_server[i].position.x = SPAWN_A;
                // players_server[i].position.y = SPAWN_B;
                players_server[i].deaths++;
                players_server[killer].kills++;
            }

            if(players_server[i].powerupA_start_time>0){
                players_server[i].powerupA_start_time-=1;
                //printf("This is the time: %d\n", players_server[i].powerupA_start_time);
            }

            if(players_server[i].powerupB_start_time>0){
                players_server[i].powerupB_start_time-=1;
            }

            if(players_server[i].powerupC_start_time>0){
                players_server[i].powerupC_start_time-=1;
            }

            if(players_server[i].powerupB_start_time==0){
                players_server[i].powerup_b= 0;
            }

            if(players_server[i].powerupA_start_time==0){
                players_server[i].powerup_a= 0;
            }

            if(players_server[i].powerupC_start_time==0){
                players_server[i].powerup_c= 0;
            }


        }

        for (i = 0; i < number_of_connected_clients; i++) {
//printf("This is the k value: %d\n",check_if_powerup_collect(&players_server[i], &powerup_list) );
            if (check_if_powerup_collect(&players_server[i], &powerup_list)>-1){
                //printf("This is the i value: %d\n",check_if_powerup_collect(&players[i], &powerup_list) );
                //for(j=0; j<number_of_connected_clients; j++){

                if(players[0].powerup_pos_arrx[check_if_powerup_collect(&players_server[i], &powerup_list)]!= -1){
                    //players_server[i].powerup_a++;

                    if(check_if_powerup_collect(&players_server[i], &powerup_list)== MAX_POWERUP-2){
                        if(i==1){
                            players[0].win+= 1; 
                            //players[0].powerup_pos_arrx[check_if_powerup_collect(&players_server[i], &powerup_list)]= -1;
                            
                            players_server[0].position.x = SPAWN_X;
                            players_server[0].position.y = SPAWN_Y;
                            players_server[1].position.x = SPAWN_A;
                            players_server[1].position.y = SPAWN_B;
                            
                            
                        }

                    }

                    else if(check_if_powerup_collect(&players_server[i], &powerup_list)== MAX_POWERUP-1){
                        if(i==0){
                            players[0].win+= 5;  
                            //players[0].powerup_pos_arrx[check_if_powerup_collect(&players_server[i], &powerup_list)]= -1;
                            players_server[0].position.x = SPAWN_X;
                            players_server[0].position.y = SPAWN_Y;
                            players_server[1].position.x = SPAWN_A;
                            players_server[1].position.y = SPAWN_B;
                        }
                    }

                    else if(check_if_powerup_collect(&players_server[i], &powerup_list)%3==0){

                        players_server[i].powerup_a= 1;
                        players_server[i].powerupA_start_time= POWERUPA_APPLY_TIME;

                        int temp1=0;
                        while(temp1!=1){
                            int a= rand()%20;
                            int b= rand()%15;
                            //printf("This is the enter a: %d\n", a);
                            //printf("This is the enter b: %d\n", b);
                            if(check_empty(b,a)==1){
                                players[0].powerup_pos_arrx[check_if_powerup_collect(&players_server[i], &powerup_list)]= a*48+12;
                                players[0].powerup_pos_arry[check_if_powerup_collect(&players_server[i], &powerup_list)]= b*48+12;
                                temp1= temp1+1;
                            }

                        
                        }

                        struct PowerUp temp;

                        temp.position.x = players[0].powerup_pos_arrx[check_if_powerup_collect(&players_server[i], &powerup_list)];
                        temp.position.y = players[0].powerup_pos_arry[check_if_powerup_collect(&players_server[i], &powerup_list)];

                        temp.position.w = POWERUP_WIDTH;
                        temp.position.h = POWERUP_HEIGHT;
                        push_element_powerup(&powerup_list, &temp ,sizeof(struct PowerUp), check_if_powerup_collect(&players_server[i], &powerup_list));
                        erase_element(&powerup_list, check_if_powerup_collect(&players_server[i], &powerup_list));

                    }

                    else if(check_if_powerup_collect(&players_server[i], &powerup_list)%3==1){
                        players_server[1-i].powerup_b= 1;
                        players_server[1-i].powerupB_start_time= POWERUPB_APPLY_TIME;

                        int temp1=0;
                        while(temp1!=1){
                            int a= rand()%20;
                            int b= rand()%15;
                            //printf("This is the enter a: %d\n", a);
                            //printf("This is the enter b: %d\n", b);
                            if(check_empty(b,a)==1){
                                players[0].powerup_pos_arrx[check_if_powerup_collect(&players_server[i], &powerup_list)]= a*48+12;
                                players[0].powerup_pos_arry[check_if_powerup_collect(&players_server[i], &powerup_list)]= b*48+12;
                                temp1= temp1+1;
                            }

                        
                        }

                        struct PowerUp temp;

                        temp.position.x = players[0].powerup_pos_arrx[check_if_powerup_collect(&players_server[i], &powerup_list)];
                        temp.position.y = players[0].powerup_pos_arry[check_if_powerup_collect(&players_server[i], &powerup_list)];

                        temp.position.w = POWERUP_WIDTH;
                        temp.position.h = POWERUP_HEIGHT;
                        push_element_powerup(&powerup_list, &temp ,sizeof(struct PowerUp), check_if_powerup_collect(&players_server[i], &powerup_list));
                        erase_element(&powerup_list, check_if_powerup_collect(&players_server[i], &powerup_list));
                    }

                    else if(check_if_powerup_collect(&players_server[i], &powerup_list)%3==2){
                        players_server[i].powerup_c=1;
                        players_server[i].powerupB_start_time= POWERUPC_APPLY_TIME;

                        int temp1=0;
                        while(temp1!=1){
                            int a= rand()%20;
                            int b= rand()%15;
                            //printf("This is the enter a: %d\n", a);
                            //printf("This is the enter b: %d\n", b);
                            if(check_empty(b,a)==1){
                                players[0].powerup_pos_arrx[check_if_powerup_collect(&players_server[i], &powerup_list)]= a*48+12;
                                players[0].powerup_pos_arry[check_if_powerup_collect(&players_server[i], &powerup_list)]= b*48+12;
                                temp1= temp1+1;
                            }

                        
                        }

                        struct PowerUp temp;

                        temp.position.x = players[0].powerup_pos_arrx[check_if_powerup_collect(&players_server[i], &powerup_list)];
                        temp.position.y = players[0].powerup_pos_arry[check_if_powerup_collect(&players_server[i], &powerup_list)];

                        temp.position.w = POWERUP_WIDTH;
                        temp.position.h = POWERUP_HEIGHT;
                        push_element_powerup(&powerup_list, &temp ,sizeof(struct PowerUp), check_if_powerup_collect(&players_server[i], &powerup_list));
                        erase_element(&powerup_list, check_if_powerup_collect(&players_server[i], &powerup_list));

                    }
                    
                     
                    //players[0].powerup_pos
                }
                //}
                //printf("This is the power value: %d\n", players_server[i].powerup_a);
            }

        }



        int16_t *bullet_array = NULL;
        int bullets_n = get_bullet_array(bullets_server, &bullet_array);
        for (i = 0; i < number_of_connected_clients; i++) {
            for (j = 0; j < number_of_connected_clients; j++) {
                tab[0] = j;
                tab[1] = players_server[j].position.x;
                tab[2] = players_server[j].position.y;
                tab[3] = players_server[j].kills;
                tab[4] = players_server[j].deaths;
                tab[5] = players_server[j].powerup_a;
                tab[6] = players_server[j].powerupA_start_time;
                tab[7] = players_server[j].powerup_b;
                tab[8] = players_server[j].powerupB_start_time;
                tab[9] = players[0].win;
                tab[10] = players[0].rand_num;
                tab[11] = players_server[j].powerup_c;
                tab[12] = players_server[j].powerupC_start_time;
                tab[13] = players_server[j].face;
                

                for(int k=0; k<MAX_POWERUP; k++){
                    int16_t helping= (int16_t)players[0].powerup_pos_arrx[k];
                    tab[k+14]= helping;
                }

                for(int k=0; k<MAX_POWERUP; k++){
                    int16_t helping= (int16_t)players[0].powerup_pos_arry[k];
                    tab[k+14+MAX_POWERUP]= helping;
                }

                for(int k=0; k<15; k++){
                    for(int j=0; j<20; j++){
                        int16_t helping= (int16_t)players[0].map_array[k][j];
                        tab[14+2*MAX_POWERUP+ k*20+j] = helping;
                    }
                    
                }


                // for(int i=0; i<MAX_POWERUP; i++){
                //     tabx[i]= players[0].powerup_poss_arrx[i];
                // }
                // for(int i=0; i<MAX_POWERUP; i++){
                //     taby[i]= players[0].powerup_poss_arrx[i];
                // }
                //tab[5] = players_server[j].powerup_pos_arrx;
                //tab[6] = players_server[j].powerup_pos_arry;
                send_data(socket, clients_addresses[i], tab, 320+ 2*MAX_POWERUP);
                usleep(20);
            }
            send_data(socket, clients_addresses[i], bullet_array, 1 + (bullets_n * 2));
            usleep(20);
        }
        free(bullet_array);
        gettimeofday(&stop, NULL);
        time_interval = (double) (stop.tv_usec - start.tv_usec);
        if ((double) (stop.tv_usec - start.tv_usec) > 0) {
            time_interval = (double) (stop.tv_usec - start.tv_usec);
        }
        usleep(FRAME_TIME - time_interval);

    }
}


int its_an_old_client(int client_pos) {
    return (client_pos < number_of_connected_clients && client_pos >= 0);
}

void add_adr_to_list(int client_pos, struct sockaddr_in *client_addr) {
    if (client_pos >= number_of_connected_clients) {
        clients_addresses[number_of_connected_clients++] = *client_addr;
    }
}
