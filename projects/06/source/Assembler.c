#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include "Assembler.h"
#include "CommandTypes.h"

FILE *ifp;
char *inputFileName;

int SYMB_BIT_LEN = 15;  // max value in an A-command (2^15)

char *uIntToBinary(int x, int bitLen) {

  if (x > pow(2, bitLen)) return NULL;

  char *ret = malloc(sizeof(char) * bitLen);
  if (!ret) { 
    fprintf(stderr, "Memory allocation failed\n");
    exit(EXIT_FAILURE);
  } 
  for (int i=0; i<bitLen; i++) ret[i] = '0';

  int bitIdx = bitLen-1;
  
  while (x > 0) {
    int rem = x % 2;
    ret[bitIdx] = rem + '0'; // change to char by + '0'
    x /= 2;
    bitIdx--;
  }

  return ret;
}


int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "No file name provided in the arguments.\n");
    return 1;
  }

  // open input file
  ifp = fopen(argv[1], "r");
  if (!ifp) {
    fprintf(stderr, "File %s not found.\n", argv[1]); 
    return 1;
  } 

  inputFileName = argv[1];
  while (parserHasMoreCommands(ifp)) {

    int cmdType = parserCommandType();

    char cmdOut[17] = "";

    if (cmdType == A_COMMAND) {
      char *symb = parserSymbol();
      assert(symb != NULL);

      cmdOut[0] = '0';
      
      // symb is a unsigned decimal number
      int symbInt = atoi(symb);
      char *valBin = uIntToBinary(symbInt, SYMB_BIT_LEN);
      if (valBin == NULL) {
        fprintf(stderr, 
                "Symbol integer too large for %d-bit assembler: %d",
                SYMB_BIT_LEN, symbInt);
        return 1;
      }

      free(symb);

      strcat(cmdOut, valBin);
      printf("%s\n", cmdOut);

    } else if (cmdType == L_COMMAND) {
      char *symb = parserSymbol();
      assert(symb != NULL);
      printf("(%s)\n", parserSymbol());

    } else if (cmdType == C_COMMAND) {
      char *comp = parserComp();
      char *dest = parserDest();
      char *jump = parserJump();

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


    } else if (cmdType == NOT_COMMAND) {
      // do nothing
    } else {
      fprintf(stderr, "Invalid command type.");
      return 1;
    }

    parserAdvance(ifp);
  }
}
