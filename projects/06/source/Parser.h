typedef struct Parser {
  FILE *ifp;
  int currLineNumber;
  char currCommand[1024];
} Parser;

Parser *initParser(char*);
void parserAdvance(Parser*);
int  parserHasMoreCommands(Parser*);
int  parserCommandType(Parser*);
char *parserSymbol(Parser*);
char *parserDest(Parser*);
char *parserComp(Parser*);
char *parserJump(Parser*);
