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
	struct player player_info_;
	memset(&player_info_,0,sizeof(player_info_));
	memcpy(&player_info_,buffer,sizeof(struct player));//get player_info

	memset(buffer,0,sizeof(buffer));
	struct player * player_info = &player_info_;
	printplayer(player_info);
//build connection with neighbors
	//bind this player
	int host_fd = socket(AF_INET,SOCK_STREAM,0);
	struct hostent * this_host_name = gethostbyname(player_info->hostname);
  	if ( this_host_name == NULL ) {
    	exit(EXIT_FAILURE);
  	}
   	struct sockaddr_in this_host_in;
  	this_host_in.sin_family = AF_INET;
  	this_host_in.sin_port = htons(player_info->port);
	//this_host_in.sin_addr.s_addr = INADDR_ANY;
	memcpy(&this_host_in.sin_addr, this_host_name->h_addr_list[0], this_host_name->h_length);
	printf("is %x\n",this_host_in.sin_addr.s_addr);
  	int bind_status = bind(host_fd,(struct sockaddr *)&this_host_in,sizeof(this_host_in));//assign a address to socket
  	
  	if ( bind_status < 0 ) {
    	perror("bind:");
    	exit(bind_status);
  	}
	listen(host_fd,5);

//tell server bind complete
	char hint[16]="COMPLETE";
	send(client_fd,hint,sizeof(hint),0);
	memset(hint,0,sizeof(hint));
	while(1){
		recv(client_fd,&hint,sizeof(hint),0);
		if(strcmp(hint,"KNOW")==0){
			break ; 
		}
	}

  	//send connect to left player
  	int left_client_fd = socket(AF_INET,SOCK_STREAM,0);//send to left player
	struct hostent * left_host_name = gethostbyname(player_info->left_hostname);
  	if ( left_host_name == NULL ) {
    	exit(EXIT_FAILURE);
  	}
   	struct sockaddr_in left_host_in;
  	left_host_in.sin_family = AF_INET;
  	left_host_in.sin_port = htons(player_info->left_port);
	memcpy(&left_host_in.sin_addr, left_host_name->h_addr_list[0], left_host_name->h_length);
	int left_connect_status = connect(left_client_fd,(struct sockaddr *)&left_host_in,sizeof(server_in));
	if(left_connect_status==-1){
		printf("ERROR: FAIL TO CONNECT TO THE LEFT PLAYER\n");
	}
	struct sockaddr_in incoming;
	socklen_t len = sizeof(incoming);
	int right_host_fd = accept(host_fd,(struct sockaddr*)&incoming,&len);//recv from right player
// tell server left connection is done
	char left_hint[16]="COMPLETE";
	send(client_fd,left_hint,sizeof(left_hint),0);
	memset(hint,0,sizeof(hint));
	while(1){
		recv(client_fd,&left_hint,sizeof(left_hint),0);
		if(strcmp(left_hint,"KNOW")==0){
			break ; 
		}
	}
	//send connect to right player
	int right_client_fd = socket(AF_INET,SOCK_STREAM,0);
	struct hostent * right_host_name = gethostbyname(player_info->right_hostname);
  	if ( right_host_name == NULL ) {
    	exit(EXIT_FAILURE);
  	}
   	struct sockaddr_in right_host_in;
  	right_host_in.sin_family = AF_INET;
  	right_host_in.sin_port = htons(player_info->right_port);
	memcpy(&right_host_in.sin_addr, right_host_name->h_addr_list[0], right_host_name->h_length);
	int right_connect_status = connect(right_client_fd,(struct sockaddr *)&right_host_in,sizeof(server_in));
	if(right_connect_status==-1){
		printf("ERROR: FAIL TO CONNECT TO THE RIGHT PLAYER\n");
	}
	int left_host_fd = accept(host_fd,(struct sockaddr*)&incoming,&len);

	char done[8]="DONE";
	send(client_fd,done,sizeof(done),0);
	//send(player_info->server_socket,done,sizeof(done),0);
