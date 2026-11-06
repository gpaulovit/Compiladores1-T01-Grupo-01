
#include <stdio.h>
#include "gerador_ts.h"
#include <string.h>

/* Contexto da geração de código. */
typedef struct {
    FILE *saida;
    int indentacao;
} GeradorTS;

/* Converte os tipos semânticos para tipos TypeScript. */
static const char *gerar_tipo(ExpType tipo) {
    switch (tipo) {
        case EXP_TYPE_INT:
        case EXP_TYPE_FLOAT:
            return "number";

        case EXP_TYPE_VOID:
            return "void";

        default:
            return "any";
    }
}

/* Emite a indentação atual. */
static void emitir_indentacao(GeradorTS *g) {
    for (int i = 0; i < g->indentacao; i++) {
        fprintf(g->saida, "    ");
    }
}

/* Gera uma expressão recursivamente. */
static void gerar_expressao(GeradorTS *g, const No *no);

/* Gera os argumentos de uma chamada de função. */
static void gerar_argumentos(GeradorTS *g, const No *arg) {
    int primeiro = 1;

    for (; arg != NULL; arg = arg->prox) {
        if (!primeiro) {
            fprintf(g->saida, ", ");
        }

        gerar_expressao(g, arg);
        primeiro = 0;
    }
}

/* Gera uma expressão recursivamente a partir da AST. */
static void gerar_expressao(GeradorTS *g, const No *no) {
    if (no == NULL) {
        return;
    }

    switch (no->cat) {
        case NO_ID:
            fprintf(g->saida, "%s", no->valor.s);
            break;

        case NO_LIT_INT:
            fprintf(g->saida, "%d", no->valor.i);
            break;

        case NO_LIT_FLOAT:
            fprintf(g->saida, "%.17g", no->valor.f);
            break;

        case NO_LIT_CHAR:
        case NO_LIT_STRING:
            fprintf(g->saida, "%s", no->valor.s);
            break;

        case NO_BINARIO:
        if (no->op != NULL &&
            (strcmp(no->op, "==") == 0 ||
            strcmp(no->op, "!=") == 0 ||
            strcmp(no->op, "<") == 0 ||
            strcmp(no->op, ">") == 0 ||
            strcmp(no->op, "<=") == 0 ||
            strcmp(no->op, ">=") == 0)) {

            fprintf(g->saida, "Number((");
            gerar_expressao(g, no->filhos[0]);
            fprintf(g->saida, " %s ", no->op);
            gerar_expressao(g, no->filhos[1]);
            fprintf(g->saida, "))");

        } else if (no->op != NULL &&
            (strcmp(no->op, "&&") == 0 ||
            strcmp(no->op, "||") == 0)) {
 
            fprintf(g->saida, "Number(Boolean(");
            gerar_expressao(g, no->filhos[0]);
            fprintf(g->saida, ") %s Boolean(", no->op);
            gerar_expressao(g, no->filhos[1]);
            fprintf(g->saida, "))");

        } else if (no->op != NULL &&
            strcmp(no->op, "/") == 0 &&
            no->filhos[0] != NULL &&
            no->filhos[1] != NULL &&
            no->filhos[0]->tipo == EXP_TYPE_INT &&
            no->filhos[1]->tipo == EXP_TYPE_INT) {

        fprintf(g->saida, "Math.trunc((");
        gerar_expressao(g, no->filhos[0]);
        fprintf(g->saida, ") / (");
        gerar_expressao(g, no->filhos[1]);
        fprintf(g->saida, "))");

        } else {
            fprintf(g->saida, "(");
            gerar_expressao(g, no->filhos[0]);
            fprintf(g->saida, " %s ", no->op);
            gerar_expressao(g, no->filhos[1]);
            fprintf(g->saida, ")");
        }
        break;

        case NO_UNARIO:
        if (strcmp(no->op, "!") == 0) {
            fprintf(g->saida, "Number(!(");
            gerar_expressao(g, no->filhos[0]);
            fprintf(g->saida, "))");
        } else {
            fprintf(g->saida, "(%s", no->op);
            gerar_expressao(g, no->filhos[0]);
            fprintf(g->saida, ")");
        }
        break;

        case NO_PRE_INCDEC:
            fprintf(g->saida, "(%s", no->op);
            gerar_expressao(g, no->filhos[0]);
            fprintf(g->saida, ")");
            break;

        case NO_POS_INCDEC:
            fprintf(g->saida, "(");
            gerar_expressao(g, no->filhos[0]);
            fprintf(g->saida, "%s)", no->op);
            break;

        case NO_ATRIBUICAO:
            /*
             * Divisão composta entre inteiros também precisa
             * truncar o resultado.
             */
            if (no->op != NULL &&
                no->op[0] == '/' &&
                no->op[1] == '=' &&
                no->op[2] == '\0' &&
                no->filhos[0] != NULL &&
                no->filhos[1] != NULL &&
                no->filhos[0]->tipo == EXP_TYPE_INT &&
                no->filhos[1]->tipo == EXP_TYPE_INT) {

                fprintf(g->saida, "(");
                gerar_expressao(g, no->filhos[0]);
                fprintf(g->saida, " = Math.trunc(");
                gerar_expressao(g, no->filhos[0]);
                fprintf(g->saida, " / ");
                gerar_expressao(g, no->filhos[1]);
                fprintf(g->saida, "))");

            } else {
                fprintf(g->saida, "(");
                gerar_expressao(g, no->filhos[0]);
                fprintf(g->saida, " %s ", no->op);
                gerar_expressao(g, no->filhos[1]);
                fprintf(g->saida, ")");
            }
            break;

        case NO_CHAMADA:
            fprintf(g->saida, "%s(", no->valor.s);
            gerar_argumentos(g, no->filhos[0]);
            fprintf(g->saida, ")");
            break;

        default:
            fprintf(g->saida, "/* expressao nao suportada */");
            break;
    }
}

