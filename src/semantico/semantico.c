#include "semantico.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int sem_errors;

static SymbolTable *symtab_ref = NULL;

static struct {
    char *name;
    DataType return_type;
    int is_active;
} current_func = {NULL, TYPE_VOID, 0};

static DataType converte_tipo_nome(const char *tipo) {
    if (strcmp(tipo, "int") == 0) {
        return TYPE_INT;
    }
    if (strcmp(tipo, "float") == 0) {
        return TYPE_FLOAT;
    }
    return TYPE_VOID;
}

void semantico_init(SymbolTable *table) {
    symtab_ref = table;
}

void semantico_checa_declaracao_var(const char *tipo, const char *nome, int linha) {
    if (strcmp(tipo, "void") == 0) {
        fprintf(stderr, "Erro semantico (linha %d): variavel '%s' nao pode ter tipo void.\n",
                linha, nome);
        sem_errors++;
        return;
    }

    if (symtab_ref == NULL) {
        return;
    }

    if (!symtab_insert_variable(symtab_ref, nome, converte_tipo_nome(tipo), linha)) {
        fprintf(stderr, "Erro semantico (linha %d): variavel '%s' ja declarada neste escopo.\n",
                linha, nome);
        sem_errors++;
    }
}

void semantico_checa_declaracao_var_init(const char *tipo, const char *nome, SemanticExpr exp, int linha) {
    semantico_checa_declaracao_var(tipo, nome, linha);

    if (exp.type == EXP_TYPE_VOID) {
        fprintf(stderr, "Erro semantico (linha %d): funcao void '%s' usada como valor.\n",
                linha, exp.func_name ? exp.func_name : "");
        sem_errors++;
    } else if (exp.type == EXP_TYPE_STRING) {
        fprintf(stderr, "Erro semantico (linha %d): string fora do print.\n", linha);
        sem_errors++;
    } else if (exp.type == EXP_TYPE_CHAR) {
        fprintf(stderr, "Erro semantico (linha %d): char fora do print.\n", linha);
        sem_errors++;
    } else if (strcmp(tipo, "int") == 0 && exp.type == EXP_TYPE_FLOAT) {
        fprintf(stderr, "Erro semantico (linha %d): atribuicao incompativel para '%s'.\n",
                linha, nome);
        sem_errors++;
    }
}

void semantico_checa_declaracao_param(const char *tipo, const char *nome, int linha) {
    if (strcmp(tipo, "void") == 0) {
        fprintf(stderr, "Erro semantico (linha %d): parametro '%s' nao pode ter tipo void.\n",
                linha, nome);
        sem_errors++;
    }
}

void semantico_define_funcao_inicio(const char *tipo_ret, const char *nome, int linha) {
    free(current_func.name);
    current_func.name = strdup(nome);
    current_func.return_type = converte_tipo_nome(tipo_ret);
    current_func.is_active = 1;
}

void semantico_define_funcao_fim(void) {
    free(current_func.name);
    current_func.name = NULL;
    current_func.is_active = 0;
}

void semantico_checa_retorno(SemanticExpr exp, int linha) {
    if (!current_func.is_active) {
        fprintf(stderr, "Erro semantico (linha %d): comando return fora de funcao.\n", linha);
        sem_errors++;
        return;
    }

    if (current_func.return_type == TYPE_VOID) {
        fprintf(stderr, "Erro semantico (linha %d): funcao void '%s' nao pode retornar valor.\n",
                linha, current_func.name ? current_func.name : "");
        sem_errors++;
        return;
    }

    if (exp.type == EXP_TYPE_VOID) {
        fprintf(stderr, "Erro semantico (linha %d): funcao void '%s' usada como valor.\n",
                linha, exp.func_name ? exp.func_name : "");
        sem_errors++;
        return;
    }

    if (exp.type == EXP_TYPE_STRING) {
        fprintf(stderr, "Erro semantico (linha %d): string fora do print.\n", linha);
        sem_errors++;
        return;
    }

    if (exp.type == EXP_TYPE_CHAR) {
        fprintf(stderr, "Erro semantico (linha %d): char fora do print.\n", linha);
        sem_errors++;
        return;
    }

    if (current_func.return_type == TYPE_INT && exp.type == EXP_TYPE_FLOAT) {
        fprintf(stderr, "Erro semantico (linha %d): retorno incompativel na funcao '%s'.\n",
                linha, current_func.name ? current_func.name : "");
        sem_errors++;
        return;
    }
}

