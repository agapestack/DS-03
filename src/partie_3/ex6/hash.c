#include <stdio.h>
#include <stdlib.h>

#include "hash.h"
#include "../ex5/linked_list.h"
#include "../../partie_2/ex3/keys_struct.h"

void verify_list_protected(CellProtected **cp)
{
  CellProtected *cursor;
  CellProtected *prev, *nouv_tete;
  int nb_erreur = 0;
  int nb_test = 0;
  float pourcentage_erreur;

  cursor = *cp;
  while (cursor)
  {
    CellProtected *to_del;

    if (verify(cursor->data) == 1)
    { // signature valide
      prev = cursor;
      cursor = cursor->next;
    }
    else
    { // signature invalide
      nb_erreur += 1;

      if (prev)
      { // Si on a un element precedent
        prev->next = cursor->next;
        delete_cell_protected(cursor);
        cursor = prev->next;
      }
      else
      { // pas d'element precedent --> tete de liste
        nouv_tete = cursor->next;
        delete_cell_protected(cursor);
        *cp = nouv_tete;
        cursor = nouv_tete;
      }
    }

    nb_test++;
  }

  pourcentage_erreur = (nb_erreur * 100.0) / (nb_test);
  printf("Nombre de signatures: %d\nNombre de signatures invalides supprimées: %d\nPourcentage d'erreur: %.02f%%\n", nb_test, nb_erreur, pourcentage_erreur);
  return;
}

HashCell *create_hashcell(Key *key)
{
  HashCell *nouv = (HashCell *)malloc(sizeof(HashCell));
  if (!nouv)
    exit(12);

  nouv->key = key;
  nouv->val = 0;

  return nouv;
}

int hash_function(Key *key, int size)
{
  int s = 0;
  char *chaine = key_to_str(key);
  char *cursor = chaine;

  while (*cursor != '\0')
  {
    s += (int)(*cursor);
    cursor++;
  }

  free(chaine);
  return s % size;
}

// retourne la position dans laquelle est ou devrait être key, retourn -1 si plus de place et pas dans la table
int find_position(HashTable *t, Key *key)
{
  HashCell **tab = NULL;
  tab = t->tab;

  int key_position = hash_function(key, t->size);

  for (int i = key_position; i < t->size; i++)
  {
    if (tab[i] == NULL)
    {
      return i;
    }
    else
    {
      // une cle ayant les meme valeur est deja inserer
      if (tab[i]->key->n == key->n && tab[i]->key->val == key->val)
      {
        return i;
      }
    }
  }

  // Si on a pas trouver de position alors le tableau est plein entre key_position et la taille du tableau du coup on réitère de 0 à key_position -1
  for (int i = 0; i < key_position; i++)
  {
    if (tab[i] == NULL)
    {
      return i;
    }
    else
    {
      if (tab[i]->key->n == key->n && tab[i]->key->val == key->val)
      {
        return i;
      }
    }
  }

  // On a pas trouve la cle dans la liste et il n'y pas plus de place disponible
  return -1;
}

HashTable *create_hashtable(CellKey *list_key, int size)
{
  HashTable *res = (HashTable *)malloc(sizeof(HashTable));
  if (!res)
    exit(12);
  res->size = size;
  res->tab = (HashCell **)malloc(sizeof(HashCell(*)) * size);
  if (!res->tab)
    exit(12);

  for (int i = 0; i < size; i++)
  {
    res->tab[i] = NULL;
  }

  int nb_error_insert = 0;

  while (list_key)
  {
    HashCell *nouv = create_hashcell(list_key->data);
    int pos = find_position(res, list_key->data);

    if (pos != -1)
    {
      res->tab[pos] = nouv;
    }
    else
    {
      nb_error_insert += 1;
    }
    list_key = list_key->next;
  }

  printf("Nombre de cles ou l'insertion a echoue: %d (table de hachage pleine)\n", nb_error_insert);

  return res;
}

void delete_hashtable(HashTable *t)
{
  for (int i = 0; i < t->size; i++)
  {
    if (t->tab[i]->key != NULL)
    {
      // liberation cle --> delete list key
      // liberation hashcell
      free(t->tab[i]);
    }
  }
  // liberation tableau de hashcell
  free(t->tab);
  // liberation table de hachage
  free(t);
}

Key *compute_winner(CellProtected *decl, CellKey *candidates, CellKey *voters, int sizeC, int sizeV)
{
  HashTable *hc = create_hashtable(candidates, sizeC);
  HashTable *hv = create_hashtable(voters, sizeV);

  CellProtected *cursor = decl;
  while (cursor)
  {
    Key *pKey_electeur = cursor->data->pKey;
    Key *pKey_cand = str_to_key(cursor->data->declaration_vote);

    int position_candidat = find_position(hc, pKey_cand);
    int position_electeur = find_position(hv, pKey_electeur);
  
    // Verification de la validite du vote (electeur n'a pas deja voter, candidat dans la liste) et comptabilisation du vote
    if (hv->tab[position_electeur]->val == 0 && position_candidat != -1)
    {
      hc->tab[position_candidat]->val += 1;
      hv->tab[position_electeur]->val += 1;
    }

    free(pKey_cand);
    cursor = cursor->next;
  }

  int position_gagnant = 0;
  int nb_vote = hc->tab[0]->val;

  // Affichage du nombres de voix par candidats #TF1 et determinisation du gagnant (on suppose qu'il n'y a pas d'egalite)
  printf("\nRESULTATS DE L'ELECTION\n");
  for (int i = 0; i < sizeC; i++)
  {

    char *str_key = key_to_str(hc->tab[i]->key);

    printf("Cle candidat: %s\tNombre de voix: %d\n", str_key, hc->tab[i]->val);
    free(str_key);

    if (hc->tab[i]->val > nb_vote)
    {
      nb_vote = hc->tab[i]->val;
      position_gagnant = i;
    }
  }
  // // Affichage du gagnant
  Key *res = hc->tab[position_gagnant]->key;
  char *tmp = key_to_str(res);
  printf("\n\t-----WINNER: %s-----\n\n", tmp);
  free(tmp);

  // cleaning
  delete_hashtable(hc);
  delete_hashtable(hv);

  return res;
}
