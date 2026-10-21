#include "symtab.h"

#include <assert.h>
#include <stdio.h>

/* Testa a criação e a destruição da tabela. */
static void test_create_destroy(void)
{
    SymbolTable *table = symtab_create();

    assert(table != NULL);
    assert(symtab_lookup(table, "x") == NULL);

    symtab_destroy(table);
}

/* Testa a inserção e a busca de uma variável. */
static void test_insert_lookup_variable(void)
{
    SymbolTable *table = symtab_create();

    assert(table != NULL);
    assert(symtab_insert_variable(table, "x", TYPE_INT, 1) == 1);

    const Symbol *symbol = symtab_lookup(table, "x");

    assert(symbol != NULL);
    assert(symbol->type == TYPE_INT);
    assert(symbol->kind == SYMBOL_VARIABLE);
    assert(symbol->line == 1);

    symtab_destroy(table);
}

/* Testa a rejeição de redeclaração no mesmo escopo. */
static void test_redeclaration(void)
{
    SymbolTable *table = symtab_create();

    assert(table != NULL);
    assert(symtab_insert_variable(table, "x", TYPE_INT, 1) == 1);
    assert(symtab_insert_variable(table, "x", TYPE_FLOAT, 2) == 0);

    symtab_destroy(table);
}

/* Testa a abertura e o fechamento de escopos. */
static void test_scopes(void)
{
    SymbolTable *table = symtab_create();

    assert(table != NULL);
    assert(symtab_insert_variable(table, "x", TYPE_INT, 1) == 1);

    assert(symtab_open_scope(table) == 1);

    /* O escopo interno consegue acessar o símbolo global. */
    assert(symtab_lookup(table, "x") != NULL);

    /* Mas a busca local não encontra símbolos globais. */
    assert(symtab_lookup_current(table, "x") == NULL);

    assert(symtab_close_scope(table) == 1);

    /* Após fechar o escopo, a variável global continua acessível. */
    assert(symtab_lookup_current(table, "x") != NULL);

    /* O escopo global não pode ser fechado. */
    assert(symtab_close_scope(table) == 0);

    symtab_destroy(table);
}

/* Testa o sombreamento de uma variável em escopo interno. */
static void test_shadowing(void)
{
    SymbolTable *table = symtab_create();

    assert(table != NULL);
    assert(symtab_insert_variable(table, "x", TYPE_INT, 1) == 1);

    assert(symtab_open_scope(table) == 1);
    assert(symtab_insert_variable(table, "x", TYPE_FLOAT, 2) == 1);

    /* A busca retorna a declaração mais próxima. */
    const Symbol *symbol = symtab_lookup(table, "x");

    assert(symbol != NULL);
    assert(symbol->type == TYPE_FLOAT);
    assert(symbol->line == 2);

    assert(symtab_close_scope(table) == 1);

    /* Depois de fechar o escopo interno, volta a valer a declaração global. */
    symbol = symtab_lookup(table, "x");

    assert(symbol != NULL);
    assert(symbol->type == TYPE_INT);
    assert(symbol->line == 1);

    symtab_destroy(table);
}

/* Testa a inserção de funções e seus tipos de parâmetros. */
static void test_function(void)
{
    SymbolTable *table = symtab_create();

    assert(table != NULL);

    ParameterType parameters[] = {
        { TYPE_INT },
        { TYPE_FLOAT }
    };

    assert(symtab_insert_function(
        table, "calcular", TYPE_FLOAT, parameters, 2, 10
    ) == 1);

    const Symbol *symbol = symtab_lookup(table, "calcular");

    assert(symbol != NULL);
    assert(symbol->kind == SYMBOL_FUNCTION);
    assert(symbol->type == TYPE_FLOAT);
    assert(symbol->parameter_count == 2);
    assert(symbol->parameters[0].type == TYPE_INT);
    assert(symbol->parameters[1].type == TYPE_FLOAT);

    /* Não permite redeclarar a função no mesmo escopo. */
    assert(symtab_insert_function(
        table, "calcular", TYPE_INT, NULL, 0, 20
    ) == 0);

    symtab_destroy(table);
}

/* Testa a rejeição de variáveis e parâmetros do tipo void. */
static void test_void_declarations(void)
{
    SymbolTable *table = symtab_create();

    assert(table != NULL);

    assert(symtab_insert_variable(
        table, "invalida", TYPE_VOID, 1
    ) == 0);

    assert(symtab_insert_parameter(
        table, "param_invalido", TYPE_VOID, 2
    ) == 0);

    assert(symtab_lookup(table, "invalida") == NULL);
    assert(symtab_lookup(table, "param_invalido") == NULL);

    symtab_destroy(table);
}

int main(void)
{
    test_create_destroy();
    test_insert_lookup_variable();
    test_redeclaration();
    test_scopes();
    test_shadowing();
    test_function();
    test_void_declarations();

    printf("Testes da tabela de simbolos: OK\n");

    return 0;
}