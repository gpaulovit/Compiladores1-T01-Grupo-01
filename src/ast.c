
#include <stdlib.h>
#include "ast.h"

No *ast_cria(Categoria cat, int linha, No *f0, No *f1, No *f2, No *f3) {
    No *n = calloc(1, sizeof(No));
    if (n == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente para a AST.\n");
        exit(1);
    }
    n->cat = cat;
    n->linha = linha;
    n->tipo = EXP_TYPE_ERROR;
    n->filhos[0] = f0;
    n->filhos[1] = f1;
    n->filhos[2] = f2;
    n->filhos[3] = f3;
    return n;
}

No *ast_op(Categoria cat, const char *op, int linha, No *f0, No *f1) {
    No *n = ast_cria(cat, linha, f0, f1, NULL, NULL);
    n->op = op;
    return n;
}

No *ast_texto(Categoria cat, char *s, int linha) {
    No *n = ast_cria(cat, linha, NULL, NULL, NULL, NULL);
    n->valor.s = s;
    return n;
}

No *ast_int(int v, int linha) {
    No *n = ast_cria(NO_LIT_INT, linha, NULL, NULL, NULL, NULL);
    n->valor.i = v;
    n->tipo = EXP_TYPE_INT;
    return n;
}

No *ast_float(double v, int linha) {
    No *n = ast_cria(NO_LIT_FLOAT, linha, NULL, NULL, NULL, NULL);
    n->valor.f = v;
    n->tipo = EXP_TYPE_FLOAT;
    return n;
}

No *ast_lista_adiciona(No *lista, No *novo) {
    if (lista == NULL) {
        return novo;
    }
    No *fim = lista;
    while (fim->prox != NULL) {
        fim = fim->prox;
    }
    fim->prox = novo;
    return lista;
}

static const char *NOMES_CATEGORIA[] = {
    [NO_PROGRAMA]   = "Programa",
    [NO_FUNCAO]     = "Funcao",
    [NO_PARAM]      = "Param",
    [NO_DECL_VAR]   = "DeclVar",
    [NO_BLOCO]      = "Bloco",
    [NO_IF]         = "If",
    [NO_WHILE]      = "While",
    [NO_DO_WHILE]   = "DoWhile",
    [NO_FOR]        = "For",
    [NO_RETURN]     = "Return",
    [NO_PRINT]      = "Print",
    [NO_VAZIO]      = "Vazio",
    [NO_ATRIBUICAO] = "Atribuicao",
    [NO_BINARIO]    = "Binario",
    [NO_UNARIO]     = "Unario",
    [NO_PRE_INCDEC] = "PreIncDec",
    [NO_POS_INCDEC] = "PosIncDec",
    [NO_CHAMADA]    = "Chamada",
    [NO_ID]         = "Id",
    [NO_LIT_INT]    = "LitInt",
    [NO_LIT_FLOAT]  = "LitFloat",
    [NO_LIT_CHAR]   = "LitChar",
    [NO_LIT_STRING] = "LitString",
};

const char *ast_nome_categoria(Categoria cat) {
    return NOMES_CATEGORIA[cat];
}

static const char *nome_tipo(ExpType tipo) {
    switch (tipo) {
        case EXP_TYPE_INT:    return "int";
        case EXP_TYPE_FLOAT:  return "float";
        case EXP_TYPE_VOID:   return "void";
        case EXP_TYPE_STRING: return "string";
        case EXP_TYPE_CHAR:   return "char";
        default:              return "erro";
    }
}

/* Comandos de controle e o programa não têm tipo; os demais têm. */
static int tem_tipo(Categoria cat) {
    return cat == NO_FUNCAO || cat == NO_PARAM || cat == NO_DECL_VAR || cat >= NO_ATRIBUICAO;
}

static int guarda_texto(Categoria cat) {
    switch (cat) {
        case NO_FUNCAO: case NO_PARAM: case NO_DECL_VAR: case NO_CHAMADA:
        case NO_ID: case NO_LIT_CHAR: case NO_LIT_STRING:
            return 1;
        default:
            return 0;
    }
}

/* Imprime n e os irmãos encadeados em prox, todos no mesmo nível. */
static void imprime_lista(const No *n, int nivel, FILE *saida) {
    for (; n != NULL; n = n->prox) {
        fprintf(saida, "%*s%s", nivel * 2, "", ast_nome_categoria(n->cat));

        if (n->op != NULL)               fprintf(saida, " %s", n->op);
        if (guarda_texto(n->cat))        fprintf(saida, " %s", n->valor.s);
        else if (n->cat == NO_LIT_INT)   fprintf(saida, " %d", n->valor.i);
        else if (n->cat == NO_LIT_FLOAT) fprintf(saida, " %g", n->valor.f);
        if (tem_tipo(n->cat))            fprintf(saida, " (%s)", nome_tipo(n->tipo));
        fprintf(saida, " @%d\n", n->linha);

        /* Filho ausente antes de um presente */
        int ultimo = AST_MAX_FILHOS - 1;
        while (ultimo >= 0 && n->filhos[ultimo] == NULL) {
            ultimo--;
        }
        for (int i = 0; i <= ultimo; i++) {
            if (n->filhos[i] != NULL) {
                imprime_lista(n->filhos[i], nivel + 1, saida);
            } else {
                fprintf(saida, "%*s-\n", (nivel + 1) * 2, "");
            }
        }
    }
}

void ast_imprime(const No *raiz, FILE *saida) {
    imprime_lista(raiz, 0, saida);
}

void ast_libera(No *raiz) {
    while (raiz != NULL) {
        No *prox = raiz->prox;
        for (int i = 0; i < AST_MAX_FILHOS; i++) {
            ast_libera(raiz->filhos[i]);
        }
        if (guarda_texto(raiz->cat)) {
            free(raiz->valor.s);
        }
        free(raiz);
        raiz = prox;
    }
}
