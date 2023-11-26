#include <string.h>
#include <stdlib.h> 
#include <stdio.h>

char *codeDest(char* mnemonic) {
  char *ret = malloc(sizeof(char) * 3);
  strcpy(ret, "000");

  if (mnemonic == NULL) return ret;
  
  for (int i=0; i<3; i++) {
    switch(mnemonic[i]) {
      case 'A':
        ret[0] = '1';
        break;
      case 'D':
        ret[1] = '1';
        break;
      case 'M':
        ret[2] = '1';
        break;
    }
  }
  return ret;
}

char *codeJump(char *mnemonic) {
  // this sucks so freaking much but I'm too lazy to implement a hash function myself in C.
  // deal with it.
  if (mnemonic == NULL) return NULL;

  if (strcmp(mnemonic, "JGT")) return "001";
  if (strcmp(mnemonic, "JEQ")) return "010";
  if (strcmp(mnemonic, "JGE")) return "011";
  if (strcmp(mnemonic, "JLT")) return "100";
  if (strcmp(mnemonic, "JNE")) return "101";
  if (strcmp(mnemonic, "JLE")) return "110";
  if (strcmp(mnemonic, "JMP")) return "111";

  return NULL;
}

char *codeComp(char *mnemonic) {
  if (mnemonic == NULL) return NULL;

  int mnemonicLen = strlen(mnemonic);

  if (mnemonicLen == 1) {
    // constants or register
    switch (mnemonic[0]) {
      case '0': return "0101010";
      case '1': return "0111111";
      case 'A': return "0110000";
      case 'D': return "0001100";
      case 'M': return "1001100";
    }
  } else if (mnemonicLen == 2) {
    // unary operators
    if (mnemonic[0] == '!') {
      // !X
      if (mnemonic[1] == 'A') return "0110001";
      if (mnemonic[1] == 'D') return "0001101";
      if (mnemonic[1] == 'M') return "1110001";

      fprintf(stderr, "Unknown negation command: %s\n", mnemonic);
      return NULL;

    } else if (mnemonic[0] == '-') {
      // -X
      if (mnemonic[1] == '1') return "0111010";
      if (mnemonic[1] == 'A') return "0110011";
      if (mnemonic[1] == 'D') return "0001111";
      if (mnemonic[1] == 'M') return "1110011";

      fprintf(stderr, "Unknown unary subtractive command: %s\n", mnemonic);
      return NULL;
    } 

    fprintf(stderr, "Unknown unary command: %s\n", mnemonic);
    return NULL;
   
  } else if (mnemonicLen == 3) {
    // binary operators
    if (mnemonic[1] == '+') {
      // X+Y
      if (mnemonic[0] == 'D' && mnemonic[2] == '1') return "0011111";
      if (mnemonic[0] == 'A' && mnemonic[2] == '1') return "0110111";
      if (mnemonic[0] == 'D' && mnemonic[2] == 'A') return "0000010";
      if (mnemonic[0] == 'M' && mnemonic[2] == '1') return "1110111";
      if (mnemonic[0] == 'D' && mnemonic[2] == 'M') return "1000010";

      fprintf(stderr, "Unknown binary Addition command: %s\n", mnemonic);
      return NULL;

    } else if (mnemonic[1] == '-') {
      // X-Y
      if (mnemonic[0] == 'D' && mnemonic[2] == '1') return "0001110";
      if (mnemonic[0] == 'A' && mnemonic[2] == '1') return "0110010";
      if (mnemonic[0] == 'D' && mnemonic[2] == 'A') return "0010011";
      if (mnemonic[0] == 'A' && mnemonic[2] == 'D') return "0000111";
      if (mnemonic[0] == 'M' && mnemonic[2] == '1') return "1110010";
      if (mnemonic[0] == 'D' && mnemonic[2] == 'M') return "1010011";
      if (mnemonic[0] == 'M' && mnemonic[2] == 'D') return "1000111";

      fprintf(stderr, "Unknown binary Subtractive command: %s\n", mnemonic);
      return NULL;

    } else if (mnemonic[1] == '&') {
      // X&Y
      if (mnemonic[0] == 'D' && mnemonic[2] == 'A') return "0000000";
      if (mnemonic[0] == 'D' && mnemonic[2] == 'M') return "1000000";

      fprintf(stderr, "Unknown binary And command: %s\n", mnemonic);
      return NULL;

    } else if (mnemonic[1] == '|') {
      // X|Y
      if (mnemonic[0] == 'D' && mnemonic[2] == 'A') return "0010101";
      if (mnemonic[0] == 'D' && mnemonic[2] == 'M') return "1010101";

      fprintf(stderr, "Unknown binary Or command: %s\n", mnemonic);
      return NULL;

    } else {
      fprintf(stderr, "Unknown binary command: %s\n", mnemonic);
      return NULL;
    } 

  } else {
    fprintf(stderr, "Invalid command length: %s\n", mnemonic);
    return NULL;
  }

  fprintf(stderr, "Failed to convert command: %s\n", mnemonic);
  return NULL;
}

