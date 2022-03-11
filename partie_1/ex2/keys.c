#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "keys.h"
#include "../ex1/exponentiation.h"
#include "../ex1/miller_rabin.h"

long extended_gcd(long s, long t, long *u, long *v) {
  if(s == 0) {
    *u = 0;
    *v = 1;
    return t;
  }

  long uPrim, vPrim;
  long gcd = extended_gcd(t%s, s, &uPrim, &vPrim);
  *u = vPrim - (t/s)*uPrim;
  printf("u=%ln\n", u);
  *v = uPrim;

  return gcd;
}

void generate_key_values(long p, long q, long* n, long *s, long *u) {
  *n = p * q;
  long t = (p - 1) * (q - 1);
  long v;

  do {
    *s = rand_long(1, t);
  }while (extended_gcd(*s, t, u, &v) != 1);

  return;
}

long* encrypt(char* chaine, long s, long n) {
  long *encrypted = (long*)malloc(sizeof(long) * strlen(chaine));
  if(!encrypted) {
    exit(12);
  }

  int i = 0;
  while(chaine[i] != '\0') {
    printf("%d\t%ld\n", (int)chaine[i], (long)chaine[i]);
    encrypted[i] = modpow((long) chaine[i], s, n);
    i++;
  }

  return encrypted;
}

char* decrypt(long* crypted, int size, long u, long n) {
  char *decrypted = (char*)malloc(sizeof(char) * (size + 1));
  if(!decrypted) {
    exit(12);
  }

  for(int i=0; i < size; i++) {
    decrypted[i] = modpow(crypted[i], u, n) + '0';
  }

  decrypted[size] = '\0';

  return decrypted;
}