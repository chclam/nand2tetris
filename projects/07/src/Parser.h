#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "CommandTypes.h"
#include <assert.h>

#define BUFFER_SIZE 1024

typedef struct Parser {
  FILE *ifp;
  int currLineNumber;
  char currCommand[BUFFER_SIZE];
} Parser;

Parser *initParser(char*);
int hasMoreCommands(Parser*);
void advance(Parser*);
int commandType(Parser*);
char *arg1(Parser*);
int arg2(Parser*);
void removeComment(Parser*);
