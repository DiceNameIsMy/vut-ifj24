#include <stdlib.h>
#include "structs/bvs.h"
#include "structs/stack.h"

#ifndef SYMTABLE_H
#define SYMTABLE_H

typedef enum {U8_LITERAL, U8_ARRAY, I32, F64, U8_ARRAY_NULLABLE, 
              I32_NULLABLE, F64_NULLABLE, BOOL, FUNCTION, NONETYPE} type_t;

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
  bool mut;
  bool init;
  type_t retType;
  struct param_t *paramList;
} Symbol;

typedef struct param_t {
  type_t paramType;
  struct param_t *next;
} Param;


Symbol *SymTable_Search(SymTable *table, char *name);
void SymTable_SetType(SymTable *table, char *name, type_t type);
void SymTable_SetMut(SymTable *table, char *name, bool isMutable);
void SymTable_SetInit(SymTable *table, char *name, bool isInit);
void SymTable_PushFuncParam(SymTable *table, char *name, type_t paramType);
void SymTable_SetRetType(SymTable *table, char *name, type_t retType);

type_t SymTable_GetType(SymTable *table, char *name);
bool SymTable_GetMut(SymTable *table, char *name);
bool SymTable_GetInit(SymTable *table, char *name);
type_t SymTable_GetRetType(SymTable *table, char *name);
Param *SymTable_GetParamList(SymTable *table, char *name);

void SymTable_Init(SymTable *table);
void SymTable_AddSymbol(SymTable *table, Symbol *symbol);
void SymTable_NewScope(SymTable *table); //Use this function to jump into a new sub-scope
void SymTable_UpperScope(SymTable *table);
void Symtable_Dispose(SymTable *table); //"delete root" algorithm

#endif
