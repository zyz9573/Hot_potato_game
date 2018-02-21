//created by Panjoy Zhang at 2/13/2018
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
//51015-51097
#define port_low 51015
#define port_high 51097
struct player{
        int total;
	int server_socket;
	int id;
	int port;
 	char hostname[64];
 	int player_socket;
 	int left_id;
 	int left_port;
 	char left_hostname[64];
 	int right_id;
 	int right_port;
 	char right_hostname[64];
 	int change;
};

int max(int a, int b){
	if(a>b){return a;}
	return b;
}
int leftorright(int a){
	time_t t;
  	srand((unsigned) time(&t)+a);
  	int fp=rand()%2;
  	return fp;
}
char * itostr(char *str, int i)
{
    sprintf(str, "%d", i);
    return str;
}
void printplayer(struct player * temp){
	printf("server_socket: %d\n",temp->server_socket);
	printf("id: %d, port: %d, hostname: %s, socketid: %d\n",temp->id,temp->port,temp->hostname,temp->player_socket);
	printf("left id: %d, left port: %d, left hostname: %s\n",temp->left_id,temp->left_port,temp->left_hostname);
	printf("right id: %d, right port: %d, right hostname: %s\n\n",temp->right_id,temp->right_port,temp->right_hostname);
}
