#include <stdlib.h>
#include "structs/bvs.h"
#include "structs/stack.h"

#ifndef SYMTABLE_H
#define SYMTABLE_H

typedef enum {U8, I32, F64, NONETYPE} type_t;

typedef struct SymTable_t {
  struct Scope_t *current;
  Stack *stack;
} SymTable;

typedef struct Scope_t {
  BVS *tree;
  struct Scope_t *parent;
} Scope;

// TODO: Add counter of amount of usages (or a boolean whether a symbol was used)

typedef struct {
  char *name;
  type_t type;
  bool decl;
  bool init;
} Symbol;


void SymTable_SetType(SymTable *table, char *name, type_t type);
void SymTable_SetDecl(SymTable *table, char *name, bool isDeclared);
void SymTable_SetInit(SymTable *table, char *name, bool isInit);

type_t Symtable_GetType(SymTable *table, char *name);
bool SymTable_GetDecl(SymTable *table, char *name);
bool SymTable_GetInit(SymTable *table, char *name);

void SymTable_Init(SymTable *table);
void SymTable_AddSymbol(SymTable *table, Symbol *symbol);
void SymTable_NewScope(SymTable *table); //Use this function to jump into a new sub-scope
void Symtable_Dispose(SymTable *table); //"delete root" algorithm

#endif
