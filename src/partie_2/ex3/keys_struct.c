#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "keys_struct.h"
#include "../../partie_1/ex2/keys.h"
#include "../../partie_1/ex1/miller_rabin.h"
#include "../../params.h"

void free_signature(Signature *s)
{
  if (s)
  {
    free(s->content);
    free(s);
  }

  return;
}

void free_protected(Protected *p)
{
  if (p)
  {
    free_signature(p->sgn);
    free(p->declaration_vote);
    free(p->pKey);
    free(p);
  }

  return;
}

void print_long_vector(long *result, int size)
{
  for (int i = 0; i < size; i++)
  {
    printf("%lx ", result[i]);
  }
  printf("\n");
}

// initialise les valeur d'une clé
void init_key(Key *key, long val, long n)
{
  key->val = val;
  key->n = n;

  return;
}

// génère un couple de clé
void init_pair_keys(Key *pKey, Key *sKey, long low_size, long up_size)
{
  long p = random_prime_number(low_size, up_size, NB_TEST_MILLER_RABIN);
  long q = random_prime_number(low_size, up_size, NB_TEST_MILLER_RABIN);
  long n, s, u;

  generate_key_values(p, q, &n, &s, &u);

  init_key(pKey, s, n);
  init_key(sKey, u, n);

  return;
}

char *key_to_str(Key *key)
{
  char *buffer = malloc(sizeof(char) * 256);
  if (!buffer)
    exit(12);

  sprintf(buffer, "(%lx,%lx)", key->val, key->n);

  return buffer;
}

Key *str_to_key(char *str)
{
  Key *res = (Key *)malloc(sizeof(Key));
  if (!res)
    exit(12);

  sscanf(str, "(%lx, %lx)", &res->val, &res->n);

  return res;
}

// content = message chiffré avec sa clé secrète
Signature *init_signature(long *content, int size)
{
  Signature *res = (Signature *)malloc(sizeof(Signature));
  if (!res)
    exit(12);

  res->content = content;
  res->size = size;

  return res;
}

Signature *sign(char *mess, Key *sKey)
{
  Signature *res = init_signature(encrypt(mess, sKey->val, sKey->n), strlen(mess));

  return res;
}

char *signature_to_str(Signature *sgn)
{
  char *result = (char *)malloc(10 * sgn->size * sizeof(char));
  result[0] = '#';
  int pos = 1;
  char buffer[156];
  for (int i = 0; i < sgn->size; i++)
  {
    sprintf(buffer, "%lx", sgn->content[i]);
    for (int j = 0; j < strlen(buffer); j++)
    {
      result[pos] = buffer[j];
      pos++;
    }
    result[pos] = '#';
    pos++;
  }
  result[pos] = '\0';
  result = (char *)realloc(result, (pos + 1) * sizeof(char));
  return result;
}

Signature *str_to_signature(char *str)
{
  int len = strlen(str);
  long *content = (long *)malloc(sizeof(long) * len);
  int num = 0;
  char buffer[256];
  int pos = 0;
  for (int i = 0; i < len; i++)
  {
    if (str[i] != '#')
    {
      buffer[pos] = str[i];
      pos++;
    }
    else
    {
      if (pos)
      {
        buffer[pos] = '\0';
        sscanf(buffer, "%lx", &(content[num]));
        num++;
        pos = 0;
      }
    }
  }

  content = (long *)realloc(content, num * sizeof(long));

  return init_signature(content, num);
}

Protected *init_protected(Key *pKey, char *mess, Signature *sgn)
{
  Protected *res = (Protected *)malloc(sizeof(Protected));
  if (!res)
    exit(12);

  res->pKey = pKey;
  res->declaration_vote = mess;
  res->sgn = sgn;

  return res;
}

// return 0 si la signature est incorrect, 1 sinon;
int verify(Protected *pr)
{
  char *dechiffre;
  int res;

  dechiffre = decrypt(pr->sgn->content, pr->sgn->size, pr->pKey->val, pr->pKey->n);
  res = strcmp(dechiffre, pr->declaration_vote);
  
  free(dechiffre);

  return res == 0;
}

char *protected_to_str(Protected *pr)
{
  char *res = malloc(sizeof(char) * 256);
  char *key = key_to_str(pr->pKey);
  char *sgn = signature_to_str(pr->sgn);

  sprintf(res, "%s %s %s", key, pr->declaration_vote, sgn);

  free(key);
  free(sgn);

  return res;
}

Protected *str_to_protected(char *str)
{
  char buffer_sgn[256], buffer_pKey[256];
  char *declaration_vote = (char *)malloc(sizeof(char) * 256);
  sscanf(str, "%s %s %s", buffer_pKey, declaration_vote, buffer_sgn);

  Key *pKey = str_to_key(buffer_pKey);
  Signature *sign = str_to_signature(buffer_sgn);
  Protected *res = init_protected(pKey, declaration_vote, sign);

  return res;
}