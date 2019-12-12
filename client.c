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

double n = 80460899, e = 37, d, p, q, fi;
int login=0;
char name[BUFSIZE];

//function log in with a username and password
void LogIn(int clientSocket){
	char password[1024], buffer[1024];
	int wrong_pass_count=0;
	char *temp_ptr;
	printf("Username: \n");
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
			printf("Password: \n");					//nguoi dung nhap them password
			scanf("%s%*c", password);
			send(clientSocket, password, strlen(password), 0);
			bzero(password, sizeof(password));
			bzero(buffer, sizeof(buffer));
			if(recv(clientSocket, buffer, 1024, 0) < 0){
				printf("[-]Error in receiving data.\n");
			}else if((wrong_pass_count=strtoul(buffer, &temp_ptr, 10))!=0){			//neu ki tu dau la so -> wrong_pass_count
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

void send_recv(int i, int sockfd)
{
  	char mess[BUFSIZE];
  	char cryp_recv_mess[BUFSIZE], text_send[BUFSIZE];
  	char cryp_mess[BUFSIZE];
	char recv_buf[BUFSIZE];
	int nbyte_recvd;
	const char delim[2]="-";
	char* sender_name;
	char* recv_message;

	if (i == 0){
		fgets(mess, BUFSIZE, stdin);
		if (strcmp(mess , "\n") == 0) {
			exit(0);
		}
	    mess[strlen(mess) - 1] = '\0';
	    //printf("User: %s; Mess: %s\n",name, mess );
	    strcpy(cryp_mess, crypPlainText(mess, e, n));
	    sprintf(text_send,"%s-%s", name, cryp_mess);
		send(sockfd, text_send, strlen(text_send), 0);
	}else {
		nbyte_recvd = recv(sockfd, recv_buf, BUFSIZE, 0);
		recv_buf[nbyte_recvd] = '\0';
		//printf("Receive: %s\n", recv_buf);
		//sender_id=strtoul(recv_buf, &recv_message, 10);
    	sender_name = strtok(recv_buf, delim);
    	//printf("%s\n", sender_name);
    	recv_message = strtok(NULL, delim);
    	strcpy(cryp_recv_mess, crypPlainText(recv_message, d, n));
		printf("From %s: %s\n" , sender_name, cryp_recv_mess);
		fflush(stdout);
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

int main()
{
	int sockfd, fdmax, i;
	struct sockaddr_in server_addr;
	fd_set master;
	fd_set read_fds;

  getInput(n, e, &p, &q, &fi, &d);

	connect_request(&sockfd, &server_addr);
	FD_ZERO(&master);
    FD_ZERO(&read_fds);
    FD_SET(0, &master);
    FD_SET(sockfd, &master);
	fdmax = sockfd;
	
	while(login==0)
		LogIn(sockfd);
	while(1){
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("select");
			exit(4);
		}

		for(i=0; i <= fdmax; i++ )
			if(FD_ISSET(i, &read_fds))
				send_recv(i, sockfd);
	}
	printf("client-quited\n");
	close(sockfd);
	return 0;
}