void semantico_checa_retorno_vazio(int linha) {
    if (!current_func.is_active) {
        fprintf(stderr, "Erro semantico (linha %d): comando return fora de funcao.\n", linha);
        sem_errors++;
        return;
    }

    if (current_func.return_type != TYPE_VOID) {
        fprintf(stderr, "Erro semantico (linha %d): funcao '%s' deve retornar valor.\n",
                linha, current_func.name ? current_func.name : "");
        sem_errors++;
    }
}

SemanticExpr semantico_checa_atribuicao(const char *nome, SemanticExpr exp, int linha) {
    SemanticExpr res;
    res.line = linha;
    res.func_name = NULL;
    res.type = EXP_TYPE_ERROR;

    if (symtab_ref == NULL) {
        return res;
    }

    const Symbol *s = symtab_lookup(symtab_ref, nome);
    if (s == NULL) {
        fprintf(stderr, "Erro semantico (linha %d): variavel '%s' nao declarada.\n", linha, nome);
        sem_errors++;
        return res;
    }

    if (s->kind == SYMBOL_FUNCTION) {
        fprintf(stderr, "Erro semantico (linha %d): '%s' e uma funcao e nao pode receber atribuicao.\n",
                linha, nome);
        sem_errors++;
        return res;
    }

    if (exp.type == EXP_TYPE_VOID) {
        fprintf(stderr, "Erro semantico (linha %d): funcao void '%s' usada como valor.\n",
                linha, exp.func_name ? exp.func_name : "");
        sem_errors++;
        return res;
    }

    if (exp.type == EXP_TYPE_STRING) {
        fprintf(stderr, "Erro semantico (linha %d): string fora do print.\n", linha);
        sem_errors++;
        return res;
    }

    if (exp.type == EXP_TYPE_CHAR) {
        fprintf(stderr, "Erro semantico (linha %d): char fora do print.\n", linha);
        sem_errors++;
        return res;
    }

    if (s->type == TYPE_INT && exp.type == EXP_TYPE_FLOAT) {
        fprintf(stderr, "Erro semantico (linha %d): atribuicao incompativel para '%s'.\n",
                linha, nome);
        sem_errors++;
        return res;
    }

    res.type = (s->type == TYPE_INT) ? EXP_TYPE_INT : EXP_TYPE_FLOAT;
    return res;
}

SemanticExpr semantico_checa_atribuicao_composta(const char *nome, const char *op, SemanticExpr exp, int linha) {
    (void)op;
    return semantico_checa_atribuicao(nome, exp, linha);
}

SemanticExpr semantico_checa_identificador(const char *nome, int linha) {
    SemanticExpr res;
    res.line = linha;
    res.func_name = NULL;
    res.type = EXP_TYPE_ERROR;

    if (symtab_ref == NULL) {
        return res;
    }

    const Symbol *s = symtab_lookup(symtab_ref, nome);
    if (s == NULL) {
        fprintf(stderr, "Erro semantico (linha %d): variavel '%s' nao declarada.\n", linha, nome);
        sem_errors++;
        return res;
    }

    if (s->kind == SYMBOL_FUNCTION) {
        res.type = (s->type == TYPE_VOID) ? EXP_TYPE_VOID : ((s->type == TYPE_INT) ? EXP_TYPE_INT : EXP_TYPE_FLOAT);
        res.func_name = strdup(nome);
        return res;
    }

    if (s->type == TYPE_INT) {
        res.type = EXP_TYPE_INT;
    } else if (s->type == TYPE_FLOAT) {
        res.type = EXP_TYPE_FLOAT;
    } else {
        res.type = EXP_TYPE_VOID;
        res.func_name = strdup(nome);
    }

    return res;
}