//something very tricky, need further thinking
/*	if(player_info->change){
		int temp = left_host_fd;
		left_host_fd = right_host_fd;
		right_host_fd = temp;
	}

//test player communication

	if(player_info->id==0){
		char test[16]="from 0";
		send(left_client_fd,test,sizeof(test),0);
	}
	if(player_info->id==1){
		char test[16]="from 1";
		send(left_client_fd,test,sizeof(test),0);
	}
	if(player_info->id==2){
		char test[16]="from 2";
		send(left_client_fd,test,sizeof(test),0);
	}

	if(player_info->id==0){
		char test[16];
		memset(test,0,sizeof(test));
		recv(right_host_fd,&test,sizeof(test),0);
		printf("%s\n",test);
	}
	if(player_info->id==1){
		char test[16];
		memset(test,0,sizeof(test));
		recv(right_host_fd,&test,sizeof(test),0);
		printf("%s\n",test);
	}
	if(player_info->id==2){
		char test[16];
		memset(test,0,sizeof(test));
		recv(right_host_fd,&test,sizeof(test),0);
		printf("%s\n",test);
	}
*/
/*
RACE CONDITION!!!
	char test[16]="to left";
	send(left_host_fd,test,sizeof(test),0);
	char test_buffer[16];
	memset(test_buffer,0,sizeof(test_buffer));
	recv(left_client_fd,&test_buffer,sizeof(test_buffer),0);
	printf("%s\n",test_buffer);

	char test2[16]="to right";
	send(right_client_fd,test2,sizeof(test2),0);
	char test2_buffer[16];
	memset(test2_buffer,0,sizeof(test2_buffer));
	recv(right_host_fd,&test2_buffer,sizeof(test2_buffer),0);
	printf("%s\n",test2_buffer);
*/
	printf("Here is %d, left fd is %d, right fd is %d\n",player_info->id,left_host_fd,right_host_fd);



//construct fd_set
	fd_set set;
	FD_ZERO(&set);
	//FD_SET(client_fd, &set);
	FD_SET(left_host_fd, &set);
	FD_SET(right_host_fd, &set);
	int maxfdp = max(left_host_fd,right_host_fd)+1;
	int status=0;
	int direction=-1;
	while(1){
		status=0;
		char s_message[32];
		recv(client_fd,&s_message,sizeof(s_message),0);//IT,END,POTATO
		if(strcmp(s_message,"IT")==0){
			printf("I'm it\n");
			break ;
		}
		else if(strcmp(s_message,"END")==0){
			break ; 
		}
		else if(strcmp(s_message,"potato")==0){
			status=1;
		}
		else if(strcmp(s_message,"READY")==0){
			status=2;
		}
		else if(strcmp(s_message,"READY1")==0){
			status=3;
		}
		else{
			continue ;
		}
		if(status==1){
			//get potato from server
			char next_id[8];
			memset(next_id,0,sizeof(next_id));
			
			if(leftorright(player_info->id)==0){
				direction=1;
				itostr(next_id,player_info->left_id);
				printf("sending potato to %d\n",player_info->left_id);
			}
			else{
				direction=2;
				itostr(next_id,player_info->right_id);
				printf("sending potato to %d\n",player_info->right_id);
			}
			send(client_fd,next_id,sizeof(next_id),0);
			char ready[8];
			recv(client_fd,&ready,sizeof(ready),0);
			while(1){
				if(strcmp(ready,"READY1")==0){
					status=3;
					break ;
				}
			}			
		}
		if(status==2 || status==3){
			if(status==2){
				char ready[8]="ready";
				send(client_fd,ready,sizeof(ready),0);
				int am = select(maxfdp,&set,NULL,NULL,NULL);
				printf("active is %d\n",am);
				char recv_message[32];
				memset(recv_message,0,sizeof(recv_message));	
				if(FD_ISSET(left_host_fd,&set)){
					recv(left_host_fd,&recv_message,sizeof(recv_message),0);
				}
				else if(FD_ISSET(right_host_fd,&set)){
					recv(right_host_fd,&recv_message,sizeof(recv_message),0);
				}
				FD_ZERO(&set);
				FD_SET(left_host_fd, &set);
				FD_SET(right_host_fd, &set);
				direction=0;
				char next_id[8];
				memset(next_id,0,sizeof(next_id));
				if(leftorright(player_info->id)==0){
					direction=1;
					itostr(next_id,player_info->left_id);
					printf("sending potato to %d\n",player_info->left_id);
				}		
				else{
					direction=2;
					itostr(next_id,player_info->right_id);
					printf("sending potato to %d\n",player_info->right_id);
				}
				send(client_fd,next_id,sizeof(next_id),0);
				//status=3;		
			}
			if(status==3){
				char c_message[32]="hello";
				if(direction==1){
					send(left_client_fd,c_message,sizeof(c_message),0);
				}
				else if(direction==2){
					send(right_client_fd,c_message,sizeof(c_message),0);
				}				
			}
		}
	}

	close(client_fd);

	return EXIT_SUCCESS;
}