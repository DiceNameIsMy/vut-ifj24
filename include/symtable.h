#include <stdlib.h>
#include "bvs.h"
#include "stack.h"
#include "types.h"

#ifndef SYMTABLE_H
#define SYMTABLE_H

typedef struct SymTable_t {
  struct Scope_t *current;
  Stack *stack;
} SymTable;

typedef struct Scope_t {
  BVS *tree;
  struct Scope_t *parent;
  int unused_cnt;
} Scope;

// TODO: Add counter of amount of usages (or a boolean whether a symbol was used)

typedef struct {
  char *name;
  type_t type;
  bool mut;
  bool used;
  type_t retType;
  struct param_t *paramList;
} Symbol;

typedef struct param_t {
  type_t paramType;
  char *name;
  struct param_t *next;
} Param;


Symbol *SymTable_Search(SymTable *table, char *name);
void SymTable_SetType(SymTable *table, char *name, type_t type);
void SymTable_SetMut(SymTable *table, char *name, bool isMutable);
void SymTable_SetUsed(SymTable *table, char *name, bool isUsed);

/// @brief 
/// @param table 
/// @param name 
/// @param paramType 
/// @param paramName Can be set to NULL if pushing a built-in function parameter.
void SymTable_PushFuncParam(SymTable *table, char *name, type_t paramType, char *paramName);
void SymTable_SetRetType(SymTable *table, char *name, type_t retType);

type_t SymTable_GetType(SymTable *table, char *name);
bool SymTable_GetMut(SymTable *table, char *name);
bool SymTable_GetUsed(SymTable *table, char *name);
type_t SymTable_GetRetType(SymTable *table, char *name);
Param *SymTable_GetParamList(SymTable *table, char *name);

void SymTable_Init(SymTable *table);
void SymTable_AddSymbol(SymTable *table, Symbol *symbol);
void SymTable_NewScope(SymTable *table); //Use this function to jump into a new sub-scope
int SymTable_UpperScope(SymTable *table); //returns count of unused local vars
void Symtable_Dispose(SymTable *table); //"delete root" algorithm

#endif
