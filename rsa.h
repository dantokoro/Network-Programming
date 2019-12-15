#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>

#define MAXNUMBER 100000000000
#define MAXLEIGHT 10
#define MAXBINARY 500

#define NISTOOSMALLERRORMES "N is too small !!!\n\n"

int check;
int binaryNum[MAXBINARY];

void get_n(double *n, double *e, double* p, double* q);
void get_e(double *e);
void setCheckPoint(int input);
int getCharIndex(char input);
int isPrimeNumber(double num);
void analysisToTwoPrime(double num, double *num1, double *num2);
void euclid(double n, double e, double *d);
double gcd(double a, double b);
int decToBinary(double n);
char* decToAlpha(double num);
double stringToDb(char *input);
void printTokList(char *tokList);
void getInput(double n, double e, double* p, double* q, double* fi, double* d);
// way : 0 - cryp || 1 - decryp
char* crypPlainText(char *pt, double y, double n, int way);
char* crypto(char *monoPt, double y, double n);
