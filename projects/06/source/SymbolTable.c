#include <stdio.h>
#include "Hash.h"

void addEntry(char *symbol, int address) {
  install(symbol, address);
}

int contains(char *symbol) {
  if (lookup(symbol) != NULL) {
    return 1;
  }
  return 0;
}

int getAddress(char *symbol) {
  nlist *np;
  if((np=lookup(symbol)) != NULL) {
    return np->defn;
  }
  return -1;
}

