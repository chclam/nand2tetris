#include <stdio.h>
#include "Parser.h"

char *inputFilePath;

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "No file name provided in the arguments.\n");
    return 1;
  }

  // open input file
  inputFilePath = argv[1];
  Parser *parser = initParser(inputFilePath);
  if (parser == NULL) {
    fprintf(stderr, "Failed to parse file:%s", inputFilePath); 
    exit(1);
  }

  for (; hasMoreCommands(parser); advance(parser)) {
    //printf("%s", parser->currCommand);
   removeComment(parser);
   printf("%s %d\n", arg1(parser), arg2(parser));
  }
  free(parser);
}
