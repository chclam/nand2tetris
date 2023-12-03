typedef struct nlist {
  struct nlist *next;
  char *name;
  int defn;
} nlist;

nlist *install(char*, int);
nlist *lookup(char*);
