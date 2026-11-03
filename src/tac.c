#include <stdlib.h>
#include <string.h>
#include "tac.h"

TacPrograma *tac_cria(void) {
    TacPrograma *p = calloc(1, sizeof(TacPrograma));
    if (p == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente para o TAC.\n");
        exit(1);
    }
    return p;
}

void tac_libera(TacPrograma *p) {
    if (p == NULL) {
        return;
    }
    TacInstr *in = p->inicio;
    while (in != NULL) {
        TacInstr *prox = in->prox;
        free(in);
        in = prox;
    }
    TacStr *st = p->strings;
    while (st != NULL) {
        TacStr *prox = st->prox;
        free(st->s);
        free(st);
        st = prox;
    }
    free(p);
}

const char *tac_str(TacPrograma *p, const char *s) {
    if (s == NULL) {
        return NULL;
    }
    TacStr *no = malloc(sizeof(TacStr));
    char *copia = strdup(s);
    if (no == NULL || copia == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente para o TAC.\n");
        exit(1);
    }
    no->s = copia;
    no->prox = p->strings;
    p->strings = no;
    return copia;
}

const char *tac_novo_temp(TacPrograma *p) {
    char buf[32];
    snprintf(buf, sizeof(buf), "t%d", p->cont_temp++);
    return tac_str(p, buf);
}

const char *tac_novo_rotulo(TacPrograma *p) {
    char buf[32];
    snprintf(buf, sizeof(buf), "L%d", p->cont_rotulo++);
    return tac_str(p, buf);
}

TacInstr *tac_emite(TacPrograma *p, TacOp cat, const char *op,
                    const char *res, const char *arg1, const char *arg2) {
    TacInstr *in = calloc(1, sizeof(TacInstr));
    if (in == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente para o TAC.\n");
        exit(1);
    }
    in->cat = cat;
    in->op = op;
    in->res = res;
    in->arg1 = arg1;
    in->arg2 = arg2;

    if (p->fim == NULL) {
        p->inicio = p->fim = in;
    } else {
        p->fim->prox = in;
        p->fim = in;
    }
    return in;
}

// Base de expressoes; a Frente A expande declaracoes, atribuicoes e operadores
const char *tac_gera_expr(TacPrograma *p, const No *n) {
    if (n == NULL) {
        return NULL;
    }
    char buf[64];
    switch (n->cat) {
        case NO_ID:
        case NO_LIT_CHAR:
        case NO_LIT_STRING:
            return n->valor.s;
        case NO_LIT_INT:
            snprintf(buf, sizeof(buf), "%d", n->valor.i);
            return tac_str(p, buf);
        case NO_LIT_FLOAT:
            snprintf(buf, sizeof(buf), "%g", n->valor.f);
            return tac_str(p, buf);
        default:
            return NULL;
    }
}

void tac_gera_no(TacPrograma *p, const No *n) {
    for (; n != NULL; n = n->prox) {
        switch (n->cat) {
            case NO_PROGRAMA:
            case NO_BLOCO:
                tac_gera_no(p, n->filhos[0]);
                break;
            case NO_VAZIO:
                break;
            default:
                (void)tac_gera_expr(p, n);
                break;
        }
    }
}

TacPrograma *tac_gera(const No *raiz) {
    TacPrograma *p = tac_cria();
    tac_gera_no(p, raiz);
    return p;
}

void tac_imprime(const TacPrograma *p, FILE *saida) {
    if (p == NULL) {
        return;
    }
    for (const TacInstr *in = p->inicio; in != NULL; in = in->prox) {
        switch (in->cat) {
            case TAC_BEGIN_FUNC:
                fprintf(saida, "func %s:\n", in->res);
                break;
            case TAC_END_FUNC:
                fprintf(saida, "endfunc %s\n", in->res);
                break;
            case TAC_LABEL:
                fprintf(saida, "%s:\n", in->res);
                break;
            case TAC_PARAM:
                fprintf(saida, "  param %s\n", in->res);
                break;
            case TAC_COPY:
                fprintf(saida, "  %s = %s\n", in->res, in->arg1);
                break;
            case TAC_BINARIO:
                fprintf(saida, "  %s = %s %s %s\n", in->res, in->arg1, in->op, in->arg2);
                break;
            case TAC_UNARIO:
                fprintf(saida, "  %s = %s %s\n", in->res, in->op, in->arg1);
                break;
            case TAC_GOTO:
                fprintf(saida, "  goto %s\n", in->res);
                break;
            case TAC_IFZ:
                fprintf(saida, "  ifz %s goto %s\n", in->arg1, in->res);
                break;
            case TAC_IFNZ:
                fprintf(saida, "  ifnz %s goto %s\n", in->arg1, in->res);
                break;
            case TAC_ARG:
                fprintf(saida, "  arg %s\n", in->arg1);
                break;
            case TAC_CALL:
                if (in->res != NULL) {
                    fprintf(saida, "  %s = call %s, %s\n", in->res, in->arg1, in->arg2);
                } else {
                    fprintf(saida, "  call %s, %s\n", in->arg1, in->arg2);
                }
                break;
            case TAC_RETURN:
                if (in->arg1 != NULL) {
                    fprintf(saida, "  return %s\n", in->arg1);
                } else {
                    fprintf(saida, "  return\n");
                }
                break;
            case TAC_PRINT:
                fprintf(saida, "  print %s\n", in->arg1);
                break;
        }
    }
}
