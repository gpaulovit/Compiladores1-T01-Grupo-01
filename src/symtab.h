
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

#endif /* SYMTAB_H */
