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
	if(num_players<1){
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
//initial print
  	printf("Ptotato Ringmaster\nPlayers = %d\nHops = %d\n",num_players,num_hops);
//create player array to store player information
  	//struct player* players[256];
	//printf("%d\n",server_fd);

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
//wait for all player connect to this game
  	struct player* players[512];//at most 512 player can join this game
  	struct sockaddr_in incoming;
  	for(int i=0;i<num_players;++i){
  		socklen_t len = sizeof(server_in);
  		int client_fd = accept(server_fd,(struct sockaddr*)&incoming,&len);
  		printf("%x\n",incoming.sin_addr.s_addr);
  		printf("%d\n",incoming.sin_port);

    	struct hostent* temp_host = gethostbyaddr((char *)&incoming.sin_addr, sizeof(struct in_addr), AF_INET);
  		struct player * temp = (struct player*)malloc(sizeof(struct player));
  		temp->id = i;
  		temp->hostname = temp_host->h_name;
  		temp->player_socket = client_fd;
  		temp->port = incoming.sin_port;

  		players[i] = temp;
  		printf("Player %d is ready to play\n",i);
  	

  		//char testmessage[256]="testmessage";
  		//send(client_fd,testmessage,sizeof(testmessage),0);
  		//memset(testmessage,'t',sizeof(testmessage)-200);
	}

//initial set up when you get all player connected
	for(int i=0;i<num_players;++i){
		if(i==0){
			players[0]->left_hostname = players[num_players-1]->hostname;
			players[0]->left_id = players[num_players-1]->id;
			players[0]->left_port = players[num_players-1]->port;
			players[0]->right_hostname = players[1]->hostname;
			players[0]->right_id = players[1]->id;
			players[0]->right_port = players[1]->port;
		}
		else if(i==num_players-1){
			players[num_players-1]->left_hostname = players[num_players-2]->hostname;
			players[num_players-1]->left_id = players[num_players-2]->id;
			players[num_players-1]->left_port = players[num_players-2]->port;
			players[num_players-1]->right_hostname = players[0]->hostname;
			players[num_players-1]->right_id = players[0]->id;
			players[num_players-1]->right_port = players[0]->port;
		}
		else{
			players[i]->left_hostname = players[i-1]->hostname;
			players[i]->left_id = players[i-1]->id;
			players[i]->left_port = players[i-1]->port;
			players[i]->right_hostname = players[i+1]->hostname;
			players[i]->right_id = players[i+1]->id;
			players[i]->right_port = players[i+1]->port;
		}
	}

  	time_t t;
  	srand((unsigned) time(&t));
  	int fp=rand()%num_players;
	printf("Ready to start the game, sending potato to player %d\n",fp);

	for(int i=0;i<num_players;++i){
		printplayer(players[i]);
	}
  	close(server_fd);

  	return EXIT_SUCCESS;
}