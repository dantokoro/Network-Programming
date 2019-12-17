#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

#include "user.h"
#include "rsa.h"

#define BUFSIZE 1024

double n, e, d, p, q, fi, n_friend_double, e_friend_double;
int login=0, chatting=0;
char name[BUFSIZE];

void LogIn(int clientSocket){		//function log in with a username and password
	char password[1024], buffer[1024];
	int wrong_pass_count=0;
	char *temp_ptr;
	printf("Username: ");
	fgets(name, 100, stdin);
	name[strlen(name) - 1] = '\0';
	if(name[0]=='\0') exit(0);
	send(clientSocket, name, strlen(name), 0);
	bzero(buffer, sizeof(buffer));
	if(recv(clientSocket, buffer, 1024, 0) < 0){
		printf("[-]Error in receiving data.\n");
	}else if(strcmp(buffer, "Account is blocked")==0){
		printf("%s\n", buffer);
	}else if(strcmp(buffer, "Account is active")==0){
		do{
			printf("Password: ");					
			scanf("%s%*c", password);
			send(clientSocket, password, strlen(password), 0);
			bzero(password, sizeof(password));
			bzero(buffer, sizeof(buffer));
			if(recv(clientSocket, buffer, 1024, 0) < 0){
				printf("[-]Error in receiving data.\n");
			}else if((wrong_pass_count=strtoul(buffer, &temp_ptr, 10))!=0){			
				printf("%s\n", buffer);
				if(wrong_pass_count>3){
					printf("Over 3 times incorrect password. Account is blocked\n");
					break;
				}
			}else if(strcmp(buffer, "Password is correct")==0){
				printf("Login successfully\n");
				login=1;
				break;
			}else
				printf("%s\n",buffer);
		}while(1);////
	}else if(strcmp(buffer, "Account is not exist")==0){
		printf("%s\n", buffer);
	}else 
		printf("%s\n", buffer);
	if(login==0)
		bzero(name, sizeof(name));

}

void get_active_user_list(int sockfd){
	int nbyte_recvd, count=1;
	char friend_name[100];
	char recv_buf[BUFSIZE];
	printf("\t----ACTIVE USERS----\n");
	do{
		bzero(recv_buf, sizeof(recv_buf));
		nbyte_recvd = recv(sockfd, recv_buf, BUFSIZE, 0);
		recv_buf[nbyte_recvd] = '\0';
		send(sockfd, "OK", strlen("OK"), 0);
		if(strcmp(recv_buf, "|done|")!=0){
			printf("\t|\t%s\t   |\n", recv_buf );
			count++;
		}
	}while(strcmp(recv_buf, "|done|")!=0);
	bzero(recv_buf, sizeof(recv_buf));
	printf("\t--------------------\n");
	printf("Input name of user who you want to chat with (input update_list to renew list)\n");
	
}

void send_recv(int i, int sockfd)		//send and recv mess
{
  	char mess[BUFSIZE];
  	char cryp_recv_mess[BUFSIZE], text_send[BUFSIZE];
  	char cryp_mess[BUFSIZE];
	char recv_buf[BUFSIZE];
	int nbyte_recvd;
	const char delim[2]="^";
	char* sender_name;
	char* recv_message;
	
	if (i == 0){
		fgets(mess, BUFSIZE, stdin);
		if (strcmp(mess , "\n") == 0) {
			exit(0);
		}
	    mess[strlen(mess) - 1] = '\0';
	    if (strcmp(mess, "end_chat")==0){
			send(sockfd, mess, strlen(text_send), 0);
		}else{
		    strcpy(cryp_mess, crypPlainText(mess, e_friend_double, n_friend_double, 0));
		    sprintf(text_send,"%s^%s", name, cryp_mess);
			send(sockfd, text_send, strlen(text_send), 0);
		}
	}else {
		nbyte_recvd = recv(sockfd, recv_buf, BUFSIZE, 0);
		recv_buf[nbyte_recvd] = '\0';
		if (strcmp(recv_buf, "end_chat")==0){
			printf("-------End chat-------\n" );
			//printf("Input name of user who you want to chat with\n");
			send(sockfd, "Send_user2_name", strlen("Send_user2_name"), 0);
			get_active_user_list(sockfd);
			chatting=0;
		}else{
			sender_name = strtok(recv_buf, delim);
	    	recv_message = strtok(NULL, delim);
	    	strcpy(cryp_recv_mess, crypPlainText(recv_message, d, n, 1));
			printf("%70s |", cryp_recv_mess);
			printf("\033[1;32m");
			printf("%s" , sender_name);
			printf("\033[0m");
			printf("|\n");
			fflush(stdout);
		}
	}
}


