#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#include "Parser.h"
#include "CommandTypes.h"

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

Parser *initParser(char *inputFileName) {
  Parser *ret = malloc(sizeof(Parser));
  if (ret != NULL) {
    ret->ifp = fopen(inputFileName, "r");
    ret->currLineNumber = 0;
    ret->currCommand[0] = '\0';
  }

  if (!ret->ifp) {
    fprintf(stderr, "File %s not found.\n", inputFileName); 
    exit(1);
  } 

  return ret;
}

int parserHasMoreCommands(Parser *parser) {
  /* Are there more commands in the input? */
  return !feof(parser->ifp);
}

void parserAdvance(Parser *parser) {
  /* Reads the next command from the input and makes it the current command.
  Should be called only if hasMoreCommands() is true.
  Initially there is no current command. */
  if (!parserHasMoreCommands(parser)) return;

  fgets(parser->currCommand, 1024, parser->ifp);

  // truncate currCommand until comments
  char *commentPtr = strstr(parser->currCommand, "//");
  if (commentPtr) *commentPtr = '\0';

  parser->currLineNumber++;
}

int parserCommandType(Parser *parser) {
  /* Returns the type of current command.
  return A_COMMAND for @Xxx where Xxx is either a symbol or a decimal number;
  return C_COMMAND for dest=comp;jump
  return L_COMMAND (Actually pseudo command)(Xxx) where Xxx is a symbol. */

  char *atPtr = strchr(parser->currCommand, '@');
  int isA = (atPtr != NULL);

  char *eqPtr = strchr(parser->currCommand, '=');
  char *semiColonPtr = strchr(parser->currCommand, ';');
  int isC = (eqPtr != NULL) || (semiColonPtr != NULL);
  if ((eqPtr != NULL) && (semiColonPtr != NULL)) {
    fprintf(stderr, "Unambiguous C Command in line %d: %s\n", parser->currLineNumber, parser->currCommand);
    exit(1);
  }

  char *leftBrPtr = strchr(parser->currCommand, '(');
  char *rightBrPtr = strchr(parser->currCommand, ')');
  int isL = (leftBrPtr != NULL) && (rightBrPtr != NULL) && (leftBrPtr < rightBrPtr); // Check if "(" occurs before ")"
  if ((leftBrPtr != NULL) != (rightBrPtr != NULL)) {
    fprintf(stderr, "Invalid brackets for L Command in line %d: %s\n", parser->currLineNumber, parser->currCommand);
    exit(1);
  }

  if (isA + isC + isL > 1) {
    // One line can only consist of one command in this assembly language!
    fprintf(stderr, "Invalid command found in line %d: %s\n", parser->currLineNumber, parser->currCommand);
    exit(1);
  }

  if (isA) return A_COMMAND;
  if (isC) return C_COMMAND;
  if (isL) return L_COMMAND;
  return NOT_COMMAND;
}

char *parserSymbol(Parser *parser) {
  int cmdType = parserCommandType(parser);

  if (!(cmdType == A_COMMAND || cmdType == L_COMMAND)) return NULL;

  char *allowed = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890_.$:";

  if (cmdType == A_COMMAND) {
    char *atPtr = strchr(parser->currCommand, '@');
    char *symbolPtr = atPtr + 1;
    int symbolLen = strspn(symbolPtr, allowed);

    char *symb = malloc(sizeof(char)*symbolLen+1);
    strncpy(symb, symbolPtr, symbolLen);
    symb[symbolLen] = '\0';

    // validity checks
    int isDecimal = 1;
    {
      for (char *s=symb; *s!='\0'; s++) {
        if (!isdigit(*s))
          isDecimal = 0;
     }

      if (isDecimal) {
        if (strlen(symb) > 1 && symb[0] == 0) {
          fprintf(stderr, "Leading 0s found in decimal number in line %d: %s", parser->currLineNumber, parser->currCommand);
          exit(1);
        }

        if (atoi(symb) > 32768) {
          fprintf(stderr, "Decimal greater than allowed by bit length of 32768 at line %d: %s", parser->currLineNumber, parser->currCommand);
          exit(1);
        }

      } else {
        if (isdigit(symb[0])) {
          fprintf(stderr, "Invalid symbol for C-COMMAND found at line %d. Symbols must not start with a digit. %s",
                  parser->currLineNumber,
                  parser->currCommand);
          exit(1);
        }
      }
    }

    return symb;

  } else {
    char *leftBrPtr = strchr(parser->currCommand, '(');
    char *rightBrPtr = strchr(parser->currCommand, ')');

    char *symbolPtr = leftBrPtr + 1;
    int symbolLen = (int)(rightBrPtr - leftBrPtr - 1);

    if (strspn(symbolPtr, allowed) < symbolLen) {
      fprintf(stderr, "Invalid symbol for L COMMAND at line %d: %s\n", parser->currLineNumber, parser->currCommand);
      exit(1);
    }

    char *symb = malloc(sizeof(char)*symbolLen);
    strncpy(symb, symbolPtr, symbolLen);

    return symb;
  } 
}

char *parserDest(Parser *parser) {
  /* Returns the dest mnemonic in the current c-command (8 possiblities).
  Should be called only when commandType() is C_COMMAND. */
  if (parserCommandType(parser) != C_COMMAND) return NULL;

  char *equalsPtr = strchr(parser->currCommand, '=');
  if (!equalsPtr) return NULL;

  char *destMnemonic = parser->currCommand;
  while (isspace(*destMnemonic)) destMnemonic++; // trim spaces, tabs etc left
  int destLen = (int)(equalsPtr - destMnemonic);

  char *ret = malloc(sizeof(char)*destLen);
  if (!ret) {
    fprintf(stderr, "Failed to allocate memory at line %d\n", parser->currLineNumber);
    return NULL;
  }
  strncpy(ret, destMnemonic, destLen);
  
  if ((strstr("AMD", ret) == NULL) && (strstr("AD", ret) == NULL)) {
    fprintf(stderr, "Invalid dest for C Command at line %d: %s\n", parser->currLineNumber, parser->currCommand);
    exit(1);
  }

  return ret;
}

char *parserComp(Parser *parser) {
  /* Returns the comp mnemonic in the current C-command (28 possibilities).
  Should be called only when commandType() is C_COMMAND.

  Format of a C_COMMAND: dest=comp;jump */
  if (parserCommandType(parser) != C_COMMAND) return NULL;

  // '=' XOR ';' must be in currCommand since this is checked in commandType()
  char *eqPtr = strchr(parser->currCommand, '=');
  char *semiColonPtr = strchr(parser->currCommand, ';');

  char *ret;

  if (eqPtr) {
    char *compPtr = eqPtr + 1;
    while (isspace(*compPtr)) compPtr++; // trim off whitespace left
    int compLen = 0;
    while (!isspace(compPtr[compLen])) compLen++; // get the length of the comp until spaces

    ret = malloc(sizeof(char)*compLen);
    strncpy(ret, compPtr, compLen);

  } else {
    char *compPtr = parser->currCommand;

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

char *parserJump(Parser *parser) {
  /* Returns the jump mnemonic in the current C-command
  (8 possilibities)
  */
  if (parserCommandType(parser) != C_COMMAND) return NULL;

  char *semiColonPtr = strchr(parser->currCommand, ';');
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



