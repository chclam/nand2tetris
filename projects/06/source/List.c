#include "List.h"

ListStr *initListStr(void) {
  ListStr *ret = malloc(sizeof(ListStr));
  ret->next = NULL;
  *(ret->val) = '\0';
  return ret;
}

ListStr *appendListStr(ListStr *node, char *val) {
  assert(strlen(val) <= 1024);

  ListStr *ret;

  if (*(node->val) == '\0') {
    strcpy(node->val, val);
    ret = node;
  }  else {
    ret = malloc(sizeof(ListStr));
    ret->next = node;
    strcpy(ret->val, val);
  }

  return ret;
}

unsigned listLen(ListStr *node) {
  unsigned ret = 0;
  ListStr *curr = node;
  while (curr != NULL) {
    ret++;
    curr = node->next;
  }
  return ret;
}
