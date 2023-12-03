#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Hash.h"

#define HASHSIZE 101

// Implementation from: C Programming language by Kernighan & Ritchie.
// WITH CHANGES to defn :str to defn: int, i.e. make the hash store int.


static nlist *hashtab[HASHSIZE];

unsigned hash(char *s) {
  unsigned hashval;
  for (hashval=0; *s != '\0'; s++) {
    hashval = *s + 31 * hashval;
  }
  return hashval % HASHSIZE;
}

nlist *lookup(char *s) {
  nlist *np;
  unsigned hashval = hash(s);

  for (np=hashtab[hashval]; np != NULL; np=np->next) {
    if (strcmp(s, np->name) == 0) return np;
  }
  return NULL;
}

nlist *install(char *name, int defn) {
  nlist *np;
  unsigned hashval;

  if ((np=lookup(name)) == NULL) {
    np = (nlist*) malloc(sizeof(*np));

    if (np == NULL || (np->name=strdup(name)) == NULL) return NULL;

    hashval = hash(name);
    np->next = hashtab[hashval];
    hashtab[hashval] = np;
  }

  np->defn=defn;

  return np;
}
