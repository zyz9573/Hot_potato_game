//created by Panjoy Zhang at 2/13/2018
#include "potato.h"

int main(int argc, char ** argv){
	char* hostname;
	hostname = argv[1];
	int port_num = atoi(argv[2]);

	struct hostent * host_info = gethostbyname(hostname);
  	if ( host_info == NULL ) {
    	exit(EXIT_FAILURE);
  	}

  	int client_fd = socket(AF_INET,SOCK_STREAM,0);

  	struct sockaddr_in server_in;
  	server_in.sin_family = AF_INET;
  	server_in.sin_port = htons(port_num);
	memcpy(&server_in.sin_addr, host_info->h_addr_list[0], host_info->h_length);
	
	int connect_status = connect(client_fd,(struct sockaddr *)&server_in,sizeof(server_in));
	if(connect_status==-1){
		printf("ERROR: FAIL TO CONNECT TO THE SERVER");
	}

	char buffer[1024];
	memset(buffer,0,sizeof(buffer));
	recv(client_fd,&buffer,sizeof(buffer),0);
	struct player test;
	memset(&test,0,sizeof(test));
	memcpy(&test,buffer,sizeof(struct player));
	printplayer(&test);
	
	close(client_fd);

	return EXIT_SUCCESS;
}