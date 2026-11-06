#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "symtab.h"
#include <stddef.h>

/* Tipos semânticos de expressões Mini-C */
typedef enum {
    EXP_TYPE_INT,
    EXP_TYPE_FLOAT,
    EXP_TYPE_VOID,
    EXP_TYPE_STRING,
    EXP_TYPE_CHAR,
    EXP_TYPE_ERROR
} ExpType;

/* Informações semânticas associadas a cada nó de expressão */
typedef struct {
    ExpType type;
    int line;
    char *func_name; /* Nome da função se for chamada retornando void, para mensagens de erro */
} SemanticExpr;

/* Lista de argumentos em chamadas de função */
typedef struct {
    ExpType *types;
    int *lines;
    char **func_names;
    size_t count;
    size_t capacity;
} SemanticArgs;

/* Inicialização e controle do analisador */
void semantico_init(SymbolTable *table);

/* Declaração de variáveis e parâmetros */
void semantico_checa_declaracao_var(const char *tipo, const char *nome, int linha);
void semantico_checa_declaracao_var_init(const char *tipo, const char *nome, SemanticExpr exp, int linha);
void semantico_checa_declaracao_param(const char *tipo, const char *nome, int linha);

/* Escopo de funções e checagem de retorno */
void semantico_define_funcao_inicio(const char *tipo_ret, const char *nome, int linha);
void semantico_define_funcao_fim(void);
void semantico_checa_retorno(SemanticExpr exp, int linha);
void semantico_checa_retorno_vazio(int linha);

/* Atribuições */
SemanticExpr semantico_checa_atribuicao(const char *nome, SemanticExpr exp, int linha);
SemanticExpr semantico_checa_atribuicao_composta(const char *nome, const char *op, SemanticExpr exp, int linha);

/* Identificadores e Literais */
SemanticExpr semantico_checa_identificador(const char *nome, int linha);
SemanticExpr semantico_literal_int(int linha);
SemanticExpr semantico_literal_float(int linha);
SemanticExpr semantico_literal_string(int linha);
SemanticExpr semantico_literal_char(int linha);

/* Operações aritméticas, relacionais e lógicas */
SemanticExpr semantico_checa_op_aritmetico(const char *op, SemanticExpr esq, SemanticExpr dir, int linha);
SemanticExpr semantico_checa_op_relacional(const char *op, SemanticExpr esq, SemanticExpr dir, int linha);
SemanticExpr semantico_checa_op_logico(const char *op, SemanticExpr esq, SemanticExpr dir, int linha);
SemanticExpr semantico_checa_op_unario(const char *op, SemanticExpr sub, int linha);
SemanticExpr semantico_checa_inc_dec(const char *nome, const char *op, int linha);

/* Chamadas de função */
SemanticExpr semantico_checa_chamada(const char *nome, SemanticArgs *args, int linha);

/* Argumentos de chamada */
SemanticArgs semantico_cria_args(SemanticExpr primeiro);
SemanticArgs semantico_adiciona_arg(SemanticArgs lista, SemanticExpr novo);
void semantico_libera_args(SemanticArgs *args);

/* Comandos com expressões */
void semantico_checa_expressao_comando(SemanticExpr exp, int linha);
void semantico_checa_print(SemanticExpr exp, int linha);
void semantico_checa_condicao(SemanticExpr exp, int linha);

/* Funções utilitárias */
void semantico_libera_expr(SemanticExpr *exp);

#endif /* SEMANTICO_H */