SemanticExpr semantico_literal_int(int linha) {
    SemanticExpr res;
    res.line = linha;
    res.type = EXP_TYPE_INT;
    res.func_name = NULL;
    return res;
}

SemanticExpr semantico_literal_float(int linha) {
    SemanticExpr res;
    res.line = linha;
    res.type = EXP_TYPE_FLOAT;
    res.func_name = NULL;
    return res;
}

SemanticExpr semantico_literal_string(int linha) {
    SemanticExpr res;
    res.line = linha;
    res.type = EXP_TYPE_STRING;
    res.func_name = NULL;
    return res;
}

SemanticExpr semantico_literal_char(int linha) {
    SemanticExpr res;
    res.line = linha;
    res.type = EXP_TYPE_CHAR;
    res.func_name = NULL;
    return res;
}

static int valida_operandos_geral(SemanticExpr esq, SemanticExpr dir, int linha) {
    int ok = 1;

    if (esq.type == EXP_TYPE_VOID) {
        fprintf(stderr, "Erro semantico (linha %d): funcao void '%s' usada como valor.\n",
                linha, esq.func_name ? esq.func_name : "");
        sem_errors++;
        ok = 0;
    } else if (esq.type == EXP_TYPE_STRING) {
        fprintf(stderr, "Erro semantico (linha %d): string fora do print.\n", linha);
        sem_errors++;
        ok = 0;
    } else if (esq.type == EXP_TYPE_CHAR) {
        fprintf(stderr, "Erro semantico (linha %d): char fora do print.\n", linha);
        sem_errors++;
        ok = 0;
    }

    if (dir.type == EXP_TYPE_VOID) {
        fprintf(stderr, "Erro semantico (linha %d): funcao void '%s' usada como valor.\n",
                linha, dir.func_name ? dir.func_name : "");
        sem_errors++;
        ok = 0;
    } else if (dir.type == EXP_TYPE_STRING) {
        fprintf(stderr, "Erro semantico (linha %d): string fora do print.\n", linha);
        sem_errors++;
        ok = 0;
    } else if (dir.type == EXP_TYPE_CHAR) {
        fprintf(stderr, "Erro semantico (linha %d): char fora do print.\n", linha);
        sem_errors++;
        ok = 0;
    }

    return ok;
}

SemanticExpr semantico_checa_op_aritmetico(const char *op, SemanticExpr esq, SemanticExpr dir, int linha) {
    (void)op;
    SemanticExpr res;
    res.line = linha;
    res.func_name = NULL;
    res.type = EXP_TYPE_ERROR;

    if (!valida_operandos_geral(esq, dir, linha)) {
        return res;
    }

    if (esq.type == EXP_TYPE_FLOAT || dir.type == EXP_TYPE_FLOAT) {
        res.type = EXP_TYPE_FLOAT;
    } else {
        res.type = EXP_TYPE_INT;
    }

    return res;
}

SemanticExpr semantico_checa_op_relacional(const char *op, SemanticExpr esq, SemanticExpr dir, int linha) {
    (void)op;
    SemanticExpr res;
    res.line = linha;
    res.func_name = NULL;
    res.type = EXP_TYPE_ERROR;

    if (!valida_operandos_geral(esq, dir, linha)) {
        return res;
    }

    res.type = EXP_TYPE_INT;
    return res;
}

SemanticExpr semantico_checa_op_logico(const char *op, SemanticExpr esq, SemanticExpr dir, int linha) {
    (void)op;
    SemanticExpr res;
    res.line = linha;
    res.func_name = NULL;
    res.type = EXP_TYPE_ERROR;

    if (!valida_operandos_geral(esq, dir, linha)) {
        return res;
    }

    res.type = EXP_TYPE_INT;
    return res;
}