/* Converte o resultado de uma expressão em condição booleana. */
static void gerar_condicao(GeradorTS *g, const No *no) {
    fprintf(g->saida, "Boolean(");
    gerar_expressao(g, no);
    fprintf(g->saida, ")");
}

/* Gera a declaração de uma variável. */
static void gerar_declaracao_variavel(GeradorTS *g, const No *no) {
    emitir_indentacao(g);

    fprintf(g->saida, "let %s: %s",
            no->valor.s,
            gerar_tipo(no->tipo));

    if (no->filhos[0] != NULL) {
        fprintf(g->saida, " = ");
        gerar_expressao(g, no->filhos[0]);
    }

    fprintf(g->saida, ";\n");
}

/* Gera os parâmetros de uma função. */
static void gerar_parametros(GeradorTS *g, const No *param) {
    int primeiro = 1;

    for (; param != NULL; param = param->prox) {
        if (!primeiro) {
            fprintf(g->saida, ", ");
        }

        fprintf(g->saida, "%s: %s",
                param->valor.s,
                gerar_tipo(param->tipo));

        primeiro = 0;
    }
}
/* Declaração antecipada para permitir a geração recursiva de comandos. */
static void gerar_comando(GeradorTS *g, const No *no);

/*
 * Gera o conteúdo de um corpo de bloco.
 *
 * Se o nó recebido já for NO_BLOCO, percorre suas instruções.
 * Caso contrário, gera o próprio comando como corpo.
 */
static void gerar_conteudo_corpo(GeradorTS *g, const No *corpo) {
    if (corpo == NULL) {
        return;
    }

    if (corpo->cat == NO_BLOCO) {
        for (const No *comando = corpo->filhos[0];
             comando != NULL;
             comando = comando->prox) {
            gerar_comando(g, comando);
        }
    } else {
        gerar_comando(g, corpo);
    }
}

/* Gera um corpo de código delimitado por chaves. */
static void gerar_corpo(GeradorTS *g, const No *corpo) {
    fprintf(g->saida, "{\n");

    g->indentacao++;
    gerar_conteudo_corpo(g, corpo);
    g->indentacao--;

    emitir_indentacao(g);
    fprintf(g->saida, "}\n");
}

/*
 * Gera a inicialização ou o passo de um comando for.
 * Esses elementos não devem receber ponto e vírgula aqui,
 * pois o próprio cabeçalho do for controla a separação.
 */
static void gerar_elemento_for(GeradorTS *g, const No *no) {
    if (no == NULL || no->cat == NO_VAZIO) {
        return;
    }

    if (no->cat == NO_DECL_VAR) {
        fprintf(g->saida, "let %s: %s",
                no->valor.s,
                gerar_tipo(no->tipo));

        if (no->filhos[0] != NULL) {
            fprintf(g->saida, " = ");
            gerar_expressao(g, no->filhos[0]);
        }
    } else {
        gerar_expressao(g, no);
    }
}

