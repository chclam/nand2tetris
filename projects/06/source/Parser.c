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

  char *commentPtr = strstr(currCommand, "//");
  if (commentPtr == NULL) commentPtr = (currCommand + strlen(currCommand) - 1);

  char *atPtr = strchr(currCommand, '@');
  int isA = (atPtr != NULL) && (atPtr < commentPtr);

  char *eqPtr = strchr(currCommand, '=');
  int isC = (eqPtr != NULL) && (eqPtr < commentPtr);

  char *leftBrPtr = strchr(currCommand, '(');
  char *rightBrPtr = strchr(currCommand, ')');
  int isL = (leftBrPtr < commentPtr) && (rightBrPtr < commentPtr);
  isL = isL && (leftBrPtr != NULL && rightBrPtr != NULL);

  if ((leftBrPtr != NULL) != (rightBrPtr != NULL)) {
    fprintf(stderr, "Unmatched brackets for L Command in line %d: %s", currLineNumber, currCommand);
  }

  if (isA + isC + isL > 1) {
    // One line can only consist of one command in this assembly language!
    fprintf(stderr, "Invalid command found in line %d: %s", currLineNumber, currCommand);
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
  while (hasMoreCommands()) {
    int cmdType = commandType();
    printf("%d", cmdType);
    advance();
  }
}

