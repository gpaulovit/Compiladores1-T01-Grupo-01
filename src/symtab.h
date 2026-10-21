
#ifndef SYMTAB_H
#define SYMTAB_H

#include <stddef.h>

/* Tipos suportados pela linguagem Mini-C. */
typedef enum {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_VOID
} DataType;

/* Categorias de identificadores registrados na tabela. */
typedef enum {
    SYMBOL_VARIABLE,
    SYMBOL_PARAMETER,
    SYMBOL_FUNCTION
} SymbolKind;

/* Representa um parâmetro da assinatura de uma função. */
typedef struct {
    DataType type;
} ParameterType;

/* Representa um símbolo declarado no programa. */
typedef struct Symbol {
    char *name;
    DataType type;
    SymbolKind kind;
    int line;

    ParameterType *parameters;
    size_t parameter_count;

    struct Symbol *next;
} Symbol;

/* Representa um escopo e os símbolos declarados nele. */
typedef struct Scope {
    Symbol *symbols;
    struct Scope *parent;
} Scope;

/* Representa a tabela de símbolos e seu escopo atual. */
typedef struct {
    Scope *current_scope;
} SymbolTable;

/* Ciclo de vida da tabela. */
SymbolTable *symtab_create(void);
void symtab_destroy(SymbolTable *table);

/* Gerenciamento de escopos. */
int symtab_open_scope(SymbolTable *table);
int symtab_close_scope(SymbolTable *table);

/* Inserção de símbolos. */
int symtab_insert_variable(
    SymbolTable *table,
    const char *name,
    DataType type,
    int line
);

int symtab_insert_parameter(
    SymbolTable *table,
    const char *name,
    DataType type,
    int line
);

int symtab_insert_function(
    SymbolTable *table,
    const char *name,
    DataType return_type,
    const ParameterType *parameters,
    size_t parameter_count,
    int line
);

/* Busca de símbolos. */
const Symbol *symtab_lookup_current(
    const SymbolTable *table,
    const char *name
);

const Symbol *symtab_lookup(
    const SymbolTable *table,
    const char *name
);

/* Impressão para depuração. */
void symtab_print(const SymbolTable *table);
#endif /* SYMTAB_H */
