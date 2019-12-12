#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "user.h"
#define PORT 4950
#define BUFSIZE 1024

int HandlingLogIn(int newSocket, Node **head, char filename[]){
	char buffer[1024], temp[100];
	char name[1024], password[1024];
	int wrong_pass_count=0, ret;

	bzero(name, sizeof(name));
	ret = recv(newSocket, name, 1024, 0);
	name[ret]='\0';
	//printf("Username: %s\n", name);
	Node *cur;
	cur = SearchName((*head), name);			// cur = user dang nhap
	if(cur != NULL){							//Neu user da ton tai
		if(cur->status == 0){					//Neu user is blocked
			printf("Account is blocked\n");
			strcpy(buffer, "Account is blocked");
			send(newSocket, buffer, strlen(buffer), 0);
			bzero(buffer, sizeof(buffer));
		}else{
			strcpy(buffer, "Account is active");
			send(newSocket, buffer, strlen(buffer), 0);
			bzero(buffer, sizeof(buffer));
			do{
				ret = recv(newSocket, password, 1024, 0);
				password[ret]='\0';
				//printf("Password: %s\n", password);
				if(strcmp(cur->password, password)!=0){	//Neu nhap sai
					wrong_pass_count++;					//Tang so lan nhap sai
					strcpy(temp, "Password is incorrect");
					sprintf(buffer, "%d-", wrong_pass_count);
					strcat(buffer, temp);
					printf("%s\n", buffer);
					send(newSocket, buffer, strlen(buffer), 0);
					bzero(buffer, sizeof(buffer));
				}else{									//Neu nhap dung
					printf("Hello %s\n", cur->username);	
					cur->login=1;
					strcpy(buffer, "Password is correct");
					send(newSocket, buffer, strlen(buffer), 0);
					bzero(buffer, sizeof(buffer));
				} 
			}while(strcmp(cur->password, password)!=0 && wrong_pass_count<=3);
			if(wrong_pass_count>3){
				printf("Over 3 times incorrect password. Account is blocked\n");
				cur->status = 0;
				ghifile(filename, (*head));
			}
		}
	} else{
		strcpy(buffer, "Account is not exist");
		send(newSocket, buffer, strlen(buffer), 0);
		bzero(buffer, sizeof(buffer));
		printf("Can't find this account\n");	
		return 0; 
	}	
	return cur->login;	
}

void send_to_all(int j, int i, int sockfd, int nbytes_recvd, char *recv_buf, fd_set *master)
{
	if (FD_ISSET(j, master)){
		if (j != sockfd && j != i) {
			if (send(j, recv_buf, nbytes_recvd, 0) == -1) {
				perror("send");
			}
		}
	}
}

void send_recv(int i, fd_set *master, int sockfd, int fdmax)
{
	int nbytes_recvd, j;
	char recv_buf[BUFSIZE], buf[BUFSIZE];
	char send_mess[BUFSIZE];

	if ((nbytes_recvd = recv(i, recv_buf, BUFSIZE, 0)) <= 0) {
		if (nbytes_recvd == 0) {
			printf("socket %d hung up\n", i);
		}else {
			perror("recv");
		}
		close(i);
		FD_CLR(i, master);
	}else {
	//	printf("%s\n", recv_buf);
		recv_buf[nbytes_recvd] = '\0';
		printf("Receive from user %d : %s\n", i, recv_buf);
		// sprintf(send_mess, "%d", i);
		// strcat(send_mess, recv_buf);
		//printf("Send to clients: %s\n", send_mess);
		for(j = 0; j <= fdmax; j++){
			send_to_all(j, i, sockfd, 100, recv_buf, master );
		}
		//bzero(send_mess, sizeof(send_mess));
		bzero(recv_buf, sizeof(recv_buf));
	}
}

void connection_accept(fd_set *master, int *fdmax, int sockfd, struct sockaddr_in *client_addr)
{
	socklen_t addrlen;
	int newsockfd;
	char client_port[20];

	addrlen = sizeof(struct sockaddr_in);
	if((newsockfd = accept(sockfd, (struct sockaddr *)client_addr, &addrlen)) == -1) {
		perror("accept");
		exit(1);
	}else {
		FD_SET(newsockfd, master);
		if(newsockfd > *fdmax)
			*fdmax = newsockfd;
		printf("new connection from %s on port %d \n",inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port));
		// sprintf(client_port, "%d", ntohs(client_addr->sin_port));
		// if (send(newsockfd, client_port, strlen(client_port), 0) == -1) {
		// 	perror("send");
		// }
	}
}

void connect_request(int *sockfd, struct sockaddr_in *my_addr)
{
	int yes = 1;

	if ((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Socket");
		exit(1);
	}

	my_addr->sin_family = AF_INET;
	my_addr->sin_port = htons(4950);
	my_addr->sin_addr.s_addr = INADDR_ANY;
	memset(my_addr->sin_zero, '\0', sizeof my_addr->sin_zero);

	if (setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
		perror("setsockopt");
		exit(1);
	}

	if (bind(*sockfd, (struct sockaddr *)my_addr, sizeof(struct sockaddr)) == -1) {
		perror("Unable to bind");
		exit(1);
	}
	if (listen(*sockfd, 10) == -1) {
		perror("listen");
		exit(1);
	}
	printf("\nTCPServer Waiting for client on port 4950\n");
	fflush(stdout);
}
int main()
{
	char name[30];
	int ret, login[20]={};

	fd_set master;
	fd_set read_fds;
	int fdmax, i;
	int sockfd= 0;
	struct sockaddr_in my_addr, client_addr;

	FD_ZERO(&master);
	FD_ZERO(&read_fds);
	connect_request(&sockfd, &my_addr);
	FD_SET(sockfd, &master);

	fdmax = sockfd;
	while(1){
		read_fds = master;
		if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1){
			perror("select");
			exit(4);
		}
		for (i = 0; i <= fdmax; i++){
			if (FD_ISSET(i, &read_fds)){
				if (i == sockfd){
					connection_accept(&master, &fdmax, sockfd, &client_addr);
				}
				else{
					if(login[i]==0){
						while(login[i]==0){
							Node *head = NULL;
							char filename[30]="account.txt";
							docfile(filename, &head);
							//print_list(head);
							printf("------------LOGIN-------------\n");
							login[i] = HandlingLogIn(i, &head, filename);
							deleteList(&head);
						}	
					}else
						send_recv(i, &master, sockfd, fdmax);
				}
			}
		}
	}
	return 0;
}
