#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "List.h"
#include "Parser.h"
#include "Assembler.h"
#include "CommandTypes.h"

char *inputFileName;

#define BITLEN 15
#define MAX_DECIMAL 32767 // 2 ** 15


char *uIntToBinary(int x) {
  if (x > MAX_DECIMAL)
    return NULL;

  char *ret = malloc(sizeof(char) * BITLEN);
  if (!ret) { 
    fprintf(stderr, "Memory allocation failed\n");
    exit(EXIT_FAILURE);
  } 

  for (int i=0; i<BITLEN; i++) {
    ret[i] = '0';
  }

  int bitIdx = BITLEN-1;
  
  while (x > 0) {
    int rem = x % 2;
    ret[bitIdx] = rem + '0'; // change to char by + '0'
    x /= 2;
    bitIdx--;
  }

  return ret;
}

int isDecimal(char *symb) {
  for (char *s=symb; *s!='\0'; s++) {
    if (!isdigit(*s))
      return 0;
  }
  return 1;
}

void addRamAddress(ListStr *head, int *ramAddress) {
  if (head->next == NULL)
    return;

  addRamAddress(head->next, ramAddress);

  if (!contains(head->val)) {
    addEntry(head->val, (*ramAddress)++);
  }
}

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "No file name provided in the arguments.\n");
    return 1;
  }

  inputFileName = argv[1];

  //
  // Initialize symbol table (haha)
  //
  {
    addEntry("SP",   0);
    addEntry("LCL",  1);
    addEntry("ARG",  2);
    addEntry("THIS", 3);
    addEntry("THAT", 4);
    addEntry("R0" ,  0);
    addEntry("R1" ,  1);
    addEntry("R2" ,  2);
    addEntry("R3" ,  3);
    addEntry("R4" ,  4);
    addEntry("R5" ,  5);
    addEntry("R6" ,  6);
    addEntry("R7" ,  7);
    addEntry("R8" ,  8);
    addEntry("R9" ,  9);
    addEntry("R10", 10);
    addEntry("R11", 11);
    addEntry("R12", 12);
    addEntry("R13", 13);
    addEntry("R14", 14);
    addEntry("R15", 15);
    addEntry("SCREEN", 16384);
    addEntry("KBD"   , 24576);
  }

  //
  // First pass: get all symbols
  //
  {
    Parser *parser = initParser(inputFileName); // first pass parser

    int romAddress = 0;
    ListStr *aCommands = initListStr();

    while (parserHasMoreCommands(parser)) {
      char *symb = parserSymbol(parser);

      int cmdType = parserCommandType(parser);

      if (cmdType == A_COMMAND) {
        if (!isDecimal(symb)) {
          aCommands = appendListStr(aCommands, symb);
        }
        romAddress++;
      } else if (cmdType == C_COMMAND) {
        romAddress++;
      } else if (cmdType == L_COMMAND) {
        addEntry(symb, romAddress);
      } 
      free(symb);

      parserAdvance(parser);
    }

    // add variables in a command to hashmap
    int ramAddress = 16;
    // go through list recursively because the aCommands list starts from the tail
    addRamAddress(aCommands, &ramAddress); 
    free(parser);
  }

  //
  // Second pass: parse all and replace all symbols by their rom or ram address
  //
  {
    // IOs are expensive, so ideally we'd save the buffer somewhere
    // to avoid multiple IO readings. But I'm too lazy to implement this.
    Parser *parser = initParser(inputFileName); 

    while (parserHasMoreCommands(parser)) {

      int cmdType = parserCommandType(parser);

      char cmdOut[17] = "";

      if (cmdType == A_COMMAND) {
        char *symb = parserSymbol(parser);
        assert(symb != NULL);

        cmdOut[0] = '0';

        int symbInt;
        if (isDecimal(symb)) {
          symbInt = atoi(symb);
        } else {
          assert(contains(symb));
          symbInt = getAddress(symb);
        }
        
        // symb is a unsigned decimal number
        char *symbBin = uIntToBinary(symbInt);
        if (symbBin == NULL) {
          fprintf(stderr, 
                  "Symbol integer too large for %d-bit assembler: %d",
                  MAX_DECIMAL, symbInt);
          return 1;
        }

        free(symb);

        strcat(cmdOut, symbBin);
        printf("%s\n", cmdOut);


      } else if (cmdType == C_COMMAND) {
        char *comp = parserComp(parser);
        char *dest = parserDest(parser);
        char *jump = parserJump(parser);

        assert(comp != NULL);
        assert((dest != NULL || jump != NULL) && !(dest != NULL && jump != NULL));

        strcat(cmdOut, "111");

        char *destBin = codeDest(dest);
        char *compBin = codeComp(comp);
        char *jumpBin = codeJump(jump);

        strcat(cmdOut, compBin);

        free(dest);
        free(comp);
        free(jump);

        if (dest != NULL) {
          // comp
          strcat(cmdOut, destBin);
          strcat(cmdOut, "000");

        } else {
          // jump
          strcat(cmdOut, "000");
          strcat(cmdOut, jumpBin);
        }

        printf("%s\n", cmdOut);

      } else if (cmdType == NOT_COMMAND || cmdType == L_COMMAND) {
        // do nothing
      } else {
        fprintf(stderr, "Invalid command type.");
        return 1;
      }

      parserAdvance(parser);
    }
    free(parser);
  }
}


