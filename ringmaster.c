//created by Panjoy Zhang at 2/13/2018
#include "potato.h"

int main(int argc, char** argv){
	if(argc!=4){
		printf("ERROR: %s <port_num> <num_players> <num_hops>\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	int port_num = atoi(argv[1]);
	int num_players = atoi(argv[2]);
	int num_hops = atoi(argv[3]);
	if(num_players<2){
		printf("ERROR: NOT ENOUGH PLAYERS");
		exit(EXIT_FAILURE);
	}
	if(num_hops<0){
		printf("ERROR: HOT ENOUGH HOPS");
		exit(EXIT_FAILURE);
	}
	if(port_num<1025 || port_num>65535){
		printf("ERROR: WRONG NUMBER OF PORT");
		exit(EXIT_FAILURE);
	}
	int server_fd = socket(AF_INET,SOCK_STREAM,0);//create socket;AF_INET means IPV4 and SOCK_STREAM means Socket type
	if ( server_fd < 0 ) {
    	perror("socket:");
    	exit(server_fd);
  	}

	printf("%d\n",server_fd);

  	char hostname[64];//
  	gethostname(hostname,64);//hostname is MSI in my local machine

	struct hostent * host_info = gethostbyname(hostname);
  	if ( host_info == NULL ) {
    	exit(EXIT_FAILURE);
  	}
	printf("%d\n",host_info->h_length);//MSI.localdomain
	//printf("%s\n",host_info->h_aliases[0]);//MSI
	printf("%p\n",host_info->h_addr_list[0]);//null

  	struct sockaddr_in server_in;
  	server_in.sin_family = AF_INET;
  	server_in.sin_port = htons(port_num);
	memcpy(&server_in.sin_addr, host_info->h_addr_list[0], host_info->h_length);


  	int bind_status = bind(server_fd,(struct sockaddr *)&server_in,sizeof(server_in));//assign a address to socket
  	if ( bind_status < 0 ) {
    	perror("bind:");
    	exit(bind_status);
  	}
  	printf("%x\n",server_in.sin_addr.s_addr);
  	printf("%s\n",host_info->h_addr_list[0]);

  	listen(server_fd,5);

  	int client_fd = accept(server_fd,NULL,NULL);
  	char testmessage[256]="testmessage";
  	send(client_fd,testmessage,sizeof(testmessage),0);
  	close(server_fd);

  	return EXIT_SUCCESS;
}