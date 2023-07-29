#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define A_COMMAND 0;
#define C_COMMAND 1;
#define L_COMMAND 2;

FILE *ifp;
char *inputFileName;

int currLineNumber = 0;
char currCommand[1024];

int hasMoreCommands() {
  /* Are there more commands in the input? */
  return !feof(ifp);
}

void advance() {
  /* Reads the next command from the input and makes it the current command.
  Should be called only if hasMoreCommands() is true.
  Initially there is no current command. */
  if (!hasMoreCommands()) return;
  fgets(currCommand, 1024, ifp);
  currLineNumber++;
}

int commandType() {
  /* Returns the type of current command.
  return A_COMMAND for @Xxx where Xxx is either a symbol or a decimal number;
  return C_COMMAND for dest=comp;jump
  return L_COMMAND (Actually pseudo command)(Xxx) where Xxx is a symbol. */
  int isA = strchr(currCommand, '@') != NULL;
  int isC = strchr(currCommand, '=') != NULL;
  int isL = strchr(currCommand, '(') != NULL && strchr(currCommand, ';') != NULL;

  if (isA + isC + isL > 1) {
    fprintf(stderr, "Invalid command found at line %d: %s", currLineNumber, currCommand);
    exit(1);
  }

  if (isA) return A_COMMAND;
  if (isC) return C_COMMAND;
  if (isL) return L_COMMAND;
  return -1;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "No file name provided in the arguments.\n");
    return 1;
  }

  // open input file
  ifp = fopen(argv[1], "r");
  if (ifp == NULL) {
    fprintf(stderr, "File %s not found.\n", argv[1]); 
    return 1;
  } 

  inputFileName = argv[1];
}

