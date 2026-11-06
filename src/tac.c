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

//Avaliacao em curto-circuito para && e ||
static const char *gera_curto_circuito(TacPrograma *p, const No *n) {
    const char *res = tac_novo_temp(p);
    const char *rot_curto = tac_novo_rotulo(p);
    const char *rot_fim = tac_novo_rotulo(p);
    int eh_and = (strcmp(n->op, "&&") == 0);

    const char *esq = tac_gera_expr(p, n->filhos[0]);
    tac_emite(p, eh_and ? TAC_IFZ : TAC_IFNZ, NULL, rot_curto, esq, NULL);

    const char *dir = tac_gera_expr(p, n->filhos[1]);
    tac_emite(p, eh_and ? TAC_IFZ : TAC_IFNZ, NULL, rot_curto, dir, NULL);

    tac_emite(p, TAC_COPY, NULL, res, eh_and ? "1" : "0", NULL);
    tac_emite(p, TAC_GOTO, NULL, rot_fim, NULL, NULL);

    tac_emite(p, TAC_LABEL, NULL, rot_curto, NULL, NULL);
    tac_emite(p, TAC_COPY, NULL, res, eh_and ? "0" : "1", NULL);

    tac_emite(p, TAC_LABEL, NULL, rot_fim, NULL, NULL);
    return res;
}

//Avalia todos os argumentos antes de emitir arg, evitando intercalar chamadas aninhadas
static const char *gera_chamada(TacPrograma *p, const No *n) {
    int qtd = 0;
    for (const No *a = n->filhos[0]; a != NULL; a = a->prox) {
        qtd++;
    }

    const char **vals = NULL;
    if (qtd > 0) {
        vals = malloc(qtd * sizeof(const char *));
        if (vals == NULL) {
            fprintf(stderr, "Erro: memoria insuficiente para o TAC.\n");
            exit(1);
        }
        int i = 0;
        for (const No *a = n->filhos[0]; a != NULL; a = a->prox) {
            vals[i++] = tac_gera_expr(p, a);
        }
        for (int j = 0; j < qtd; j++) {
            tac_emite(p, TAC_ARG, NULL, NULL, vals[j], NULL);
        }
        free(vals);
    }

    char buf[32];
    snprintf(buf, sizeof(buf), "%d", qtd);
    const char *qtd_str = tac_str(p, buf);

    if (n->tipo == EXP_TYPE_VOID) {
        tac_emite(p, TAC_CALL, NULL, NULL, n->valor.s, qtd_str);
        return NULL;
    }
    const char *res = tac_novo_temp(p);
    tac_emite(p, TAC_CALL, NULL, res, n->valor.s, qtd_str);
    return res;
}

static const char *le_variavel(TacPrograma *p, const char *nome) {
    const char *temp = tac_novo_temp(p);
    tac_emite(p, TAC_COPY, NULL, temp, nome, NULL);
    return temp;
}

