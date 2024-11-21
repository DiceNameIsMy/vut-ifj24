//
//created by GladiatorEntered on 18/11/24
//


#include "structs/symtable.h"
#include "bvs.c"
#include "structs/stack.h"
#include <stdio.h>

#define MAX_SCOPES 65536 
//2^16

void Scope_Init(Scope *scope);
Scope *Scope_ChildScope(Scope *scope);
void Scope_Dispose(Scope *scope);

void ParamList_Dispose(Param *paramList);

void SymTable_Init(SymTable *table) {
  table->stack = (Stack *)malloc(sizeof(Stack));
  initializeStack(table->stack, MAX_SCOPES);
  table->current = NULL;
  return;
}

void SymTable_NewScope(SymTable *table) {
  /*if(table == NULL) {
    fprintf(stderr, "Something terrible is going to happen\n");
  }*/
  table->current = Scope_ChildScope(table->current);
  StackPush(table->stack, table->current, sizeof(Scope));
  return;
}

void SymTable_UpperScope(SymTable *table) {
  table->current = table->current->parent;
  return;
}

Symbol *SymTable_Search(SymTable *table, char *name) {
  return ((Symbol *)(BVS_Search(table->current->tree, name)));
}

void SymTable_SetType(SymTable *table, char *name, type_t type) {
  ((Symbol *)(BVS_Search(table->current->tree, name)))->type = type;//TODO: implement "current" mechanism
  return;
}

void SymTable_SetMut(SymTable *table, char *name, bool isMutable) {
  ((Symbol *)(BVS_Search(table->current->tree, name)))->mut = isMutable;
  return;
}

void SymTable_SetInit(SymTable *table, char *name, bool isInit) {
  ((Symbol *)(BVS_Search(table->current->tree, name)))->init = isInit;
  return;
}

void SymTable_PushFuncParam(SymTable *table, char *name, type_t paramType) {
  Symbol *symbol = (Symbol *)(BVS_Search(table->current->tree, name));
  Param *current = symbol->paramList;
  if(current == NULL) {
    symbol->paramList = (Param *)malloc(sizeof(Param));
    symbol->paramList->paramType = paramType;
    symbol->paramList->next = NULL;
    return;
  }
  Param *next = current;
  while(next != NULL) {
    current = next;
    next = current->next;
  }
  current->next = (Param *)malloc(sizeof(Param));
  current->next->paramType = paramType;
  current->next->next = NULL;
  return;
}

void SymTable_SetRetType(SymTable *table, char *name, type_t retType) {
  ((Symbol *)(BVS_Search(table->current->tree, name)))->retType = retType;
}

type_t SymTable_GetType(SymTable *table, char *name) {
  return ((Symbol *)(BVS_Search(table->current->tree, name)))->type;
}

bool SymTable_GetMut(SymTable *table, char *name) {
  return ((Symbol *)(BVS_Search(table->current->tree, name)))->mut;
}

bool SymTable_GetInit(SymTable *table, char *name) {
  return ((Symbol *)(BVS_Search(table->current->tree, name)))->init;
}

type_t SymTable_GetRetType(SymTable *table, char *name) {
  return ((Symbol *)(BVS_Search(table->current->tree, name)))->retType;
}

Param *SymTable_GetParamList(SymTable *table, char *name) {
  return ((Symbol *)(BVS_Search(table->current->tree, name)))->paramList;
}

void SymTable_AddSymbol(SymTable *table, Symbol *symbol) {
  symbol->name = strdup(symbol->name);
  BVS_Insert(table->current->tree, symbol->name, (void *)symbol, sizeof(Symbol));
  return;
}

void SymTable_Dispose(SymTable *table) {
  while (!isStackEmpty(table->stack)) {
    Scope *scope = (Scope *)(popStack(table->stack));
    Scope_Dispose(scope);
  }
  //should we free a symtable? 
  return;
}

/////////////////////////////////////////////////////////////////////////////

void Scope_Init(Scope *scope) {
  scope->tree = (BVS *)malloc(sizeof(BVS));
  BVS_Init(scope->tree);
  return;
}

void Scope_Dispose(Scope *scope) {
  while (scope->tree->root != NULL)
  {
    free(((Symbol *)scope->tree->root->data)->name);
    ParamList_Dispose(((Symbol *)scope->tree->root->data)->paramList);
    BVS_Delete(scope->tree, scope->tree->root->key);
  }
  free(scope->tree);
  free(scope);
  return;
}

Scope *Scope_ChildScope(Scope *scope) {
  Scope *newScope = (Scope *)malloc(sizeof(Scope));
  Scope_Init(newScope);
  newScope->parent = scope;
  return newScope;   
}

///////////////////////////////////////////////////////////////////////

void ParamList_Dispose(Param *paramList) {
  Param *current = paramList;
  Param *next = paramList;
  while(current != NULL) {
    next = current->next;
    free(current);
    current = next;
  }
  return;
}
