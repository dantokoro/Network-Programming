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
char user_port[20][30]; //ex: user_port[5]="dang", user_port[4]="tu"
int chatting[20]={}, login[20]={};   //ex: chatting[5]=4

void set_chatting_default_value(){  //Set all chatting status = -1
    for (int i = 0; i < 20; i++){
      chatting[i] = -1;
    }
}

void getKey(Node *cur, int i){      // Get Pubic Key of user
    int nbytes_recvd, j;
    char recv_buf[BUFSIZE];
    const char delim[2]="-";
    char *n;
    char *e;
    if ((nbytes_recvd = recv(i, recv_buf, BUFSIZE, 0)) <= 0) {
        if (nbytes_recvd == 0) {
            printf("socket %d hung up\n", i);
        }else {
            perror("recv");
        }
        close(i);
        //FD_CLR(i, master);
    }else {
        recv_buf[nbytes_recvd] = '\0';
        printf("Receive from user %d : %s\n", i, recv_buf);
        n = strtok(recv_buf, delim);
        e = strtok(NULL, delim);
        printf("%s %s\n",n, e );
        cur->i=i;
        strcpy(cur->n, n);
        strcpy(cur->e, e);
        bzero(recv_buf, sizeof(recv_buf));
    }
}

int HandlingLogIn(int newSocket, Node **head, char filename[]){
    char buffer[1024], temp[100];
    char name[1024], password[1024];
    int wrong_pass_count=0, ret;

    bzero(name, sizeof(name));
    ret = recv(newSocket, name, 1024, 0);
    name[ret]='\0';
    //printf("Username: %s\n", name);
    Node *cur;
    cur = SearchName((*head), name);            
    if(cur != NULL){                            
        if(cur->status == 0){                   
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
                if(strcmp(cur->password, password)!=0){ 
                    wrong_pass_count++;                 
                    strcpy(temp, "Password is incorrect");
                    sprintf(buffer, "%d-", wrong_pass_count);
                    strcat(buffer, temp);
                    printf("%s\n", buffer);
                    send(newSocket, buffer, strlen(buffer), 0);
                    bzero(buffer, sizeof(buffer));
                }else{                                  
                    printf("Hello %s\n", cur->username);    
                    cur->login=1;
                    strcpy(buffer, "Password is correct");
                    send(newSocket, buffer, strlen(buffer), 0);
                    getKey(cur, newSocket);
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

void active_user_list(Node *head, int i){       // send active user list to user
    Node *cur;
    int ret, j;
    char confirm[100], chosen_user[30];
    cur = head;
    while(cur!=NULL){
        if((cur->login) == 1){
            send(i, cur->username, strlen(cur->username), 0);
            strcpy(user_port[cur->i],cur->username);            
            ret = recv(i, confirm, 1024, 0);
            confirm[ret]='\0';
            //printf("%s\n",confirm );
        }
        cur = cur->next;
    }
    send(i, "|done|", strlen("|done|"), 0);
    ret = recv(i, confirm, 1024, 0);
    confirm[ret]='\0';
}

void send_recv(int i, fd_set *master, Node* head)    //Handle send and receive message
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
        recv_buf[nbytes_recvd] = '\0';
        printf("Receive from user %d : %s\n", i, recv_buf);
        send(chatting[i], recv_buf, nbytes_recvd, 0);
        if (strcmp(recv_buf, "end_chat")==0)
        {
            send(i, recv_buf, nbytes_recvd, 0);
            recv(i, recv_buf, BUFSIZE, 0);
            active_user_list(head, i);
            active_user_list(head, chatting[i]);
            chatting[chatting[i]]=-1;
            chatting[i]=-1;
        }
        bzero(recv_buf, sizeof(recv_buf));
    }
}

void handle_connect_to_friend(int i, fd_set *master, int sockfd, int fdmax, Node* head) //Receive user2's name and create box chat for user1 and user2
{
    int nbytes_recvd, j;
    char recv_buf[BUFSIZE], buf[BUFSIZE];
    char send_mess[BUFSIZE];
    Node *cur1, *cur2;

    if ((nbytes_recvd = recv(i, recv_buf, BUFSIZE, 0)) <= 0) {  //i = user A
        if (nbytes_recvd == 0) {
            printf("socket %d hung up\n", i);
        }else {
            perror("recv");
        }
        close(i);
        FD_CLR(i, master);
    }else {
        recv_buf[nbytes_recvd] = '\0';
        printf("Receive from user %d : %s\n", i, recv_buf);
        if(strcmp(recv_buf, "update_list")==0){
            active_user_list(head, i);
            send(i, "OK", strlen("OK"), 0);
            handle_connect_to_friend(i, master, sockfd, fdmax, head);
        }else{
            cur1=SearchName(head, user_port[i]);
            cur2=SearchName(head, recv_buf);        //send notification to user2
            if(cur2==NULL){
                printf("Khong tim thay thong tin user2\n");
                send(i, "No_user_found", strlen("No_user_found"), 0);
                handle_connect_to_friend(i, master, sockfd, fdmax, head);
            }else if(cur2->login==0){
                printf("user2 chua login\n");
                send(i, "Not_online", strlen("Not_online"), 0);
                handle_connect_to_friend(i, master, sockfd, fdmax, head);
            }else if(chatting[cur2->i]!=-1){
                printf("user2 dang trong phong chat khac\n");
                send(i, "In_other_box", strlen("In_other_box"), 0);
                handle_connect_to_friend(i, master, sockfd, fdmax, head);
            }else{
                j=cur2->i;          //j=chosen user's port
                sprintf(send_mess, "%s-%s-%s", cur1->n, cur1->e, cur1->username);
                if (send(j, send_mess, strlen(send_mess), 0) == -1) {
                    perror("send");
                }
                sprintf(send_mess, "%s-%s-%s", cur2->n, cur2->e, cur2->username);   //send notification to user1
                if (send(i, send_mess, strlen(send_mess), 0) == -1) {
                    perror("send");
                }
                chatting[i]=j;
                chatting[j]=i;
                bzero(recv_buf, sizeof(recv_buf));
            }
        }
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
        //  perror("send");
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
    int ret;
    Node *head = NULL;
    char filename[30]="account.txt";
    docfile(filename, &head);
    set_chatting_default_value(); //Set all chatting status =-1 (not inbox yet)

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
                            printf("------------LOGIN-------------\n");
                            login[i] = HandlingLogIn(i, &head, filename);
                            print_list(head);
                        }
                        active_user_list(head, i);

                    }else{
                        if(chatting[i]==-1){
                            handle_connect_to_friend(i, &master, sockfd, fdmax, head);
                        }
                        send_recv(i, &master, head);
                    }
                }
            }
        }
    }
    return 0;
}
