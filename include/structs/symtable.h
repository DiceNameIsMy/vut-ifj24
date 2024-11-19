#include <stdlib.h>
#include "structs/bvs.h"

typedef enum {U8, I32, F64, NONETYPE} type_t;
typedef enum {LOCAL, GLOBAL, UNDEFINED} scope_t;

typedef struct SymTable_t {
  BVS *tree;
  SymTable_t *parent;
} SymTable;

typedef struct {
  char *name;
  type_t type;
  bool decl;
  bool init;
  scope_t scope;
} Symbol;


void SymTable_SetType(SymTable *table, char *name, type_t type);
void SymTable_SetDecl(SymTable *table, char *name, bool isDeclared);
void SymTable_SetInit(SymTable *table, char *name, bool isInit);
void Symtable_SetScope(SymTable *table, char *name, scope_t scope);

type_t Symtable_GetType(SymTable *table, char *name);
bool SymTable_GetDecl(SymTable *table, char *name);
bool SymTable_GetInit(SymTable *table, char *name);
scope_t SymTable_GetScope(SymTable *table, char *name);

void SymTable_Init(SymTable *table);
void SymTable_AddSymbol(SymTable *table, Symbol *symbol);
SymTable *SymTable_NewScope(SymTable *table); //I needa think how this will even work. By now (17.10.2024) I'm a bit confused.
void Symtable_Dispose(SymTable *table); //"delete root" algorithm
