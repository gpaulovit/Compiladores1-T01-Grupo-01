/* Árvore sintática abstrata (AST) do Mini-C.
 *
 * O parser constrói um nó a cada redução; a raiz fica em programa. Todos os
 * nós têm o mesmo formato: o que muda de uma construção para outra é a
 * categoria e o significado de cada posição de filhos[], documentado abaixo. */

#ifndef AST_H
#define AST_H

#include <stdio.h>
#include "semantico.h"   /* ExpType, usado no campo tipo */

#define AST_MAX_FILHOS 4

/* Uma categoria por construção da linguagem. Ao lado de cada uma, o uso dos
 * campos; filho ausente é NULL. "Lista" é encadeada pelo campo prox. */
typedef enum {
    /* Estrutura do programa */
    NO_PROGRAMA,    /* [0] lista das declarações globais (variáveis e funções) */
    NO_FUNCAO,      /* valor.s nome, tipo de retorno; [0] lista de NO_PARAM, [1] NO_BLOCO */
    NO_PARAM,       /* valor.s nome, tipo */
    NO_DECL_VAR,    /* valor.s nome, tipo; [0] inicialização. "int a, b;" gera dois nós */

    /* Comandos */
    NO_BLOCO,       /* [0] lista de declarações e comandos, na ordem do código */
    NO_IF,          /* [0] condição, [1] então, [2] senão */
    NO_WHILE,       /* [0] condição, [1] corpo */
    NO_DO_WHILE,    /* [0] corpo, [1] condição */
    NO_FOR,         /* [0] inicialização, [1] condição, [2] passo, [3] corpo */
    NO_RETURN,      /* [0] expressão retornada */
    NO_PRINT,       /* [0] expressão impressa */
    NO_VAZIO,       /* comando vazio ";" */
    /* Uma expressão usada como comando ("x = 1;") entra na lista sem nó próprio. */

    /* Expressões: tipo é o calculado pela análise semântica */
    NO_ATRIBUICAO,  /* op "=", "+=", "-=", "*=", "/="; [0] NO_ID, [1] valor */
    NO_BINARIO,     /* op aritmético, relacional ou lógico; [0] esquerda, [1] direita */
    NO_UNARIO,      /* op "!" ou "-"; [0] operando */
    NO_PRE_INCDEC,  /* op "++" ou "--"; [0] NO_ID */
    NO_POS_INCDEC,  /* op "++" ou "--"; [0] NO_ID */
    NO_CHAMADA,     /* valor.s nome da função; [0] lista de argumentos */
    NO_ID,          /* valor.s nome */
    NO_LIT_INT,     /* valor.i */
    NO_LIT_FLOAT,   /* valor.f */
    NO_LIT_CHAR,    /* valor.s lexema, com as aspas */
    NO_LIT_STRING   /* valor.s lexema, com as aspas */
} Categoria;

typedef struct No {
    Categoria cat;
    int linha;              /* linha do primeiro token da construção */
    ExpType tipo;           /* só tem sentido em expressões, declarações, parâmetros e funções */
    const char *op;         /* operador; aponta para literal estático, nunca é liberado */
    union {
        int    i;
        double f;
        char  *s;           /* pertence ao nó: liberado por ast_libera */
    } valor;
    struct No *filhos[AST_MAX_FILHOS];
    struct No *prox;        /* próximo elemento quando o nó está numa lista */
} No;

/* Criação. Os filhos passam a pertencer ao novo nó. */
No *ast_cria(Categoria cat, int linha, No *f0, No *f1, No *f2, No *f3);
No *ast_op(Categoria cat, const char *op, int linha, No *f0, No *f1);
No *ast_texto(Categoria cat, char *s, int linha);   /* assume a posse de s (strdup do scanner) */
No *ast_int(int v, int linha);
No *ast_float(double v, int linha);

/* Acrescenta novo ao fim da lista e devolve a cabeça. Aceita lista NULL. */
No *ast_lista_adiciona(No *lista, No *novo);

/* Impressão indentada para depuração (--ast) e liberação da árvore inteira,
 * incluindo filhos, irmãos (prox) e as strings de valor.s. */
void ast_imprime(const No *raiz, FILE *saida);
void ast_libera(No *raiz);

const char *ast_nome_categoria(Categoria cat);

#endif /* AST_H */