SemanticExpr semantico_checa_op_unario(const char *op, SemanticExpr sub, int linha) {
    SemanticExpr res;
    res.line = linha;
    res.func_name = NULL;
    res.type = EXP_TYPE_ERROR;

    if (sub.type == EXP_TYPE_VOID) {
        fprintf(stderr, "Erro semantico (linha %d): funcao void '%s' usada como valor.\n",
                linha, sub.func_name ? sub.func_name : "");
        sem_errors++;
        return res;
    }

    if (sub.type == EXP_TYPE_STRING) {
        fprintf(stderr, "Erro semantico (linha %d): string fora do print.\n", linha);
        sem_errors++;
        return res;
    }

    if (sub.type == EXP_TYPE_CHAR) {
        fprintf(stderr, "Erro semantico (linha %d): char fora do print.\n", linha);
        sem_errors++;
        return res;
    }

    if (strcmp(op, "!") == 0) {
        res.type = EXP_TYPE_INT;
    } else {
        res.type = (sub.type == EXP_TYPE_FLOAT) ? EXP_TYPE_FLOAT : EXP_TYPE_INT;
    }

    return res;
}

SemanticExpr semantico_checa_inc_dec(const char *nome, const char *op, int linha) {
    (void)op;
    SemanticExpr res;
    res.line = linha;
    res.func_name = NULL;
    res.type = EXP_TYPE_ERROR;

    if (symtab_ref == NULL) {
        return res;
    }

    const Symbol *s = symtab_lookup(symtab_ref, nome);
    if (s == NULL) {
        fprintf(stderr, "Erro semantico (linha %d): variavel '%s' nao declarada.\n", linha, nome);
        sem_errors++;
        return res;
    }

    if (s->kind == SYMBOL_FUNCTION) {
        fprintf(stderr, "Erro semantico (linha %d): '%s' e uma funcao e nao pode ser incrementada/decrementada.\n",
                linha, nome);
        sem_errors++;
        return res;
    }

    res.type = (s->type == TYPE_FLOAT) ? EXP_TYPE_FLOAT : EXP_TYPE_INT;
    return res;
}

SemanticExpr semantico_checa_chamada(const char *nome, SemanticArgs *args, int linha) {
    SemanticExpr res;
    res.line = linha;
    res.func_name = NULL;
    res.type = EXP_TYPE_ERROR;

    if (symtab_ref == NULL) {
        return res;
    }

    const Symbol *s = symtab_lookup(symtab_ref, nome);
    if (s == NULL) {
        fprintf(stderr, "Erro semantico (linha %d): funcao '%s' nao declarada.\n", linha, nome);
        sem_errors++;
        return res;
    }

    if (s->kind != SYMBOL_FUNCTION) {
        fprintf(stderr, "Erro semantico (linha %d): '%s' nao e uma funcao.\n", linha, nome);
        sem_errors++;
        return res;
    }

    size_t args_count = args ? args->count : 0;
    if (args_count != s->parameter_count) {
        fprintf(stderr, "Erro semantico (linha %d): numero de argumentos incompativel na chamada de '%s'.\n",
                linha, nome);
        sem_errors++;
    }

    size_t limit = args_count < s->parameter_count ? args_count : s->parameter_count;
    for (size_t i = 0; i < limit; i++) {
        ExpType arg_t = args->types[i];
        int arg_line = args->lines[i];

        if (arg_t == EXP_TYPE_VOID) {
            fprintf(stderr, "Erro semantico (linha %d): funcao void '%s' usada como valor.\n",
                    arg_line, args->func_names[i] ? args->func_names[i] : "");
            sem_errors++;
        } else if (arg_t == EXP_TYPE_STRING) {
            fprintf(stderr, "Erro semantico (linha %d): string fora do print.\n", arg_line);
            sem_errors++;
        } else if (arg_t == EXP_TYPE_CHAR) {
            fprintf(stderr, "Erro semantico (linha %d): char fora do print.\n", arg_line);
            sem_errors++;
        } else if (s->parameters[i].type == TYPE_INT && arg_t == EXP_TYPE_FLOAT) {
            fprintf(stderr, "Erro semantico (linha %d): tipo do argumento %zu incompativel na chamada de '%s'.\n",
                    arg_line, i + 1, nome);
            sem_errors++;
        }
    }

    if (s->type == TYPE_VOID) {
        res.type = EXP_TYPE_VOID;
        res.func_name = strdup(nome);
    } else if (s->type == TYPE_INT) {
        res.type = EXP_TYPE_INT;
    } else {
        res.type = EXP_TYPE_FLOAT;
    }

    return res;
}

