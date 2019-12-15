#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include "rsa.h"

int check_point = 0;

char ascci[] = {
'~',
'a', // 1
'b', // 2
'c', // 3
'd', // 4
'e', // 5
'f', // 6
'g', // 7
'h', // 8
'i', // 9
'j', // 10
'k', // 11
'l', // 12
'm', // 13
'n', // 14
'o', // 15
'p', // 16
'q', // 17
'r', // 18
's', // 19
't', // 20
'u', // 21
'v', // 22
'w', // 23
'x', // 24
'y', // 25
'z', // 26
' ', // 27
',', // 28
'.', // 29
';',
'@',
'#',
'$',
'%',
'^',
'&',
'*',
'(',
')',
'-',
'=',
'\\',
'"',
'\'',
'+',
'<',
'>',
':',
'{',
'}',
'[',
']',
'0',
'1',
'2',
'3',
'4',
'5',
'6',
'7',
'8',
'9'
};

double prime_n[]={
  9817, 9829, 9833, 9839, 9851,  9857 , 9859 , 9871 , 9883 , 9887, 9901 , 9907 , 9923 , 9929 , 9931 , 9941 , 9949 , 9967 , 9973 , 10007
};
double prime_e[]={
  29, 31, 37, 41, 43, 47, 53, 59, 61
};
void get_n(double *n, double *e, double* p, double* q){
  int i;
  int prime_n_len=sizeof(prime_n)/sizeof(prime_n[0]);
  time_t t;
  srand((unsigned) time(&t));
  i=rand()%prime_n_len;
  *p=prime_n[i];
  if(i==19){
    *q=prime_n[i-1];
  }else{
    *q=prime_n[i+1];
  } 
  *n=(*p)*(*q);
}
void get_e(double *e){
  int i;
  int prime_e_len=sizeof(prime_e)/sizeof(prime_e[0]);
  time_t t;
  srand((unsigned) time(&t));
  i=rand()%prime_e_len;
  *e = prime_e[i];
}
char end_char = '_';

int sizeof_ascii = (int) sizeof(ascci);

void setCheckPoint(int input) {
  check_point = input;
}

int getCharIndex(char input) {
  if(input == end_char) return sizeof_ascii;
  for(int i = 0; i < sizeof_ascii; i++) {
    if(ascci[i] == input) return i;
  }
}

int isPrimeNumber(double num) {
  for(int i = 2; i <= sqrt(num); i++){
    if(fmodl(num, i) == 0){
      return 0;
    }
  }

  return 1;
}

void analysisToTwoPrime(double num, double *num1, double *num2) {
  *num1 = 1;
  *num2 = num;

  // for(int i = 2; i <= sqrt(num); i++) {
  //   if(isPrimeNumber(i)) {
  //     if(fmodl(num, i) == 0) {
  //       if(isPrimeNumber(num / i)) {
  //         *num1 = i;
  //         *num2 = num / i;
  //         return;
  //       }
  //     }
  //   }
  // }
  for(double i = floor(sqrt(num)); i >= 2; i--) {
    if(isPrimeNumber(i)) {
      if(fmodl(num, i) == 0) {
        if(isPrimeNumber(num / i)) {
          *num1 = i;
          *num2 = num / i;
          return;
        }
      }
    }
  }
}

// Get Private Key
void euclid(double n, double e, double *d) {
  *d = 1;
  for(double i = 0; i < MAXNUMBER; i++) {
    if(fmodl(i*e - 1, n) == 0) {
      *d = i;
      return;
    }
  }
}

double gcd(double a, double b) {
  if (b == 0) return a;
  return gcd(b, fmodl(a, b));
}

void getInput(double n, double e, double* p, double* q, double* fi, double* d) {
 // analysisToTwoPrime(n, p, q);
  if(*p != 1) {
    // Get fi(n)
    *fi = (*p - 1) * (*q -1);
    euclid(*fi, e, d);
    if(*d == 1) {
      printf("[!]Can't find d\n");
    }
    return;
  }
}

