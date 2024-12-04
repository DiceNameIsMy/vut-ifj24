//
// created by GladiatorEntered on 18/11/24
//

#include "symtable.h"
#include "bvs.h"
#include "stack.h"
#include "ast.h" //TEMPORARY!!
#include <string.h>
#include <stdio.h>

#define MAX_SCOPES 65536
// 2^16

void Scope_Init(Scope *scope);
Scope *Scope_ChildScope(Scope *scope);
void Scope_Dispose(Scope *scope);

void ParamList_Dispose(Param *paramList);

void SymTable_Init(SymTable *table)
{
  table->stack = (Stack *)malloc(sizeof(Stack));
  initializeStack(table->stack, MAX_SCOPES);
  table->current = NULL;
  return;
}

void SymTable_NewScope(SymTable *table)
{
  /*if(table == NULL) {
    fprintf(stderr, "Something terrible is going to happen\n");
  }*/
  table->current = Scope_ChildScope(table->current);
  StackPush(table->stack, table->current, sizeof(Scope));
  return;
}

int SymTable_UpperScope(SymTable *table)
{
  int unused = table->current->unused_cnt;
  table->current = table->current->parent;
  return unused; //how much unused
}

Symbol *SymTable_Search(SymTable *table, char *name)
{
  Scope *current = table->current;
  while (current != NULL && BVS_Search(current->tree, name) == NULL)
    current = current->parent;
  if (current == NULL) {
    return NULL;
  }
  return ((Symbol *)(BVS_Search(current->tree, name)));
}

void SymTable_SetType(SymTable *table, char *name, type_t type)
{
  Scope *current = table->current;
  while (current != NULL && BVS_Search(current->tree, name) == NULL)
    current = current->parent;
  if (current == NULL) {
    return;
  }
  ((Symbol *)(BVS_Search(current->tree, name)))->type = type;//TODO: implement "current" mechanism
  return;
}

void SymTable_SetCTVal(SymTable *table, char *name, ASTValue *value)
{
  Scope *current = table->current;
  while (current != NULL && BVS_Search(current->tree, name) == NULL)
    current = current->parent;
  if (current == NULL) {
    return;
  }
  ((Symbol *)(BVS_Search(current->tree, name)))->CompTimeVal = value;//TODO: implement "current" mechanism
  return;
}

void SymTable_SetMut(SymTable *table, char *name, bool isMutable)
{
  Scope *current = table->current;
  while (current != NULL && BVS_Search(current->tree, name) == NULL)
    current = current->parent;
  if (current == NULL) {
    return;
  }
  ((Symbol *)(BVS_Search(current->tree, name)))->mut = isMutable;
  return;
}

void SymTable_SetUsed(SymTable *table, char *name, bool isUsed)
{
  Scope *current = table->current;
  while (current != NULL && BVS_Search(current->tree, name) == NULL)
    current = current->parent;
  if (current == NULL) {
    return;
  }
  if(!SymTable_GetUsed(table, name) && isUsed == true) {
    current->unused_cnt--;
  }
  ((Symbol *)(BVS_Search(current->tree, name)))->used = isUsed;
  return;
}

void SymTable_PushFuncParam(SymTable *table, char *name, type_t paramType, char *paramName)
{
  Scope *current = table->current;
  while (current != NULL && BVS_Search(current->tree, name) == NULL)
    current = current->parent;
  if (current == NULL) {
    return;
  }
  Symbol *symbol = (Symbol *)(BVS_Search(current->tree, name));

  // Allocate new parameter
  Param *newParam = malloc(sizeof(Param));
  newParam->paramType = paramType;
  if (paramName == NULL)
  {
    newParam->name = strdup("placeholder");
  }
  else
  {
    newParam->name = strdup(paramName);
  }
  newParam->next = NULL;

  // If list is empty, initialize it
  Param *currentParam = symbol->paramList;
  if (currentParam == NULL)
  {
    symbol->paramList = newParam;
    return;
  }

  // Otherwise, append to the end
  Param *nextParam = currentParam;
  while (nextParam != NULL)
  {
    currentParam = nextParam;
    nextParam = currentParam->next;
  }

  currentParam->next = newParam;

  //fprintf(stderr, "Param pushed for %s", name);
  return;
}