SemanticArgs semantico_cria_args(SemanticExpr primeiro) {
    SemanticArgs args;
    args.count = 1;
    args.capacity = 4;
    args.types = malloc(args.capacity * sizeof(ExpType));
    args.lines = malloc(args.capacity * sizeof(int));
    args.func_names = malloc(args.capacity * sizeof(char *));

    if (args.types && args.lines && args.func_names) {
        args.types[0] = primeiro.type;
        args.lines[0] = primeiro.line;
        args.func_names[0] = primeiro.func_name ? strdup(primeiro.func_name) : NULL;
    }
    return args;
}

SemanticArgs semantico_adiciona_arg(SemanticArgs lista, SemanticExpr novo) {
    if (lista.count >= lista.capacity) {
        lista.capacity *= 2;
        lista.types = realloc(lista.types, lista.capacity * sizeof(ExpType));
        lista.lines = realloc(lista.lines, lista.capacity * sizeof(int));
        lista.func_names = realloc(lista.func_names, lista.capacity * sizeof(char *));
    }

    if (lista.types && lista.lines && lista.func_names) {
        lista.types[lista.count] = novo.type;
        lista.lines[lista.count] = novo.line;
        lista.func_names[lista.count] = novo.func_name ? strdup(novo.func_name) : NULL;
        lista.count++;
    }
    return lista;
}

void semantico_libera_args(SemanticArgs *args) {
    if (args == NULL) {
        return;
    }
    if (args->func_names) {
        for (size_t i = 0; i < args->count; i++) {
            free(args->func_names[i]);
        }
        free(args->func_names);
    }
    free(args->types);
    free(args->lines);
    args->types = NULL;
    args->lines = NULL;
    args->func_names = NULL;
    args->count = 0;
    args->capacity = 0;
}

void semantico_checa_expressao_comando(SemanticExpr exp, int linha) {
    if (exp.type == EXP_TYPE_STRING) {
        fprintf(stderr, "Erro semantico (linha %d): string fora do print.\n", linha);
        sem_errors++;
    } else if (exp.type == EXP_TYPE_CHAR) {
        fprintf(stderr, "Erro semantico (linha %d): char fora do print.\n", linha);
        sem_errors++;
    }
}

void semantico_checa_print(SemanticExpr exp, int linha) {
    if (exp.type == EXP_TYPE_VOID) {
        fprintf(stderr, "Erro semantico (linha %d): funcao void '%s' usada como valor.\n",
                linha, exp.func_name ? exp.func_name : "");
        sem_errors++;
    }
}

void semantico_checa_condicao(SemanticExpr exp, int linha) {
    if (exp.type == EXP_TYPE_VOID) {
        fprintf(stderr, "Erro semantico (linha %d): funcao void '%s' usada como valor.\n",
                linha, exp.func_name ? exp.func_name : "");
        sem_errors++;
    } else if (exp.type == EXP_TYPE_STRING) {
        fprintf(stderr, "Erro semantico (linha %d): string fora do print.\n", linha);
        sem_errors++;
    } else if (exp.type == EXP_TYPE_CHAR) {
        fprintf(stderr, "Erro semantico (linha %d): char fora do print.\n", linha);
        sem_errors++;
    }
}

void semantico_libera_expr(SemanticExpr *exp) {
    if (exp && exp->func_name) {
        free(exp->func_name);
        exp->func_name = NULL;
    }
}
