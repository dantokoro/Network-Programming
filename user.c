#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "user.h"

//Create a new node and add to the head of linked list
void push(Node** head, char username[], char password[], int status) 
{ 
    Node* new_node = (Node*) malloc(sizeof(Node)); 

    strcpy(new_node->username, username);
    strcpy(new_node->password, password);
    new_node->status  = status; 
    new_node->login=0;

    new_node->next = (*head); 
    (*head)    = new_node; 
} 

//search a node by username
Node* SearchName(Node *head, char name[]){			
	Node *cur;
	cur = head;
	while(cur!=NULL){
		if(strcmp(cur->username, name) == 0) return cur;
		cur = cur->next;
	}
	return cur;		//Neu k tim thay -> return NULL
}

void savePuclicKey(Node* cur, int i, char n[], char e[]){
	// Node *cur;
	
	// cur = SearchName((*head), name);			// cur = user dang nhap
	if(cur == NULL){	
		printf("Khong tim thay user vua dang nhap :|\n");
	}else{
		cur->i=i;
		strcpy(cur->n, n);
		strcpy(cur->e, e);
	}
}
// Print the linked list
void print_list(Node *head){
	printf("\nName\t\tPassword\ti\t\tn\t\te\n");
	while(head != NULL){
		printf("%s\t%12s%12d%20s%20s\n", head->username, head->password, head->i, head->n, head->e);
		head = head->next;
	}
	printf("\n");
}
void deleteList(Node** head_ref) 
{ 
   /* deref head_ref to get the real head */
   struct Node* current = *head_ref; 
   struct Node* next; 
  
   while (current != NULL)  
   { 
       next = current->next; 
       free(current); 
       current = next; 
   } 
    
   /* deref head_ref to affect the real head back 
      in the caller. */
   *head_ref = NULL; 
} 

//Read file and save into linked list
void docfile(char filename[], Node **head)
{
	FILE *fptr = fopen(filename,"r+");
	char name[50];
	char password[20];
	int status;
	char c;
	while(!feof(fptr)){
		fscanf(fptr, "%s %s %d\n", name, password, &status);
		push(head, name, password, status);
	}
	fclose(fptr);
}

//Write the linked list to the filename
void ghifile(char filename[], Node *head){
	FILE *fptr = fopen(filename,"w+");
	Node *cur;
	cur = head;
	while(cur!=NULL){
		fprintf(fptr, "%s %s %d\n", cur->username, cur->password, cur->status);
		cur = cur->next;
	}
	fclose(fptr);
}


//Menu's 1st func
void Register(Node **head, char filename[]){
	char name[50], password[20];
	printf("Username: \n");
	scanf("%s%*c", name);
	if(SearchName((*head), name) != NULL) 		//Neu username da ton tai
		printf("Account existed!!\n");			//-> bao loi
	else{										//Neu chua ton tai
		printf("Password: \n");					//nguoi dung nhap them password
		scanf("%s%*c", password);
		push(head, name, password, 1);			//luu vao linked list
		ghifile(filename, *head);
		printf("Successful registion\n");
	}
}

//Menu's 2nd func
void SignIn(Node **head, char filename[]){
	char name[50], password[20];
	int wrong_pass_count=0;
	printf("Username: \n");
	scanf("%s%*c", name);
	Node *cur;
	
	cur = SearchName((*head), name);			// cur = user dang nhap
	if(cur != NULL){							//Neu user da ton tai
		if(cur->status == 0){					//Neu user is blocked
			printf("Account is blocked\n");
		}else{
			do{
				printf("Password: \n");					//nguoi dung nhap them password
				scanf("%s%*c", password);
				if(strcmp(cur->password, password)!=0){	//Neu nhap sai
					printf("Password is incorrect.\n");
					wrong_pass_count++;					//Tang so lan nhap sai
				}else{									//Neu nhap dung
					printf("Hello %s\n", cur->username);	
					cur->login=1;
				} 
			}while(strcmp(cur->password, password)!=0 && wrong_pass_count<=3);
			if(wrong_pass_count>3){
				printf("Over 3 times incorrect password. Account is blocked\n");
				cur->status = 0;
				ghifile(filename, (*head));
			}
		}
	} else printf("Can't find this account\n");	 						
}	

//Menu's 3rd func
void Search(Node *head){
	char name[50];
	Node *cur;
	printf("Name of the user that you want to find: ");
	scanf("%s%*c", name);
	cur = SearchName(head, name);					//Tim user co ten da duoc nhap
	if(cur){										//Neu tim thay
		if(cur->login == 0){						//Neu chua dang nhap
			printf("This user is not login\n");
		}else{
			printf("Username: %s\n", cur->username);
			printf("Status: ");
			if(cur->status == 1) printf("Active\n");	//neu status=1 -> active 
			else printf("Blocked\n");
		}
	}else printf("Can't find this user.\n");		//Neu khong tim thay thong tin tai khoan
}	

//Menu's 4th func
void SignOut(Node **head){
	char name[50];
	Node *cur;
	printf("Name of the user that you want to Sign Out: ");
	scanf("%s%*c", name);
	cur = SearchName(*head, name);					//Tim user co ten da duoc nhap
	if(cur){										//Neu tim thay
		if(cur->login == 0){						//Neu chua dang nhap
			printf("This user is not login\n");
		}else{										//Neu da dang nhap
			printf("Bye %s\n", cur->username);
			cur->login=0;
		}
	}else printf("Can't find this user.\n");		//Neu khong tim thay thong tin tai khoan
}	