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

struct player{
	int id;
 	int listenport;
 	int speakport;
 	char *hostname;
 	int left_id;
 	int left_port;
 	char *left_hostname;
 	int right_id;
 	int right_port;
 	char *right_hostname;
 	int player_socket;
};