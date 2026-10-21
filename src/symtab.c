
#include "symtab.h"
#include <string.h>
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

/* Insere um símbolo no escopo atual, copiando seu nome. */
static int symtab_insert_symbol(
    SymbolTable *table,
    const char *name,
    DataType type,
    SymbolKind kind,
    int line
)
{
    if (table == NULL || table->current_scope == NULL || name == NULL) {
        return 0;
    }

    /* Impede redeclaração no mesmo escopo. */
    for (Symbol *current = table->current_scope->symbols;
         current != NULL;
         current = current->next) {
        if (strcmp(current->name, name) == 0) {
            return 0;
        }
    }

    Symbol *symbol = malloc(sizeof(Symbol));

    if (symbol == NULL) {
        return 0;
    }

    size_t name_length = strlen(name) + 1;
    symbol->name = malloc(name_length);

    if (symbol->name == NULL) {
        free(symbol);
        return 0;
    }

    memcpy(symbol->name, name, name_length);

    symbol->type = type;
    symbol->kind = kind;
    symbol->line = line;
    symbol->parameters = NULL;
    symbol->parameter_count = 0;

    symbol->next = table->current_scope->symbols;
    table->current_scope->symbols = symbol;

    return 1;
}

/* Insere uma variável no escopo atual. */
int symtab_insert_variable(
    SymbolTable *table,
    const char *name,
    DataType type,
    int line
)
{
    if (type == TYPE_VOID) {
        return 0;
    }

    return symtab_insert_symbol(
        table, name, type, SYMBOL_VARIABLE, line
    );
}

/* Insere um parâmetro no escopo atual. */
int symtab_insert_parameter(
    SymbolTable *table,
    const char *name,
    DataType type,
    int line
)
{
    if (type == TYPE_VOID) {
        return 0;
    }

    return symtab_insert_symbol(
        table, name, type, SYMBOL_PARAMETER, line
    );
}

/* Insere uma função e copia os tipos dos seus parâmetros. */
int symtab_insert_function(
    SymbolTable *table,
    const char *name,
    DataType return_type,
    const ParameterType *parameters,
    size_t parameter_count,
    int line
)
{
    if (table == NULL || table->current_scope == NULL || name == NULL) {
        return 0;
    }

    if (parameter_count > 0 && parameters == NULL) {
        return 0;
    }

    /* Impede redeclaração no escopo atual. */
    for (Symbol *current = table->current_scope->symbols;
         current != NULL;
         current = current->next) {
        if (strcmp(current->name, name) == 0) {
            return 0;
        }
    }

    Symbol *symbol = malloc(sizeof(Symbol));

    if (symbol == NULL) {
        return 0;
    }

    size_t name_length = strlen(name) + 1;
    symbol->name = malloc(name_length);

    if (symbol->name == NULL) {
        free(symbol);
        return 0;
    }

    memcpy(symbol->name, name, name_length);

    symbol->type = return_type;
    symbol->kind = SYMBOL_FUNCTION;
    symbol->line = line;
    symbol->parameters = NULL;
    symbol->parameter_count = parameter_count;

    if (parameter_count > 0) {
        symbol->parameters = malloc(
            parameter_count * sizeof(ParameterType)
        );

        if (symbol->parameters == NULL) {
            free(symbol->name);
            free(symbol);
            return 0;
        }

        memcpy(
            symbol->parameters,
            parameters,
            parameter_count * sizeof(ParameterType)
        );
    }

    symbol->next = table->current_scope->symbols;
    table->current_scope->symbols = symbol;

    return 1;
}
