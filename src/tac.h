#ifndef TAC_H
#define TAC_H

#include <stdio.h>
#include "ast.h"

typedef enum {
    /* Operacoes basicas e expressoes (Frente A) */
    TAC_COPY,       /* res = arg1 */
    TAC_BINARIO,    /* res = arg1 op arg2 */
    TAC_UNARIO,     /* res = op arg1 */

    /* Controle de fluxo e funcoes (Frente B) */
    TAC_LABEL,      /* res: */
    TAC_GOTO,       /* goto res */
    TAC_IFZ,        /* ifz arg1 goto res */
    TAC_IFNZ,       /* ifnz arg1 goto res */
    TAC_BEGIN_FUNC, /* func res: */
    TAC_END_FUNC,   /* endfunc res */
    TAC_PARAM,      /* param res */
    TAC_ARG,        /* arg arg1 */
    TAC_CALL,       /* [res =] call arg1, arg2 */
    TAC_RETURN,     /* return [arg1] */
    TAC_PRINT       /* print arg1 */
} TacOp;

typedef struct TacInstr {
    TacOp cat;
    const char *op;
    const char *res;
    const char *arg1;
    const char *arg2;
    struct TacInstr *prox;
} TacInstr;

typedef struct TacStr {
    char *s;
    struct TacStr *prox;
} TacStr;

typedef struct {
    TacInstr *inicio;
    TacInstr *fim;
    TacStr *strings;
    int cont_temp;
    int cont_rotulo;
} TacPrograma;

/* Ciclo de vida e emissao */
TacPrograma *tac_cria(void);
void tac_libera(TacPrograma *p);

const char *tac_str(TacPrograma *p, const char *s);
const char *tac_novo_temp(TacPrograma *p);
const char *tac_novo_rotulo(TacPrograma *p);

TacInstr *tac_emite(TacPrograma *p, TacOp cat, const char *op,
                    const char *res, const char *arg1, const char *arg2);

/* Geracao a partir da AST e impressao (--tac) */
TacPrograma *tac_gera(const No *raiz);
void tac_gera_no(TacPrograma *p, const No *n);
const char *tac_gera_expr(TacPrograma *p, const No *n);
void tac_imprime(const TacPrograma *p, FILE *saida);

#endif /* TAC_H */
