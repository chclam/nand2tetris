#include "Parser.h"
#include "ValidCommands.h"

Parser *initParser(char *filePath) {
  FILE *ifp = fopen(filePath, "r");
  if (!ifp) {
    fprintf(stderr, "File %s not found.\n", filePath); 
    return NULL;
  }

  Parser *ret = malloc(sizeof(Parser));
  if (ret == NULL) {
    fprintf(stderr, "Failed to allocate memory for parser."); 
  } else {
    ret->ifp=ifp;
    ret->currLineNumber = 0;
  }
  return ret;
}

int hasMoreCommands(Parser *parser) {
  /*
  Are there more commands in the input?
  */
  return !feof(parser->ifp);
}

void advance(Parser *parser) {
  /*
  Reads the next command from the input and makes it the current command.
  Should be called only if hasMoreCommands() is true. Initially there is no current command.
  */
  fgets(parser->currCommand, BUFFER_SIZE, parser->ifp);
  if (strlen(parser->currCommand) > BUFFER_SIZE) {
    fprintf(stderr, "Command longer than buffer size in line: %d", parser->currLineNumber);
  }
  parser->currLineNumber++;
}

void removeComment(Parser *parser) {
  // remove comments
  char *commentPtr = strstr(parser->currCommand, "//");
  if (commentPtr != NULL) {
    *commentPtr = '\0';
  }
}

int commandType(Parser *parser) {
  /*
  Returns the type of the current VM command.
  C_ARITHMETIC is returned for all the arithmetic commands.

  Return a command type as defined in "CommandTypes.h".
  Return NULL if current line is not a command, e.g. a comment.
  */
  char *curr[strlen(parser->currCommand)];
  strcpy(curr, parser->currCommand);

  // and that there are no comments.
  assert(strstr(curr, "//") == NULL);

  char *token = strtok(curr, " ");
  if (token == NULL) return C_NOT_A_COMMAND;

  // loop through arithmetic commands
  for (int i=0; i < sizeof(arithmeticCommands) / sizeof(char**); i++) {
    if (strcmp(token, arithmeticCommands[i]) == 0) {
      return C_ARITHMETIC;
    }
  }

  if (strcmp(token, "push") == 0){
    return C_PUSH;
  } else if (strcmp(token, "pop") == 0) {
    return C_POP;
  }

  return C_NOT_A_COMMAND;
}

char *arg1(Parser *parser) {
  /*
  Returns the first argument of the current command.
  In the case of C_ARITHMETIC, the command itself (add, sub, etc.) is returned.
  Should not be called if the current command is C_RETURN.
  */
  int cmdType = commandType(parser);

  if (cmdType == C_RETURN || cmdType == C_NOT_A_COMMAND) {
    return NULL;
  }

  char *curr = malloc(strlen(parser->currCommand) * sizeof(char));
  strcpy(curr, parser->currCommand);

  if (cmdType == C_ARITHMETIC) {
    return curr;
  } else {
    char *ret = strtok(curr, " ");
    ret = strtok(NULL, " ");
    return ret;
  }
}

int arg2(Parser *parser) {
  /*
  Returns the second argument of the current command.
  Should be called only if the current command is C_PUSH, C_POP, C_FUNCTION, or C_CALL.
  */
  int cmdType = commandType(parser);

  if (!(cmdType == C_PUSH || cmdType == C_POP || cmdType == C_FUNCTION || cmdType == C_CALL)) {
    return NULL;
  }

  char *curr[strlen(parser->currCommand)];
  strcpy(curr, parser->currCommand);

  char *token = strtok(curr, " ");
  token = strtok(NULL, " ");
  token = strtok(NULL, " ");
  return atoi(token);
}

