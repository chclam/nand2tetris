
// Parser.c
int  parserHasMoreCommands(FILE*);
void parserAdvance(FILE*);
int  parserCommandType(void);
char *parserSymbol(void);
char *parserDest(void);
char *parserComp(void);
char *parserJump(void);

// Code.c
char *codeDest(char*);
char *codeJump(char*); 
char *codeComp(char*);
