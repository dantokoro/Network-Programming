#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct Node 
{ 
	char username[50];
	char password[20];
	int status; 
	int login;
	int i;			//Port number
	char n[100];	//Public Key
	char e[100];
    struct Node *next; 
} Node;

void push(Node** head, char username[], char password[], int status);
Node* SearchName(Node *head, char name[]);
void savePuclicKey(Node*cur, int i, char n[], char e[]);
void print_list(Node *head);
void deleteList(Node** head_ref);
void docfile(char filename[], Node **head);
void ghifile(char filename[], Node *head);
void Register(Node **head, char filename[]);
void SignIn(Node **head, char filename[]);
void Search(Node *head);
void SignOut(Node **head);