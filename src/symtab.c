
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