void SymTable_SetRetType(SymTable *table, char *name, type_t retType)
{
  Scope *current = table->current;
  while (BVS_Search(current->tree, name) == NULL)
    current = current->parent;
  if (current == NULL) {
    return;
  }
  ((Symbol *)(BVS_Search(current->tree, name)))->retType = retType;
  return;
}

type_t SymTable_GetType(SymTable *table, char *name)
{
  Scope *current = table->current;
  while (BVS_Search(current->tree, name) == NULL)
    current = current->parent;
  if (current == NULL) {
    exit(99);
  }
  return ((Symbol *)(BVS_Search(current->tree, name)))->type;
}

ASTValue *SymTable_GetCTVal(SymTable *table, char *name)
{
  Scope *current = table->current;
  while (BVS_Search(current->tree, name) == NULL)
    current = current->parent;
  if (current == NULL) {
    exit(99);
  }
  return ((Symbol *)(BVS_Search(current->tree, name)))->CompTimeVal;
}

bool SymTable_GetMut(SymTable *table, char *name)
{
  Scope *current = table->current;
  while (BVS_Search(current->tree, name) == NULL)
    current = current->parent;
  if (current == NULL) {
    exit(99);
  }
  return ((Symbol *)(BVS_Search(current->tree, name)))->mut;
}

bool SymTable_GetUsed(SymTable *table, char *name)
{
  Scope *current = table->current;
  while (BVS_Search(current->tree, name) == NULL)
    current = current->parent;
  if (current == NULL) {
    exit(99);
  }
  return ((Symbol *)(BVS_Search(current->tree, name)))->used;
}

type_t SymTable_GetRetType(SymTable *table, char *name)
{
  Scope *current = table->current;
  while (BVS_Search(current->tree, name) == NULL)
    current = current->parent;
  if (current == NULL) {
    exit(99);
  }
  return ((Symbol *)(BVS_Search(current->tree, name)))->retType;
}

Param *SymTable_GetParamList(SymTable *table, char *name)
{
  Scope *current = table->current;
  while (BVS_Search(current->tree, name) == NULL)
    current = current->parent;
  if (current == NULL) {
    exit(99);
  }
  return ((Symbol *)(BVS_Search(current->tree, name)))->paramList;
}

void SymTable_AddSymbol(SymTable *table, Symbol *symbol)
{
  symbol->name = strdup(symbol->name);  
  if(BVS_Search(table->current->tree, symbol->name) == NULL && symbol->used == false) {
    table->current->unused_cnt++;
  }
  BVS_Insert(table->current->tree, symbol->name, (void *)symbol, sizeof(Symbol));
  return;
}

void SymTable_Dispose(SymTable *table)
{
  while (!isStackEmpty(table->stack))
  {
    Scope *scope = (Scope *)(popStack(table->stack));
    Scope_Dispose(scope);
  }
  // should we free a symtable?
  return;
}

/////////////////////////////////////////////////////////////////////////////

void Scope_Init(Scope *scope)
{
  scope->tree = (BVS *)malloc(sizeof(BVS));
  BVS_Init(scope->tree);
  return;
}

void Scope_Dispose(Scope *scope)
{
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

Scope *Scope_ChildScope(Scope *scope)
{
  Scope *newScope = (Scope *)malloc(sizeof(Scope));
  Scope_Init(newScope);
  newScope->parent = scope;
  newScope->unused_cnt = 0; //count unused vars
  return newScope;
}

///////////////////////////////////////////////////////////////////////

void ParamList_Dispose(Param *paramList)
{
  Param *current = paramList;
  Param *next = paramList;
  while (current != NULL)
  {
    next = current->next;
    free(current->name);
    free(current);
    current = next;
  }
  return;
}
