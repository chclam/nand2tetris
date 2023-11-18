#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define A_COMMAND    0
#define C_COMMAND    1
#define L_COMMAND    2
#define NOT_COMMAND -1

FILE *ifp;
char *inputFileName;

int currLineNumber = 0;
char currCommand[1024];

char *validComps[] = {
  "0", "1", "-1", "D", "A", "!D",
  "!A", "-D", "-A", "D+1", "A+1", "D-1",
  "A-1", "D+A", "D-A", "A-D", "D&A", "D|A",
  "M", "!M", "-M", "M+1", "M-1", "D+M", "D-M",
  "M-D", "D&M", "D|M"
}; // @speed

char *validJumps[] = {
  "JGT",
  "JEQ",
  "JGE",
  "JLT",
  "JNE",
  "JLE",
  "JMP"
}; // @speed

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

  // truncate currCommand until comments
  char *commentPtr = strstr(currCommand, "//");
  if (commentPtr != NULL) *commentPtr = '\0';

  currLineNumber++;
}

int commandType() {
  /* Returns the type of current command.
  return A_COMMAND for @Xxx where Xxx is either a symbol or a decimal number;
  return C_COMMAND for dest=comp;jump
  return L_COMMAND (Actually pseudo command)(Xxx) where Xxx is a symbol. */

  char *atPtr = strchr(currCommand, '@');
  int isA = (atPtr != NULL);

  char *eqPtr = strchr(currCommand, '=');
  char *semiColonPtr = strchr(currCommand, ';');
  int isC = (eqPtr != NULL) || (semiColonPtr != NULL);
  if ((eqPtr != NULL) && (semiColonPtr != NULL)) {
    fprintf(stderr, "Unambiguous C Command in line %d: %s\n", currLineNumber, currCommand);
    exit(1);
  }

  char *leftBrPtr = strchr(currCommand, '(');
  char *rightBrPtr = strchr(currCommand, ')');
  int isL = (leftBrPtr != NULL) && (rightBrPtr != NULL) && (leftBrPtr < rightBrPtr); // Check if "(" occurs before ")"
  if ((leftBrPtr != NULL) != (rightBrPtr != NULL)) {
    fprintf(stderr, "Invalid brackets for L Command in line %d: %s\n", currLineNumber, currCommand);
    exit(1);
  }

  if (isA + isC + isL > 1) {
    // One line can only consist of one command in this assembly language!
    fprintf(stderr, "Invalid command found in line %d: %s\n", currLineNumber, currCommand);
    exit(1);
  }

  if (isA) return A_COMMAND;
  if (isC) return C_COMMAND;
  if (isL) return L_COMMAND;
  return NOT_COMMAND;
}

char *symbol() {
  int cmdType = commandType();

  if (!(cmdType == A_COMMAND || cmdType == L_COMMAND)) return NULL;

  char *allowed = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890_.$:";

  if (cmdType == A_COMMAND) {
    char *atPtr = strchr(currCommand, '@');
    char *symbolPtr = atPtr + 1;
    int symbolLen = strspn(symbolPtr, allowed);

    char *ret = malloc(sizeof(char)*symbolLen);
    strncpy(ret, symbolPtr, symbolLen);
    return ret;

  } else {
    char *leftBrPtr = strchr(currCommand, '(');
    char *rightBrPtr = strchr(currCommand, ')');

    char *symbolPtr = leftBrPtr + 1;
    int symbolLen = (int)(rightBrPtr - leftBrPtr - 1);

    if (strspn(symbolPtr, allowed) < symbolLen) {
      fprintf(stderr, "Invalid symbol for L COMMAND at line %d: %s\n", currLineNumber, currCommand);
      exit(1);
    }

    char *ret = malloc(sizeof(char)*symbolLen);
    strncpy(ret, symbolPtr, symbolLen);

    return ret;
  } 
}

char *dest() {
  /* Returns the dest mnemonic in the current c-command (8 possiblities).
  Should be called only when commandType() is C_COMMAND. */
  if (commandType() != C_COMMAND) return NULL;

  char *equalsPtr = strchr(currCommand, '=');
  if (equalsPtr == NULL) return NULL;

  char *destMnemonic = currCommand;
  while (isspace(*destMnemonic)) destMnemonic++; // trim spaces, tabs etc left
  int destLen = (int)(equalsPtr - destMnemonic);

  char *ret = malloc(sizeof(char)*destLen);
  strncpy(ret, destMnemonic, destLen);
  
  if ((strstr("AMD", ret) == NULL) && (strstr("AD", ret) == NULL)) {
    fprintf(stderr, "Invalid dest for C Command at line %d: %s\n", currLineNumber, currCommand);
    exit(1);
  }

  return ret;
}

char *comp() {
  /* Returns the comp mnemonic in the current C-command (28 possibilities).
  Should be called only when commandType() is C_COMMAND.

  Format of a C_COMMAND: dest=comp;jump */
  if (commandType() != C_COMMAND) return NULL;

  // '=' XOR ';' must be in currCommand since this is checked in commandType()
  char *eqPtr = strchr(currCommand, '=');
  char *semiColonPtr = strchr(currCommand, ';');

  char *ret;

  if (eqPtr != NULL) {
    char *compPtr = eqPtr + 1;
    while (isspace(*compPtr)) compPtr++; // trim off whitespace left
    int compLen = 0;
    while (!isspace(compPtr[compLen])) compLen++;

    ret = malloc(sizeof(char)*strlen(compPtr));
    strncpy(ret, compPtr, compLen);

  } else {
    char *compPtr = currCommand;

    while (isspace(*compPtr)) compPtr++; // trim off whitespace left

    int compLen = (int)(semiColonPtr - compPtr);
    ret = malloc(sizeof(char)*compLen);
    strncpy(ret, compPtr, compLen);
  }

  int validCompsLen = sizeof(validComps) / sizeof(char*);
  for (int i=0; i<validCompsLen; i++) {
    if (strcmp(ret, validComps[i]) == 0) return ret;
  }
  return NULL;
}

char *jump() {
  /* Returns the jump mnemonic in the current C-command
  (8 possilibities)
  */
  if (commandType() != C_COMMAND) return NULL;

  char *semiColonPtr = strchr(currCommand, ';');
  if (semiColonPtr == NULL) return NULL;

  char *jumpPtr = semiColonPtr + 1;
  int jumpLen = 0;
  while (!isspace(jumpPtr[jumpLen])) jumpLen++;
  
  char *ret = malloc(sizeof(char)*jumpLen);
  strncpy(ret, jumpPtr, jumpLen);

  int validJumpsLen = sizeof(validJumps)/sizeof(char*);
  for (int i=0; i<validJumpsLen; i++) {
    if (strcmp(ret, validJumps[i]) == 0) return ret;
  }
  return NULL;
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
    char *sym = symbol();
    char *destMnem = dest();
    char *compCom = comp();
    char *jumpCom = jump();

    printf("-------\n");
    printf("%s", currCommand);
    printf("-------\n");
    printf("SYMB %s\n", sym);
    printf("DEST %s\n", destMnem);
    printf("COMP %s\n", compCom);
    printf("JUMP %s\n", jumpCom);
    printf("\n");

    free(sym);
    free(destMnem);
    free(compCom);
    free(jumpCom);

    advance();
  }
}

