#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "rsa.h"

int check_point = 0;

int isPrimeNumber(double num) {
  for(int i = 2; i <= sqrt(num); i++){
    if(fmod(num, i) == 0){
      return 0;
    }
  }

  return 1;
}

void analysisToTwoPrime(double num, double *num1, double *num2) {
  *num1 = 1;
  *num2 = num;

  for(int i = 2; i <= sqrt(num); i++) {
    if(isPrimeNumber(i)) {
      if(fmod(num, i) == 0) {
        if(isPrimeNumber(num / i)) {
          *num1 = i;
          *num2 = num / i;
          return;
        }
      }
    }
  }
}

// Get Primary Key
void euclid(double n, double e, double *d) {
  *d = 1;
  for(int i = 0; i < MAXNUMBER; i++) {
    if(fmod(i*e - 1, n) == 0) {
      *d = i;
      return;
    }
  }
}

double gcd(double a, double b) {
  if (b == 0) return a;
  return gcd(b, fmod(a, b));
}

void getInput(double n, double e, double* p, double* q, double* fi, double* d) {
  analysisToTwoPrime(n, p, q);
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
  if(check_point) printf("%s to %.1lf\n", monoPt, x);

  if(x > n) {
    printf("%.1lf is too small !!!\n\n", n);
    return NISTOOSMALLERRORMES;
  }

  // Decimal to binary
  int loops = decToBinary(y);
  double result = 1;
  for(int i = 0; i < loops; i++) {
    if(check_point) printf("Loops %d(%d) : %.1lf^2 => ", i, binaryNum[i], result);
    result = fmod((result * result), n);
    if(check_point) printf("%.0lf", result);
    if(binaryNum[i] == 1) {
      if(check_point) printf(" || %.1lf", result);
      result = fmod((result * x), n);
      if(check_point) printf(" * %.1lf => %.1lf", x, result);
    }
    if(check_point) printf("\n");

  }
  if(result < 0) {
    result += n;
  }
  char* tmp = (char *)malloc(12 * sizeof(char));

  tmp = decToAlpha(result);
  if(check_point) printf("Cryp result : %s - %.1lf\n", tmp, result);

  return tmp;
}

int decToBinary(double num) {
  int tmpBinary[MAXBINARY];

  int n = 0, tmp;

  if(check_point) printf("Dec : %.1lf to Binary : ", num);

  while(num > 0) {
    tmp = (int)fmod(num, 2);
    tmpBinary[n] = fmod(num, 2);
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

  if(check_point) printf("Dec to Alpha : %.1lf\n", num);

  int n = 0;
  double bina;
  while(num > 0) {
    bina = fmod(num, 27);
    tmpAlpha[n] = (int)bina;
    num = (num - bina) / 27;
    n++;
  }
  tmpAlpha[n] = '\0';

  char* tmp = (char *)malloc(12 * sizeof(char));

  int j = 0;
  for(int i = n - 1; i >= 0; i--) {
    *(tmp + j++) = tmpAlpha[i] + 96;
    if(check_point) printf("Alpha : %d %d %c\n", tmpAlpha[i], tmpAlpha[i] + 96, tmpAlpha[i] + 96);
  }
  *(tmp + n) = '\0';

  return tmp;
}

double stringToDb(char *input) {
  double result = 0;
  for(int i = 0; i < strlen(input); i++) {
    result += (tolower(input[i]) - 96) * pow(27, strlen(input) - i - 1);
    if(check_point) printf("i : %d - char : %c - %d\n", i, input[i], input[i] - 96);
  }
  if(check_point) printf("Result : %.1lf\n", result);

  return result;
}

char* crypPlainText(char *pt, double y, double n) {
  const char delim[] = " ";
  char *tmp, *tmpPtList;
  char* result = (char *)malloc(MAXLEIGHT * sizeof(char));

  // slipt(pt, tmpPtList);
  tmpPtList = strtok(pt, delim);
  while (tmpPtList != NULL) {
    tmp = crypto(tmpPtList, y, n);
    if(strcmp(tmp, NISTOOSMALLERRORMES) == 0) return NISTOOSMALLERRORMES;
    strcat(result, tmp);
		tmpPtList = strtok(NULL, delim);
    if(tmpPtList != NULL) {
      strcat(result, " ");
    }
	}

  return result;
}