void connect_request(int *sockfd, struct sockaddr_in *server_addr)
{
	char my_port[20];
	int fd;
	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}
	server_addr->sin_family = AF_INET;
	server_addr->sin_port = htons(4950);
	server_addr->sin_addr.s_addr = inet_addr("127.0.0.1");
	memset(server_addr->sin_zero, '\0', sizeof server_addr->sin_zero);

	if(connect(*sockfd, (struct sockaddr *)server_addr, sizeof(struct sockaddr)) == -1) {
		perror("connect");
		exit(1);
	}
	// int nbyte_recvd = recv(*sockfd, my_port, 20, 0);
	// my_port[nbyte_recvd] = '\0';
	 //printf("-----------My ID: %d----------\n",fd );
	// bzero(my_port, sizeof(my_port));

}
void createKey(double *n, double *e, double* p, double* q, double *d, double *fi, int sockfd){	//Generate public key and calculate primary key
	char publicKey[100];
	get_n(n, e, p, q);
	get_e(e);
	getInput(*n, *e, p, q, fi, d);
	sprintf(publicKey,"%f-%f", *n, *e);
	//printf("%s\n", publicKey);
	send(sockfd, publicKey, strlen(publicKey), 0);
	bzero(publicKey, sizeof(publicKey));
}

void connect_to_friend(int i, int sockfd){
	char mess[BUFSIZE];
	char recv_buf[BUFSIZE];
	int nbyte_recvd;
	const char delim[2]="-";
	char *n_friend, *e_friend, *friend_name, *ptr;

	if (i == 0){		//user1
		fgets(mess, BUFSIZE, stdin);
		if (strcmp(mess , "\n") == 0) {
			exit(0);
		}
	    mess[strlen(mess) - 1] = '\0';
		send(sockfd, mess, strlen(mess), 0);
		if(strcmp(mess, "update_list")==0){
			get_active_user_list(sockfd);
			nbyte_recvd = recv(sockfd, recv_buf, BUFSIZE, 0);
			recv_buf[nbyte_recvd] = '\0';
			connect_to_friend(i,sockfd);
		}else{
			nbyte_recvd = recv(sockfd, recv_buf, BUFSIZE, 0);
			recv_buf[nbyte_recvd] = '\0';
			if(strcmp(recv_buf, "No_user_found")==0){
				printf("User not exist :( Input again.\n");
				connect_to_friend(i,sockfd);
			}else if(strcmp(recv_buf, "Not_online")==0){
				printf("%s is not online :( Choose another user\n", mess);
				connect_to_friend(i,sockfd);
			}else if(strcmp(recv_buf, "In_other_box")==0){
				printf("%s is busy :( Choose another user\n", mess);
				connect_to_friend(i,sockfd);
			}else{
				n_friend = strtok(recv_buf, delim);
			    e_friend = strtok(NULL, delim);
			    friend_name = strtok(NULL, delim);
			    n_friend_double=strtod(n_friend, &ptr);
			    e_friend_double=strtod(e_friend, &ptr);
			    printf("You are inbox with %s :D (input 'end_chat' to end chat) \n",friend_name);
			    printf("\t\t-------------------CHAT WITH %s-------------------\n", friend_name);
			}
			
	    }
	}else {		//user2
		nbyte_recvd = recv(sockfd, recv_buf, BUFSIZE, 0);
		recv_buf[nbyte_recvd] = '\0';
		n_friend = strtok(recv_buf, delim);
	    e_friend = strtok(NULL, delim);
	    friend_name = strtok(NULL, delim);
	    n_friend_double=strtod(n_friend, &ptr);
	    e_friend_double=strtod(e_friend, &ptr);
	    printf("You are inbox with %s :D (input 'end_chat' to end chat) \n",friend_name);
		printf("\t\t-----------------CHAT WITH %s-----------------\n", friend_name);
		fflush(stdout);
	}
}

int main()
{
	int sockfd, fdmax, i;
	struct sockaddr_in server_addr;
	fd_set master;
	fd_set read_fds;


	connect_request(&sockfd, &server_addr);
	FD_ZERO(&master);
    FD_ZERO(&read_fds);
    FD_SET(0, &master);
    FD_SET(sockfd, &master);
	fdmax = sockfd;
	
	while(login==0)
		LogIn(sockfd);
	createKey(&n, &e, &p, &q, &d, &fi, sockfd);
//	printf("e=%f n=%f d=%f\n",e, n, d );
	get_active_user_list(sockfd);
	while(1){
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("select");
			exit(4);
		}
		for(i=0; i <= fdmax; i++ )
			if(FD_ISSET(i, &read_fds)){
				if(chatting==0){
					connect_to_friend(i, sockfd);
					chatting=1;
				}
				send_recv(i, sockfd);
			}
	}
	printf("client-quited\n");
	close(sockfd);
	return 0;
}