const char *tac_gera_expr(TacPrograma *p, const No *n) {
    if (n == NULL) {
        return NULL;
    }
    char buf[64];
    switch (n->cat) {
        case NO_ATRIBUICAO: {
            const char *dest = n->filhos[0]->valor.s;
            const char *antigo = NULL;
            if (strcmp(n->op, "=") != 0) {
                antigo = le_variavel(p, dest);
            }
            const char *val = tac_gera_expr(p, n->filhos[1]);
            if (antigo != NULL) {
                char op[2] = { n->op[0], '\0' };
                const char *res = tac_novo_temp(p);
                tac_emite(p, TAC_BINARIO, tac_str(p, op), res, antigo, val);
                val = res;
            }
            tac_emite(p, TAC_COPY, NULL, dest, val, NULL);
            return val;
        }
        case NO_UNARIO: {
            const char *arg = tac_gera_expr(p, n->filhos[0]);
            const char *res = tac_novo_temp(p);
            tac_emite(p, TAC_UNARIO, n->op, res, arg, NULL);
            return res;
        }
        case NO_PRE_INCDEC:
        case NO_POS_INCDEC: {
            const char *dest = n->filhos[0]->valor.s;
            const char *antigo = le_variavel(p, dest);
            const char *novo = tac_novo_temp(p);
            const char *op = strcmp(n->op, "++") == 0 ? "+" : "-";
            tac_emite(p, TAC_BINARIO, op, novo, antigo, "1");
            tac_emite(p, TAC_COPY, NULL, dest, novo, NULL);
            return n->cat == NO_PRE_INCDEC ? novo : antigo;
        }
        case NO_BINARIO:
            if (strcmp(n->op, "&&") == 0 || strcmp(n->op, "||") == 0) {
                return gera_curto_circuito(p, n);
            }
            {
                const char *esq = tac_gera_expr(p, n->filhos[0]);
                const char *dir = tac_gera_expr(p, n->filhos[1]);
                const char *res = tac_novo_temp(p);
                tac_emite(p, TAC_BINARIO, n->op, res, esq, dir);
                return res;
            }
        case NO_CHAMADA:
            return gera_chamada(p, n);
        case NO_ID:
            return le_variavel(p, n->valor.s);
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

static void gera_funcao(TacPrograma *p, const No *n) {
    tac_emite(p, TAC_BEGIN_FUNC, NULL, n->valor.s, NULL, NULL);
    for (const No *param = n->filhos[0]; param != NULL; param = param->prox) {
        tac_emite(p, TAC_PARAM, NULL, param->valor.s, NULL, NULL);
    }
    tac_gera_no(p, n->filhos[1]);
    tac_emite(p, TAC_END_FUNC, NULL, n->valor.s, NULL, NULL);
}

static void gera_if(TacPrograma *p, const No *n) {
    const char *cond = tac_gera_expr(p, n->filhos[0]);
    if (n->filhos[2] == NULL) {
        const char *rot_fim = tac_novo_rotulo(p);
        tac_emite(p, TAC_IFZ, NULL, rot_fim, cond, NULL);
        tac_gera_no(p, n->filhos[1]);
        tac_emite(p, TAC_LABEL, NULL, rot_fim, NULL, NULL);
    } else {
        const char *rot_senao = tac_novo_rotulo(p);
        const char *rot_fim = tac_novo_rotulo(p);
        tac_emite(p, TAC_IFZ, NULL, rot_senao, cond, NULL);
        tac_gera_no(p, n->filhos[1]);
        tac_emite(p, TAC_GOTO, NULL, rot_fim, NULL, NULL);
        tac_emite(p, TAC_LABEL, NULL, rot_senao, NULL, NULL);
        tac_gera_no(p, n->filhos[2]);
        tac_emite(p, TAC_LABEL, NULL, rot_fim, NULL, NULL);
    }
}

static void gera_while(TacPrograma *p, const No *n) {
    const char *rot_inicio = tac_novo_rotulo(p);
    const char *rot_fim = tac_novo_rotulo(p);
    tac_emite(p, TAC_LABEL, NULL, rot_inicio, NULL, NULL);
    const char *cond = tac_gera_expr(p, n->filhos[0]);
    tac_emite(p, TAC_IFZ, NULL, rot_fim, cond, NULL);
    tac_gera_no(p, n->filhos[1]);
    tac_emite(p, TAC_GOTO, NULL, rot_inicio, NULL, NULL);
    tac_emite(p, TAC_LABEL, NULL, rot_fim, NULL, NULL);
}

static void gera_do_while(TacPrograma *p, const No *n) {
    const char *rot_inicio = tac_novo_rotulo(p);
    tac_emite(p, TAC_LABEL, NULL, rot_inicio, NULL, NULL);
    tac_gera_no(p, n->filhos[0]);
    const char *cond = tac_gera_expr(p, n->filhos[1]);
    tac_emite(p, TAC_IFNZ, NULL, rot_inicio, cond, NULL);
}

static void gera_for(TacPrograma *p, const No *n) {
    const char *rot_inicio = tac_novo_rotulo(p);
    const char *rot_fim = (n->filhos[1] != NULL) ? tac_novo_rotulo(p) : NULL;

    if (n->filhos[0] != NULL) {
        (void)tac_gera_expr(p, n->filhos[0]);
    }
    tac_emite(p, TAC_LABEL, NULL, rot_inicio, NULL, NULL);
    if (n->filhos[1] != NULL) {
        const char *cond = tac_gera_expr(p, n->filhos[1]);
        tac_emite(p, TAC_IFZ, NULL, rot_fim, cond, NULL);
    }
    tac_gera_no(p, n->filhos[3]);
    if (n->filhos[2] != NULL) {
        (void)tac_gera_expr(p, n->filhos[2]);
    }
    tac_emite(p, TAC_GOTO, NULL, rot_inicio, NULL, NULL);
    if (rot_fim != NULL) {
        tac_emite(p, TAC_LABEL, NULL, rot_fim, NULL, NULL);
    }
}

void tac_gera_no(TacPrograma *p, const No *n) {
    for (; n != NULL; n = n->prox) {
        switch (n->cat) {
            case NO_PROGRAMA:
            case NO_BLOCO:
                tac_gera_no(p, n->filhos[0]);
                break;
            case NO_FUNCAO:
                gera_funcao(p, n);
                break;
            case NO_DECL_VAR:
                if (n->filhos[0] != NULL) {
                    const char *val = tac_gera_expr(p, n->filhos[0]);
                    if (val != NULL) {
                        tac_emite(p, TAC_COPY, NULL, n->valor.s, val, NULL);
                    }
                }
                break;
            case NO_IF:
                gera_if(p, n);
                break;
            case NO_WHILE:
                gera_while(p, n);
                break;
            case NO_DO_WHILE:
                gera_do_while(p, n);
                break;
            case NO_FOR:
                gera_for(p, n);
                break;
            case NO_RETURN:
                if (n->filhos[0] != NULL) {
                    const char *val = tac_gera_expr(p, n->filhos[0]);
                    tac_emite(p, TAC_RETURN, NULL, NULL, val, NULL);
                } else {
                    tac_emite(p, TAC_RETURN, NULL, NULL, NULL, NULL);
                }
                break;
            case NO_PRINT:
                {
                    const char *val = tac_gera_expr(p, n->filhos[0]);
                    tac_emite(p, TAC_PRINT, NULL, NULL, val, NULL);
                }
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