// x ^ y mod n
char* crypto(char *monoPt, double y, double n) {
  double x = stringToDb(monoPt);
  if(check_point) printf("%s to %.1f\n", monoPt, x);

  if(x > n) {
    printf("%.1f is too small !!!\n\n", n);
    return NISTOOSMALLERRORMES;
  }

  // Decimal to binary
  int loops = decToBinary(y);
  double result = 1;
  for(int i = 0; i < loops; i++) {
    if(check_point) printf("Loops %d(%d) : %.1f^2 => ", i, binaryNum[i], result);
    result = fmodl((result * result), n);
    if(result > n / 2) result = n - result;
    if(check_point) printf("%.1f", result);
    if(binaryNum[i] == 1) {
      if(check_point) printf(" || %.1f", result);
      result = fmodl((result * x), n);
      if(result > n / 2) result = n - result;
      if(check_point) printf(" * %.1f => %.1f", x, result);
    }
    if(check_point) printf("\n");

  }
  if(result < 0) {
    result += n;
  }
  char* tmp = (char *)malloc(12 * sizeof(char));

  tmp = decToAlpha(result);
  if(check_point) printf("Cryp result : %s - %.1f\n", tmp, result);

  return tmp;
}

int decToBinary(double num) {
  int tmpBinary[MAXBINARY];

  int n = 0, tmp;

  if(check_point) printf("Dec : %.1f to Binary : ", num);

  while(num > 0) {
    tmp = (int)fmodl(num, 2);
    tmpBinary[n] = fmodl(num, 2);
    num = (num - tmp) / 2;
    n++;
  }
  tmpBinary[n] = '\0';

  int j = 0;
  for(int i = n - 1; i >= 0; i--) {
    binaryNum[j++] = tmpBinary[i];
  }

  binaryNum[n] = '\0';

  if(check_point) {
    for(int i = 0; i < n; i++) {
      printf("%d", binaryNum[i]);
    }
    printf("\n");
  }
  return n;
}
char* decToAlpha(double num) {
  int tmpAlpha[MAXBINARY];

  if(num == 0) {
    return "a";
  }

  if(check_point) printf("Dec to Alpha : %.1f\n", num);

  int n = 0;
  double bina;
  while(num > 0) {
    bina = fmodl(num, sizeof_ascii + 1);
    tmpAlpha[n] = (int)bina;
    num = (num - bina) / (sizeof_ascii + 1);
    n++;
  }
  tmpAlpha[n] = '\0';

  char* tmp = (char *)malloc(12 * sizeof(char));

  int j = 0;
  for(int i = n - 1; i >= 0; i--) {
    int tmp_index = tmpAlpha[i];
    if(tmp_index == sizeof_ascii) *(tmp + j++) = end_char;
    else *(tmp + j++) = ascci[tmpAlpha[i]];
    if(check_point) printf("Alpha : %d %c\n", tmpAlpha[i] , ascci[tmpAlpha[i]]);
  }
  *(tmp + n) = '\0';

  return tmp;
}

double stringToDb(char *input) {
  double result = 0;
  for(int i = 0; i < strlen(input); i++) {
    result += (getCharIndex(tolower(input[i]))) * pow(sizeof_ascii + 1, strlen(input) - i - 1);
    if(check_point) printf("i : %d - char : %c - %d\n", i, input[i], getCharIndex(input[i]));
  }
  if(check_point) printf("Result : %.1f\n", result);

  return result;
}

char* crypPlainText(char *pt, double y, double n, int way) {
  const char delim[] = "|";
  char *tmp = (char *)malloc(4 * sizeof(char));
  char *tmp2, *tmpPtList;
  char* result = (char *)malloc(MAXLEIGHT * sizeof(char));
  int j = 0;

  if(way == 0) {
    for(int i = 0; i < strlen(pt); i++) {
      *(tmp + j++) = *(pt + i);
      if(i == strlen(pt) - 1) {
        *(tmp + j++) = '\0';
      }
      if(j == 4 || i == strlen(pt) - 1) {

        if(check_point) printf("(%d - %d) %s\n", i, j, tmp);
        j = 0;
        tmp2 = crypto(tmp, y, n);
        if(strcmp(tmp2, NISTOOSMALLERRORMES) == 0) return NISTOOSMALLERRORMES;
        strcat(result, tmp2);
        if(i != strlen(pt) - 1) strcat(result, delim);
      }
    }
  } else if (way == 1) {
    tmpPtList = strtok(pt, delim);
    while (tmpPtList != NULL) {
      tmp = crypto(tmpPtList, y, n);
      if(strcmp(tmp, NISTOOSMALLERRORMES) == 0) return NISTOOSMALLERRORMES;
      strcat(result, tmp);
  		tmpPtList = strtok(NULL, delim);
      // if(tmpPtList != NULL) {
      //   strcat(result, " ");
      // }
  	}
  } else {
    return "Input error with : way";
  }

  return result;
}
