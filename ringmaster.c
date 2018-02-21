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
	if(num_players<=1){
		printf("ERROR: NOT ENOUGH PLAYERS\n");
		exit(EXIT_FAILURE);
	}
	if(num_hops<0){
		printf("ERROR: HOT ENOUGH HOPS\n");
		exit(EXIT_FAILURE);
	}
	if(port_num<1025 || port_num>65535){
		printf("ERROR: WRONG NUMBER OF PORT\n");
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
	//printf("%d\n",host_info->h_length);//MSI.localdomain
	//printf("%s\n",host_info->h_aliases[0]);//MSI
	//printf("%p\n",host_info->h_addr_list[0]);//null

  	struct sockaddr_in server_in;
  	server_in.sin_family = AF_INET;
  	server_in.sin_port = htons(port_num);
	memcpy(&server_in.sin_addr, host_info->h_addr_list[0], host_info->h_length);


  	int bind_status = bind(server_fd,(struct sockaddr *)&server_in,sizeof(server_in));//assign a address to socket
  	if ( bind_status < 0 ) {
    	perror("bind:");
    	exit(bind_status);
  	}
  	//printf("%x\n",server_in.sin_addr.s_addr);
  	//printf("%s\n",host_info->h_addr_list[0]);

  	listen(server_fd,5);
//wait for all player connect to this game
  	struct player* players[512];//at most 512 player can join this game
  	struct sockaddr_in incoming;
  	fd_set set;
  	FD_ZERO(&set);
  	int maxfdp=0;
  	for(int i=0;i<num_players;++i){
  		socklen_t len = sizeof(server_in);
  		int client_fd = accept(server_fd,(struct sockaddr*)&incoming,&len);
  		//printf("%x\n",incoming.sin_addr.s_addr);
  		//printf("%d\n",incoming.sin_port);

    	//struct hostent* temp_host = gethostbyaddr((char *)&incoming.sin_addr, sizeof(struct in_addr), AF_INET);
  		struct player * temp = (struct player*)malloc(sizeof(struct player));
  		temp->id = i;
  		//strcpy(temp->hostname,temp_host->h_name);
  		temp->player_socket = client_fd;
  		//temp->port = incoming.sin_port;
  		//temp->port = 0;
  		players[i] = temp;
  		printf("Player %d is ready to play\n",i);
  		FD_SET(client_fd,&set);
  		maxfdp=max(maxfdp,client_fd);
	}
	maxfdp++;
// recv player port number and hostname
	for(int i=0;i<num_players;++i){
		char buffer2[1024];
		memset(buffer2,0,sizeof(buffer2));
		recv(players[i]->player_socket,&buffer2,sizeof(buffer2),0);
		struct player temp_player;
		memset(&temp_player,0,sizeof(temp_player));
		memcpy(&temp_player,buffer2,sizeof(temp_player));
		memset(buffer2,0,sizeof(buffer2));
		players[i]->port = temp_player.port;
		strcpy(players[i]->hostname,temp_player.hostname);
	}
//initial set up when you get all player connected
	//step1 
	for(int i=0;i<num_players;++i){
		players[i]->server_socket=server_fd;
		players[i]->change=0;
		if(i==0){
			strcpy(players[0]->left_hostname,players[num_players-1]->hostname);
			strcpy(players[0]->right_hostname,players[1]->hostname);
			//players[0]->left_hostname = players[num_players-1]->hostname;
			players[0]->left_id = players[num_players-1]->id;
			players[0]->left_port = players[num_players-1]->port;
			//players[0]->right_hostname = players[1]->hostname;
			players[0]->right_id = players[1]->id;
			players[0]->right_port = players[1]->port;
		}
		else if(i==num_players-1){
			strcpy(players[num_players-1]->left_hostname,players[num_players-2]->hostname);
			strcpy(players[num_players-1]->right_hostname,players[0]->hostname);
			//players[num_players-1]->left_hostname = players[num_players-2]->hostname;
			players[num_players-1]->left_id = players[num_players-2]->id;
			players[num_players-1]->left_port = players[num_players-2]->port;
			//players[num_players-1]->right_hostname = players[0]->hostname;
			players[num_players-1]->right_id = players[0]->id;
			players[num_players-1]->right_port = players[0]->port;
			players[num_players-1]->change=1;
		}
		else{
			strcpy(players[i]->left_hostname,players[i-1]->hostname);
			strcpy(players[i]->right_hostname,players[i+1]->hostname);
			//players[i]->left_hostname = players[i-1]->hostname;
			players[i]->left_id = players[i-1]->id;
			players[i]->left_port = players[i-1]->port;
			//players[i]->right_hostname = players[i+1]->hostname;
			players[i]->right_id = players[i+1]->id;
			players[i]->right_port = players[i+1]->port;
		}
	}
	//step2 make all player know what they need to know
	
	for(int i=0;i<num_players;++i){
		char buffer[1024];
		memset(buffer,0,sizeof(buffer));
		memcpy(buffer,players[i],sizeof(struct player));
		send(players[i]->player_socket,buffer,sizeof(buffer),0);
		//printplayer(players[i]);
	}
	//step2.3 make sure each player has bind completed
	char hint[16];
	for(int i=0;i<num_players;++i){
		while(1){
			memset(hint,0,sizeof(hint));
			recv(players[i]->player_socket,&hint,sizeof(hint),0);
			if(strcmp(hint,"COMPLETE")==0){
				break;
			}			
		}
	}
	char ack[16]="KNOW";
	for(int i=0;i<num_players;++i){
		send(players[i]->player_socket,ack,sizeof(ack),0);
	}
	//printf("all bind are done\n");
	//step 2.6 make sure all left connected
	char left_hint[16];
	for(int i=0;i<num_players;++i){
		while(1){
			memset(left_hint,0,sizeof(left_hint));
			recv(players[i]->player_socket,&left_hint,sizeof(left_hint),0);
			if(strcmp(left_hint,"COMPLETE")==0){
				break;
			}			
		}
	}
	char left_ack[16]="KNOW";
	for(int i=0;i<num_players;++i){
		send(players[i]->player_socket,left_ack,sizeof(left_ack),0);
	}	
	//printf("all left connection are done\n");
	//step3 make sure all players connect to their neighbor
	char done[8];
	for(int i=0;i<num_players;++i){
		while(1){
			memset(done,0,sizeof(done));
			recv(players[i]->player_socket,&done,sizeof(done),0);
			if(strcmp(done,"DONE")==0){
				//printf("%s\n",done);
				break;
			}			
		}
	}
	//printf("all right connection are done\n");

// now it is time to start the game, let's throw potato
	char trace[4096];
	memset(trace,0,sizeof(trace));
	strcat(trace,"Trace of potato:\n");
	//pick a random start
  	time_t t;
  	srand((unsigned) time(&t));
  	int fp=rand()%num_players;
	
	char potato[32] = "potato";
	char fake_potato[32] = "fake_potato";
	char this_id[8];
	//char next_id[8];

	int end_id=-1;
	printf("Ready to start the game, sending potato to player %d\n",fp);
	if(num_hops>0){
	for(int i=0;i<num_players;++i){
		if(i==fp){
			send(players[i]->player_socket,potato,sizeof(potato),0);			
		}
		else{
			send(players[i]->player_socket,fake_potato,sizeof(fake_potato),0);
		}
	}


	//printf("Ready to start the game, sending potato to player %d\n",fp);
	while(num_hops>0){
		int temp_fd=-1;
		select(maxfdp,&set,NULL,NULL,NULL);
		//printf("active is %d\n",am);	
		for(int i=0;i<num_players;i++){
			if(FD_ISSET(players[i]->player_socket,&set)){
				temp_fd = players[i]->player_socket;
				itostr(this_id,players[i]->id);
				break;
			}
		}
		strcat(trace,this_id);
		strcat(trace,",");
		char next_id[8];
		memset(next_id,0,sizeof(this_id));
		recv(temp_fd,&next_id,sizeof(next_id),0);

		int nid = atoi(next_id);
		for(int i=0;i<num_players;i++){
			if(players[i]->id==nid){
				char ready[8]="READY";
				send(players[i]->player_socket,ready,sizeof(ready),0);
				memset(ready,0,sizeof(ready));
				recv(players[i]->player_socket,&ready,sizeof(ready),0);
				while(1){
					if(strcmp(ready,"ready")==0){
						break ;
					}
				}
				break;
			}
		}

		strcat(trace,next_id);
		strcat(trace,"\n");
		memset(this_id,0,sizeof(this_id));
		strcpy(this_id,next_id);
		end_id=atoi(this_id);
		char ready[8]="READY1";
		send(temp_fd,ready,sizeof(ready),0);
		num_hops--;
		FD_ZERO(&set);
		for(int i=0;i<num_players;i++){
			FD_SET(players[i]->player_socket,&set);
		}		
	}
	}
	for(int i=0;i<num_players;i++){
		if(players[i]->id==end_id){
			char s_message[32]="IT";
			send(players[i]->player_socket,s_message,sizeof(s_message),0);
		}
		else{
			char s_message[32]="END";
			send(players[i]->player_socket,s_message,sizeof(s_message),0);			
		}

	}
	printf("%s",trace);

	for(int i=0;i<num_players;++i){
		free(players[i]);
	}

	close(server_fd);

  	return EXIT_SUCCESS;
}
  		//char testmessage[256]="testmessage";
  		//send(client_fd,testmessage,sizeof(testmessage),0);
  		//memset(testmessage,'t',sizeof(testmessage)-200);