/* Gera um comando ou estrutura de controle. */
static void gerar_comando(GeradorTS *g, const No *no) {
    if (no == NULL) {
        return;
    }

    switch (no->cat) {
        case NO_BLOCO:
            gerar_corpo(g, no);
            break;

        case NO_DECL_VAR:
            gerar_declaracao_variavel(g, no);
            break;

        case NO_ATRIBUICAO:
        case NO_PRE_INCDEC:
        case NO_POS_INCDEC:
        case NO_CHAMADA:
            emitir_indentacao(g);
            gerar_expressao(g, no);
            fprintf(g->saida, ";\n");
            break;

        case NO_RETURN:
            emitir_indentacao(g);
            fprintf(g->saida, "return");

            if (no->filhos[0] != NULL) {
                fprintf(g->saida, " ");
                gerar_expressao(g, no->filhos[0]);
            }

            fprintf(g->saida, ";\n");
            break;

        case NO_PRINT:
            emitir_indentacao(g);
            fprintf(g->saida, "console.log(");
            gerar_expressao(g, no->filhos[0]);
            fprintf(g->saida, ");\n");
            break;

        case NO_IF:
            emitir_indentacao(g);
            fprintf(g->saida, "if (");
            gerar_condicao(g, no->filhos[0]);
            fprintf(g->saida, ") ");

            gerar_corpo(g, no->filhos[1]);

            if (no->filhos[2] != NULL) {
                emitir_indentacao(g);
                fprintf(g->saida, "else ");
                gerar_corpo(g, no->filhos[2]);
            }
            break;

        case NO_WHILE:
            emitir_indentacao(g);
            fprintf(g->saida, "while (");
            gerar_condicao(g, no->filhos[0]);
            fprintf(g->saida, ") ");

            gerar_corpo(g, no->filhos[1]);
            break;

        case NO_DO_WHILE:
            emitir_indentacao(g);
            fprintf(g->saida, "do ");

            gerar_corpo(g, no->filhos[0]);

            emitir_indentacao(g);
            fprintf(g->saida, "while (");
            gerar_condicao(g, no->filhos[1]);
            fprintf(g->saida, ");\n");
            break;

        case NO_FOR:
            emitir_indentacao(g);
            fprintf(g->saida, "for (");

            gerar_elemento_for(g, no->filhos[0]);
            fprintf(g->saida, "; ");

            if (no->filhos[1] != NULL) {
                gerar_condicao(g, no->filhos[1]);
            }

            fprintf(g->saida, "; ");

            gerar_elemento_for(g, no->filhos[2]);
            fprintf(g->saida, ") ");

            gerar_corpo(g, no->filhos[3]);
            break;

        case NO_VAZIO:
            emitir_indentacao(g);
            fprintf(g->saida, ";\n");
            break;

        default:
            emitir_indentacao(g);
            fprintf(g->saida, "/* comando nao suportado */\n");
            break;
    }
}

/* Gera a assinatura e o corpo de uma função. */
static void gerar_funcao(GeradorTS *g, const No *no) {
    emitir_indentacao(g);

    fprintf(g->saida, "function %s(", no->valor.s);

    gerar_parametros(g, no->filhos[0]);

    fprintf(g->saida, "): %s ", gerar_tipo(no->tipo));

    gerar_corpo(g, no->filhos[1]);

    fprintf(g->saida, "\n");
}

/* Percorre uma lista de declarações globais. */
static void gerar_lista(GeradorTS *g, const No *no) {
    for (; no != NULL; no = no->prox) {
        switch (no->cat) {
            case NO_DECL_VAR:
                gerar_declaracao_variavel(g, no);
                break;

            case NO_FUNCAO:
                gerar_funcao(g, no);
                break;

            default:
                break;
        }
    }
}

/* Verifica se existe uma função main sem parâmetros. */
static int possui_main_sem_parametros(const No *no) {
    for (; no != NULL; no = no->prox) {
        if (no->cat == NO_FUNCAO &&
            strcmp(no->valor.s, "main") == 0) {
            return no->filhos[0] == NULL;
        }
    }

    return 0;
}

/* Ponto de entrada público do gerador. */
void gerar_typescript(const No *raiz, FILE *saida) {
    if (raiz == NULL || saida == NULL) {
        return;
    }

    GeradorTS g = {
        .saida = saida,
        .indentacao = 0
    };

    if (raiz->cat != NO_PROGRAMA) {
        return;
    }

    gerar_lista(&g, raiz->filhos[0]);

    if (possui_main_sem_parametros(raiz->filhos[0])) {
        fprintf(saida, "main();\n");
    }
}
