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

void get_n(long double *n, long double *e, long double* p, long double* q);
void get_e(long double *e);
void setCheckPoint(int input);
int getCharIndex(char input);
int isPrimeNumber(long double num);
void analysisToTwoPrime(long double num, long double *num1, long double *num2);
void euclid(long double n, long double e, long double *d);
long double gcd(long double a, long double b);
int decToBinary(long double n);
char* decToAlpha(long double num);
long double stringToDb(char *input);
void printTokList(char *tokList);
void getInput(long double n, long double e, long double* p, long double* q, long double* fi, long double* d);
// way : 0 - cryp || 1 - decryp
char* crypPlainText(char *pt, long double y, long double n, int way);
char* crypto(char *monoPt, long double y, long double n);
