#include "structs/symtable.h"
#include "bvs.c"
//TODO: implement functions from the header

void SymTable_SetType(SymTable *table, char *name, type_t type) {
  ((Symbol *)(BVS_Search(table->tree, name)))->type = type;
  return;
}

void SymTable_SetDecl(SymTable *table, char *name, bool isDeclared) {
  ((Symbol *)(BVS_Search(table->tree, name)))->decl = isDeclared;
  return;
}

void SymTable_SetInit(SymTable *table, char *name, bool isInit) {
  ((Symbol *)(BVS_Search(table->tree, name)))->init = isInit;
  return;
}

void SymTable_SetScope(SymTable *table, char *name, scope_t scope) {
  ((Symbol *)(BVS_Search(table->tree, name)))->scope = scope;
  return;
}

type_t SymTable_GetType(SymTable *table, char *name) {
  return ((Symbol *)(BVS_Search(table->tree, name)))->type;
}

bool SymTable_GetDecl(SymTable *table, char *name) {
  return ((Symbol *)(BVS_Search(table->tree, name)))->decl;
}

bool SymTable_GetInit(SymTable *table, char *name) {
  return ((Symbol *)(BVS_Search(table->tree, name)))->init;
}

scope_t SymTable_GetScope(SymTable *table, char *name) {
  return ((Symbol *)(BVS_Search(table->tree, name)))->scope;
}

void SymTable_Init(SymTable *table) {
  BVS_Init(table->tree);
  return;
}

void SymTable_AddSymbol(SymTable *table, Symbol *symbol) {
  symbol->name = strdup(symbol->name);
  BVS_Insert(table->tree, symbol->name, (void *)symbol, sizeof(Symbol));
  return;
}

void SymTable_Dispose(SymTable *table) {
  while (table->tree->root != NULL)
  {
    free(((Symbol *)table->tree->root->data)->name);
    BVS_Delete(table->tree, table->tree->root->key);
  }
  return;
}

// SymTable *SymTable_NewScope(SymTable *table) {
// HONESTLY IDK 
// HELPME PLZ (Sure, it's a joke. Almost a joke.)
//}
