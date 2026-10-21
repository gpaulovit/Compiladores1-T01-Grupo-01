
#include "symtab.h"

#include <stdlib.h>

/* Cria a tabela de símbolos com um escopo global vazio. */
SymbolTable *symtab_create(void)
{
    SymbolTable *table = malloc(sizeof(SymbolTable));

    if (table == NULL) {
        return NULL;
    }

    Scope *global_scope = malloc(sizeof(Scope));

    if (global_scope == NULL) {
        free(table);
        return NULL;
    }

    global_scope->symbols = NULL;
    global_scope->parent = NULL;

    table->current_scope = global_scope;

    return table;
}

/* Libera todos os símbolos e escopos da tabela. */
void symtab_destroy(SymbolTable *table)
{
    if (table == NULL) {
        return;
    }

    Scope *scope = table->current_scope;

    while (scope != NULL) {
        Scope *parent = scope->parent;
        Symbol *symbol = scope->symbols;

        while (symbol != NULL) {
            Symbol *next = symbol->next;

            free(symbol->name);
            free(symbol->parameters);
            free(symbol);

            symbol = next;
        }

        free(scope);
        scope = parent;
    }

    free(table);
}

/* Abre um novo escopo filho do escopo atual. */
int symtab_open_scope(SymbolTable *table)
{
    if (table == NULL || table->current_scope == NULL) {
        return 0;
    }

    Scope *scope = malloc(sizeof(Scope));

    if (scope == NULL) {
        return 0;
    }

    scope->symbols = NULL;
    scope->parent = table->current_scope;

    table->current_scope = scope;

    return 1;
}

/* Fecha o escopo atual e retorna ao escopo pai. */
int symtab_close_scope(SymbolTable *table)
{
    if (table == NULL || table->current_scope == NULL) {
        return 0;
    }

    Scope *scope = table->current_scope;

    /* O escopo global não pode ser fechado. */
    if (scope->parent == NULL) {
        return 0;
    }

    table->current_scope = scope->parent;

    Symbol *symbol = scope->symbols;

    while (symbol != NULL) {
        Symbol *next = symbol->next;

        free(symbol->name);
        free(symbol->parameters);
        free(symbol);

        symbol = next;
    }

    free(scope);

    return 1;
}
