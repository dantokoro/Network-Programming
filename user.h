#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct Node 
{ 
	char username[50];
	char password[20];
	int status; 
	int login;
    struct Node *next; 
} Node;

void push(Node** head, char username[], char password[], int status);
void print_list(Node *head);
void deleteList(Node** head_ref);
void docfile(char filename[], Node **head);
void ghifile(char filename[], Node *head);
Node* SearchName(Node *head, char name[]);
void Register(Node **head, char filename[]);
void SignIn(Node **head, char filename[]);
void Search(Node *head);
void SignOut(Node **head);