#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef struct ListStr {
  struct ListStr *next;
  char val[1024];
} ListStr;

ListStr *initListStr();
ListStr *appendListStr(ListStr*, char*);
