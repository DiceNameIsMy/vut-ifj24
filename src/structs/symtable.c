//
//created by GladiatorEntered on 18/11/24
//


#include "structs/symtable.h"
#include "bvs.c"
#include "structs/stack.h"

#define MAX_SCOPES 65536 
//2^16

void Scope_Init(Scope *scope);
Scope *Scope_ChildScope(Scope *scope);
void Scope_Dispose(Scope *scope);

void SymTable_Init(SymTable *table) {
  initializeStack(table->stack, MAX_SCOPES);
  table->current = NULL;
  return;
}

void SymTable_NewScope(SymTable *table) {
  table->current = Scope_ChildScope(table->current);
  StackPush(table->stack, table->current, sizeof(Scope));
  return;
}

void SymTable_UpperScope(SymTable *table) {
  table->current = table->current->parent;
  return;
}

void SymTable_SetType(SymTable *table, char *name, type_t type) {
  ((Symbol *)(BVS_Search(table->current->tree, name)))->type = type;//TODO: implement "current" mechanism
  return;
}

void SymTable_SetDecl(SymTable *table, char *name, bool isDeclared) {
  ((Symbol *)(BVS_Search(table->current->tree, name)))->decl = isDeclared;
  return;
}

void SymTable_SetInit(SymTable *table, char *name, bool isInit) {
  ((Symbol *)(BVS_Search(table->current->tree, name)))->init = isInit;
  return;
}

type_t SymTable_GetType(SymTable *table, char *name) {
  return ((Symbol *)(BVS_Search(table->current->tree, name)))->type;
}

bool SymTable_GetDecl(SymTable *table, char *name) {
  return ((Symbol *)(BVS_Search(table->current->tree, name)))->decl;
}

bool SymTable_GetInit(SymTable *table, char *name) {
  return ((Symbol *)(BVS_Search(table->current->tree, name)))->init;
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
  BVS_Init(scope->tree);
  return;
}

void Scope_Dispose(Scope *scope) {
  while (scope->tree->root != NULL)
  {
    free(((Symbol *)scope->tree->root->data)->name);
    BVS_Delete(scope->tree, scope->tree->root->key);
  }
  free(scope);
  return;
}

Scope *Scope_ChildScope(Scope *scope) {
  Scope *newScope = (Scope *)malloc(sizeof(Scope));
  Scope_Init(newScope);
  newScope->parent = scope;
  return newScope;   
}
