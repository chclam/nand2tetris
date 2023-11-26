#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "CommandTypes.h"

int currLineNumber = 0;
char currCommand[1024];

const char *VALID_COMPS[] = {
  "0"  , "1"  , "-1" ,
  "D"  , "A"  , "M"  ,
  "!D" , "!A" , "!M" ,
  "-D" , "-A" , "-M" ,
  "D+1", "A+1", "M+1",
  "D-1", "A-1", "M-1",
  "D+A", "D+M", "D-A",
  "A-D", "D&A", "D|A",
  "D-M", "M-D", "D&M",
  "D|M"
}; 
const int VALID_COMPS_LEN = sizeof(VALID_COMPS) * sizeof(char*);

const char *VALID_JUMPS[] = {
  "JGT", "JEQ", "JGE",
  "JLT", "JNE", "JLE",
  "JMP"
}; 
const int VALID_JUMPS_LEN = sizeof(VALID_JUMPS) * sizeof(char*);

int parserHasMoreCommands(FILE *ifp) {
  /* Are there more commands in the input? */
  return !feof(ifp);
}

void parserAdvance(FILE *ifp) {
  /* Reads the next command from the input and makes it the current command.
  Should be called only if hasMoreCommands() is true.
  Initially there is no current command. */
  if (!parserHasMoreCommands(ifp)) return;

  fgets(currCommand, 1024, ifp);

  // truncate currCommand until comments
  char *commentPtr = strstr(currCommand, "//");
  if (commentPtr) *commentPtr = '\0';

  currLineNumber++;
}

int parserCommandType() {
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

char *parserSymbol() {
  int cmdType = parserCommandType();

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

char *parserDest() {
  /* Returns the dest mnemonic in the current c-command (8 possiblities).
  Should be called only when commandType() is C_COMMAND. */
  if (parserCommandType() != C_COMMAND) return NULL;

  char *equalsPtr = strchr(currCommand, '=');
  if (!equalsPtr) return NULL;

  char *destMnemonic = currCommand;
  while (isspace(*destMnemonic)) destMnemonic++; // trim spaces, tabs etc left
  int destLen = (int)(equalsPtr - destMnemonic);

  char *ret = malloc(sizeof(char)*destLen);
  if (!ret) {
    fprintf(stderr, "Failed to allocate memory at line %d\n", currLineNumber);
    return NULL;
  }
  strncpy(ret, destMnemonic, destLen);
  
  if ((strstr("AMD", ret) == NULL) && (strstr("AD", ret) == NULL)) {
    fprintf(stderr, "Invalid dest for C Command at line %d: %s\n", currLineNumber, currCommand);
    exit(1);
  }

  return ret;
}

char *parserComp() {
  /* Returns the comp mnemonic in the current C-command (28 possibilities).
  Should be called only when commandType() is C_COMMAND.

  Format of a C_COMMAND: dest=comp;jump */
  if (parserCommandType() != C_COMMAND) return NULL;

  // '=' XOR ';' must be in currCommand since this is checked in commandType()
  char *eqPtr = strchr(currCommand, '=');
  char *semiColonPtr = strchr(currCommand, ';');

  char *ret;

  if (eqPtr) {
    char *compPtr = eqPtr + 1;
    while (isspace(*compPtr)) compPtr++; // trim off whitespace left
    int compLen = 0;
    while (!isspace(compPtr[compLen])) compLen++; // get the length of the comp until spaces

    ret = malloc(sizeof(char)*compLen);
    strncpy(ret, compPtr, compLen);

  } else {
    char *compPtr = currCommand;

    while (isspace(*compPtr)) compPtr++; // trim off whitespace left

    int compLen = (int)(semiColonPtr - compPtr);
    ret = malloc(sizeof(char)*compLen);
    strncpy(ret, compPtr, compLen);
  }

  for (int i=0; i<VALID_COMPS_LEN; i++) {
    if (strcmp(ret, VALID_COMPS[i]) == 0) return ret;
  }
  return NULL;
}

char *parserJump() {
  /* Returns the jump mnemonic in the current C-command
  (8 possilibities)
  */
  if (parserCommandType() != C_COMMAND) return NULL;

  char *semiColonPtr = strchr(currCommand, ';');
  if (semiColonPtr == NULL) return NULL;

  char *jumpPtr = semiColonPtr + 1;
  int jumpLen = 0;
  while (!isspace(jumpPtr[jumpLen])) jumpLen++;
  
  char *ret = malloc(sizeof(char)*jumpLen);
  strncpy(ret, jumpPtr, jumpLen);

  for (int i=0; i<VALID_JUMPS_LEN; i++) {
    if (strcmp(ret, VALID_JUMPS[i]) == 0) return ret;
  }
  return NULL;
}


