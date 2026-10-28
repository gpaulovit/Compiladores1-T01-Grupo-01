/* FGA0003 - Compiladores 1 */
/* Engenharia de Software - Universidade de Brasília (UnB) */
/* Fase Sintática e Semântica - Analisador do Mini-C */

%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symtab.h"
#include "semantico.h"
#include "ast.h"

int yylex(void);
void yyerror(const char *msg);

extern int yylineno;

/* Erros semânticos encontrados; lido por src/main.c para definir a saída. */
int sem_errors = 0;

/* Raiz da AST, montada na redução de programa; lida por src/main.c. */
No *ast_raiz = NULL;

/* Tabela de símbolos utilizada durante a análise semântica. */
static SymbolTable *symbol_table = NULL;

void liberar_tabela_simbolos(void);

/* Contadores das expressões reconhecidas, usados no resumo final. */
static int n_aritmeticas = 0, n_relacionais = 0, n_logicas = 0;

static DataType converte_tipo(const char *tipo) {
    if (strcmp(tipo, "int") == 0) {
        return TYPE_INT;
    }

    if (strcmp(tipo, "float") == 0) {
        return TYPE_FLOAT;
    }

    return TYPE_VOID;
}

/* Relatam um operador reconhecido. A ação dispara na REDUÇÃO, então a ordem das
 * linhas reflete a precedência: em "1 + 2 * 3" o '*' é reduzido antes do '+'. */
static void op_aritmetica(const char *op) { n_aritmeticas++; printf("   . aritmetica: %s\n", op); }
static void op_relacional(const char *op) { n_relacionais++; printf("   . relacional: %s\n", op); }
static void op_logica(const char *op)     { n_logicas++;     printf("   . logica: %s\n", op); }

/* Montagem dos nós de expressão, anotados com o tipo da análise semântica.
 * Os nomes vêm do strdup do scanner e passam a pertencer ao nó. */
static No *no_tipado(No *n, SemanticExpr sem) {
    n->tipo = sem.type;
    return n;
}

static No *no_binario(const char *op, No *esq, No *dir, SemanticExpr sem, int linha) {
    return no_tipado(ast_op(NO_BINARIO, op, linha, esq, dir), sem);
}

static No *no_atribuicao(const char *op, char *nome, No *valor, SemanticExpr sem, int linha) {
    No *alvo = no_tipado(ast_texto(NO_ID, nome, linha), sem);
    return no_tipado(ast_op(NO_ATRIBUICAO, op, linha, alvo, valor), sem);
}

static No *no_incdec(Categoria cat, const char *op, char *nome, SemanticExpr sem, int linha) {
    No *alvo = no_tipado(ast_texto(NO_ID, nome, linha), sem);
    return no_tipado(ast_op(cat, op, linha, alvo, NULL), sem);
}
%}

/* Habilita @n nas ações: a linha de cada símbolo, preenchida pelo scanner. */
%locations

%code requires {
    #include "symtab.h"
    #include "semantico.h"
    #include "ast.h"

    struct ParametroParser {
        char *name;
        DataType type;
        int line;
    };
}

/* Valor semântico dos tokens e das listas auxiliares. */
%union {
    int    ival;   /* INT_LITERAL */
    double fval;   /* FLOAT_LITERAL */
    char *sval;    /* IDENTIFIER, CHAR_LITERAL, STRING_LITERAL */
    const char *tipo;    /* Tipos fixos: int, float, void */

    struct {
        struct ParametroParser *items;
        size_t count;
    } parametros;
    struct ParametroParser parametro;

    No *no;

    /* Expressões e argumentos: resultado semântico e nó da AST. */
    struct { SemanticExpr sem; No *no; } expr;
    struct { SemanticArgs sem; No *no; } args;
}

/* Tokens com valor */
%token <ival> INT_LITERAL
%token <fval> FLOAT_LITERAL
%token <sval> IDENTIFIER CHAR_LITERAL STRING_LITERAL

/* Palavras reservadas (tipos, controle e E/S) */
%token INT FLOAT VOID
%token IF ELSE WHILE FOR DO RETURN PRINT

/* Operadores */
%token INC DEC
%token ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN
%token EQ NE LE GE LT GT
%token AND OR NOT
%token ASSIGN
%token PLUS MINUS TIMES DIVIDE MOD

/* Delimitadores */
%token LBRACE RBRACE LPAREN RPAREN SEMI COMMA

/* Não-terminais com valor semântico */
%type <tipo> tipo lista_variaveis
%type <parametros> parametros parametros_opt
%type <parametro> parametro
%type <expr> expressao expressao_opt
%type <args> argumentos argumentos_opt
%type <no> bloco itens_bloco item_bloco comando expressao_comando
%type <no> comando_if comando_while comando_do_while comando_for comando_return comando_print

%destructor { free($$); } <sval>

%destructor {
    for (size_t i = 0; i < $$.count; i++) {
        free($$.items[i].name);
    }
    free($$.items);
} <parametros>

%destructor { free($$.name); } <parametro>

%destructor { semantico_libera_args(&$$.sem); ast_libera($$.no); } <args>
%destructor { semantico_libera_expr(&$$.sem); ast_libera($$.no); } <expr>
%destructor { ast_libera($$); } <no>

/* PRECEDÊNCIA E ASSOCIATIVIDADE */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

/* Precedência de operadores (da menor para a maior) */
%right ASSIGN ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN
%left OR
%left AND
%left EQ NE
%left LT GT LE GE
%left PLUS MINUS
%left TIMES DIVIDE MOD
%right NOT
%left INC DEC

%%

programa
    : {
          symbol_table = symtab_create();

          if (symbol_table == NULL) {
              fprintf(stderr, "Erro: nao foi possivel criar a tabela de simbolos.\n");
              sem_errors++;
              YYABORT;
          }
          semantico_init(symbol_table);
      }
      declaracoes
        {
            printf("\nAnalise sintatica concluida%s.\n",
                   sem_errors > 0 ? ", com erros semanticos" : " com sucesso");
            printf("Expressoes reconhecidas: %d aritmeticas, %d relacionais, %d logicas.\n",
                   n_aritmeticas, n_relacionais, n_logicas);
        }
    ;

declaracoes
    : declaracao
    | declaracoes declaracao
    ;

declaracao
    : var_declaracao
    | fun_declaracao
    ;

var_declaracao
    : lista_variaveis SEMI
    ;

lista_variaveis
    : tipo IDENTIFIER
        {
            semantico_checa_declaracao_var($1, $2, yylineno);
            printf("=> Declaracao: %s %s\n", $1, $2);
            $$ = $1;
            free($2);
        }
    | tipo IDENTIFIER ASSIGN expressao
        {
            semantico_checa_declaracao_var_init($1, $2, $4.sem, yylineno);
            printf("=> Declaracao com inicializacao: %s %s\n", $1, $2);
            $$ = $1;
            free($2);
        }
    | lista_variaveis COMMA IDENTIFIER
        {
            semantico_checa_declaracao_var($1, $3, yylineno);
            printf("=> Declaracao: %s %s\n", $1, $3);
            $$ = $1;
            free($3);
        }
    | lista_variaveis COMMA IDENTIFIER ASSIGN expressao
        {
            semantico_checa_declaracao_var_init($1, $3, $5.sem, yylineno);
            printf("=> Declaracao com inicializacao: %s %s\n", $1, $3);
            $$ = $1;
            free($3);
        }
    ;

tipo
    : INT    { $$ = "int"; }
    | FLOAT  { $$ = "float"; }
    | VOID   { $$ = "void"; }
    ;

/* --- REGRAS DE FUNÇÕES (Main, recursão, etc) --- */
fun_declaracao
    : tipo IDENTIFIER LPAREN parametros_opt RPAREN
        {
            semantico_define_funcao_inicio($1, $2, yylineno);
            ParameterType *tipos_parametros = NULL;
            size_t quantidade = $4.count;

            if (quantidade > 0) {
                tipos_parametros = malloc(
                    quantidade * sizeof(ParameterType)
                );

                if (tipos_parametros == NULL) {
                    fprintf(stderr,
                            "Erro: memoria insuficiente para assinatura da funcao.\n");
                    sem_errors++;
                } else {
                    for (size_t i = 0; i < quantidade; i++) {
                        tipos_parametros[i].type = $4.items[i].type;
                    }
                }
            }

            if (quantidade == 0 || tipos_parametros != NULL) {
                if (!symtab_insert_function(
                        symbol_table,
                        $2,
                        converte_tipo($1),
                        tipos_parametros,
                        quantidade,
                        yylineno)) {
                    fprintf(stderr,
                            "Erro semantico (linha %d): funcao '%s' ja declarada neste escopo.\n",
                            yylineno, $2);
                    sem_errors++;
                }
            }

            free(tipos_parametros);

            $<ival>$ = symtab_open_scope(symbol_table);

            if (!$<ival>$) {
                fprintf(stderr,
                        "Erro: nao foi possivel abrir escopo para funcao '%s'.\n",
                        $2);
                sem_errors++;
            } else {
                for (size_t i = 0; i < quantidade; i++) {
                    if (!symtab_insert_parameter(
                            symbol_table,
                            $4.items[i].name,
                            $4.items[i].type,
                            $4.items[i].line)) {
                        fprintf(stderr,
                                "Erro semantico (linha %d): variavel '%s' ja declarada neste escopo.\n",
                                $4.items[i].line,
                                $4.items[i].name);
                        sem_errors++;
                    }
                }
            }
        }
      LBRACE itens_bloco RBRACE
        {
            if ($<ival>6) {
                symtab_close_scope(symbol_table);
            }

            semantico_define_funcao_fim();

            for (size_t i = 0; i < $4.count; i++) {
                free($4.items[i].name);
            }

            free($4.items);
            printf("=> Definicao de funcao: %s\n", $2);
            free($2);
        }
    ;

parametros_opt
    : %empty
        {
            $$.items = NULL;
            $$.count = 0;
        }
    | parametros
        {
            $$ = $1;
        }
    ;

parametros
    : parametro
        {
            $$.items = malloc(sizeof(struct ParametroParser));

            if ($$.items == NULL) {
                free($1.name);
                fprintf(stderr, "Erro: memoria insuficiente para parametros.\n");
                YYABORT;
            }

            $$.items[0] = $1;
            $$.count = 1;
        }
    | parametros COMMA parametro
        {
            struct ParametroParser *novos =
                realloc($1.items,
                        ($1.count + 1) * sizeof(struct ParametroParser));

            if (novos == NULL) {
                for (size_t i = 0; i < $1.count; i++) {
                    free($1.items[i].name);
                }

                free($1.items);
                free($3.name);

                fprintf(stderr, "Erro: memoria insuficiente para parametros.\n");
                YYABORT;
            }

            novos[$1.count] = $3;
            $$.items = novos;
            $$.count = $1.count + 1;
        }
    ;

parametro
    : tipo IDENTIFIER
        {
            $$.name = $2;
            $$.type = converte_tipo($1);
            $$.line = yylineno;
            semantico_checa_declaracao_param($1, $2, yylineno);
            printf("=> Parametro: %s %s\n", $1, $2);
        }
    ;

bloco
    : LBRACE
        {
            $<ival>$ = symtab_open_scope(symbol_table);

            if (!$<ival>$) {
                fprintf(stderr,
                        "Erro: nao foi possivel abrir escopo para bloco.\n");
                sem_errors++;
            }
        }
      itens_bloco RBRACE
        {
            if ($<ival>2) {
                symtab_close_scope(symbol_table);
            }
            $$ = ast_cria(NO_BLOCO, @1.first_line, $3, NULL, NULL, NULL);
        }
    ;

itens_bloco
    : %empty
        {
            $$ = NULL;
        }
    | itens_bloco item_bloco
        {
            $$ = ast_lista_adiciona($1, $2);
        }
    ;

item_bloco
    : var_declaracao
        {
            $$ = NULL;
        }
    | comando
    ;

comando
    : expressao_comando
    | bloco
    | comando_if
    | comando_while
    | comando_for
    | comando_do_while
    | comando_return
    | comando_print
    ;

/* A expressão usada como comando entra na lista sem nó próprio. */
expressao_comando
    : expressao SEMI
        {
            semantico_checa_expressao_comando($1.sem, yylineno);
            $$ = $1.no;
        }
    | SEMI
        {
            $$ = ast_cria(NO_VAZIO, @1.first_line, NULL, NULL, NULL, NULL);
        }
    ;

/* --- ESTRUTURAS DE CONTROLE (if/else) --- */
comando_if
    : IF LPAREN expressao RPAREN comando %prec LOWER_THAN_ELSE
        {
            semantico_checa_condicao($3.sem, yylineno);
            printf("=> Reconheceu IF\n");
            $$ = ast_cria(NO_IF, @1.first_line, $3.no, $5, NULL, NULL);
        }
    | IF LPAREN expressao RPAREN comando ELSE comando
        {
            semantico_checa_condicao($3.sem, yylineno);
            printf("=> Reconheceu IF-ELSE\n");
            $$ = ast_cria(NO_IF, @1.first_line, $3.no, $5, $7, NULL);
        }
    ;

/* --- ESTRUTURAS DE LAÇO --- */
comando_while
    : WHILE LPAREN expressao RPAREN comando
        {
            semantico_checa_condicao($3.sem, yylineno);
            printf("=> Reconheceu WHILE\n");
            $$ = ast_cria(NO_WHILE, @1.first_line, $3.no, $5, NULL, NULL);
        }
    ;

comando_do_while
    : DO comando WHILE LPAREN expressao RPAREN SEMI
        {
            semantico_checa_condicao($5.sem, yylineno);
            printf("=> Reconheceu DO-WHILE\n");
            $$ = ast_cria(NO_DO_WHILE, @1.first_line, $2, $5.no, NULL, NULL);
        }
    ;

comando_for
    : FOR LPAREN expressao_opt SEMI expressao_opt SEMI expressao_opt RPAREN comando
        {
            semantico_checa_expressao_comando($3.sem, yylineno);
            semantico_checa_condicao($5.sem, yylineno);
            semantico_checa_expressao_comando($7.sem, yylineno);
            printf("=> Reconheceu FOR\n");
            $$ = ast_cria(NO_FOR, @1.first_line, $3.no, $5.no, $7.no, $9);
        }
    ;

comando_return
    : RETURN SEMI
        {
            semantico_checa_retorno_vazio(yylineno);
            $$ = ast_cria(NO_RETURN, @1.first_line, NULL, NULL, NULL, NULL);
        }
    | RETURN expressao SEMI
        {
            semantico_checa_retorno($2.sem, yylineno);
            $$ = ast_cria(NO_RETURN, @1.first_line, $2.no, NULL, NULL, NULL);
        }
    ;

/* --- REGRA DE SAÍDA (print) --- */
comando_print
    : PRINT LPAREN expressao RPAREN SEMI
        {
            semantico_checa_print($3.sem, yylineno);
            printf("=> Reconheceu comando PRINT\n");
            $$ = ast_cria(NO_PRINT, @1.first_line, $3.no, NULL, NULL, NULL);
        }
    ;

expressao_opt
    : %empty
        {
            $$.sem.type = EXP_TYPE_INT;
            $$.sem.line = yylineno;
            $$.sem.func_name = NULL;
            $$.no = NULL;
        }
    | expressao
        {
            $$ = $1;
        }
    ;

/* --- EXPRESSÕES --- */
/* Cada expressão carrega o resultado semântico (sem) e o nó da AST (no),
 * anotado com o tipo calculado pela análise semântica. */
expressao
    : IDENTIFIER ASSIGN expressao
        {
            $$.sem = semantico_checa_atribuicao($1, $3.sem, yylineno);
            printf("=> Atribuicao: %s\n", $1);
            $$.no = no_atribuicao("=", $1, $3.no, $$.sem, @$.first_line);
        }
    | IDENTIFIER ADD_ASSIGN expressao
        {
            $$.sem = semantico_checa_atribuicao_composta($1, "+=", $3.sem, yylineno);
            printf("=> Atribuicao composta (+=): %s\n", $1);
            $$.no = no_atribuicao("+=", $1, $3.no, $$.sem, @$.first_line);
        }
    | IDENTIFIER SUB_ASSIGN expressao
        {
            $$.sem = semantico_checa_atribuicao_composta($1, "-=", $3.sem, yylineno);
            printf("=> Atribuicao composta (-=): %s\n", $1);
            $$.no = no_atribuicao("-=", $1, $3.no, $$.sem, @$.first_line);
        }
    | IDENTIFIER MUL_ASSIGN expressao
        {
            $$.sem = semantico_checa_atribuicao_composta($1, "*=", $3.sem, yylineno);
            printf("=> Atribuicao composta (*=): %s\n", $1);
            $$.no = no_atribuicao("*=", $1, $3.no, $$.sem, @$.first_line);
        }
    | IDENTIFIER DIV_ASSIGN expressao
        {
            $$.sem = semantico_checa_atribuicao_composta($1, "/=", $3.sem, yylineno);
            printf("=> Atribuicao composta (/=): %s\n", $1);
            $$.no = no_atribuicao("/=", $1, $3.no, $$.sem, @$.first_line);
        }
    | expressao OR expressao
        {
            op_logica("||");
            $$.sem = semantico_checa_op_logico("||", $1.sem, $3.sem, yylineno);
            $$.no = no_binario("||", $1.no, $3.no, $$.sem, @$.first_line);
        }
    | expressao AND expressao
        {
            op_logica("&&");
            $$.sem = semantico_checa_op_logico("&&", $1.sem, $3.sem, yylineno);
            $$.no = no_binario("&&", $1.no, $3.no, $$.sem, @$.first_line);
        }
    | expressao EQ expressao
        {
            op_relacional("==");
            $$.sem = semantico_checa_op_relacional("==", $1.sem, $3.sem, yylineno);
            $$.no = no_binario("==", $1.no, $3.no, $$.sem, @$.first_line);
        }
    | expressao NE expressao
        {
            op_relacional("!=");
            $$.sem = semantico_checa_op_relacional("!=", $1.sem, $3.sem, yylineno);
            $$.no = no_binario("!=", $1.no, $3.no, $$.sem, @$.first_line);
        }
    | expressao LT expressao
        {
            op_relacional("<");
            $$.sem = semantico_checa_op_relacional("<", $1.sem, $3.sem, yylineno);
            $$.no = no_binario("<", $1.no, $3.no, $$.sem, @$.first_line);
        }
    | expressao GT expressao
        {
            op_relacional(">");
            $$.sem = semantico_checa_op_relacional(">", $1.sem, $3.sem, yylineno);
            $$.no = no_binario(">", $1.no, $3.no, $$.sem, @$.first_line);
        }
    | expressao LE expressao
        {
            op_relacional("<=");
            $$.sem = semantico_checa_op_relacional("<=", $1.sem, $3.sem, yylineno);
            $$.no = no_binario("<=", $1.no, $3.no, $$.sem, @$.first_line);
        }
    | expressao GE expressao
        {
            op_relacional(">=");
            $$.sem = semantico_checa_op_relacional(">=", $1.sem, $3.sem, yylineno);
            $$.no = no_binario(">=", $1.no, $3.no, $$.sem, @$.first_line);
        }
    | expressao PLUS expressao
        {
            op_aritmetica("+");
            $$.sem = semantico_checa_op_aritmetico("+", $1.sem, $3.sem, yylineno);
            $$.no = no_binario("+", $1.no, $3.no, $$.sem, @$.first_line);
        }
    | expressao MINUS expressao
        {
            op_aritmetica("-");
            $$.sem = semantico_checa_op_aritmetico("-", $1.sem, $3.sem, yylineno);
            $$.no = no_binario("-", $1.no, $3.no, $$.sem, @$.first_line);
        }
    | expressao TIMES expressao
        {
            op_aritmetica("*");
            $$.sem = semantico_checa_op_aritmetico("*", $1.sem, $3.sem, yylineno);
            $$.no = no_binario("*", $1.no, $3.no, $$.sem, @$.first_line);
        }
    | expressao DIVIDE expressao
        {
            op_aritmetica("/");
            $$.sem = semantico_checa_op_aritmetico("/", $1.sem, $3.sem, yylineno);
            $$.no = no_binario("/", $1.no, $3.no, $$.sem, @$.first_line);
        }
    | expressao MOD expressao
        {
            op_aritmetica("%");
            $$.sem = semantico_checa_op_aritmetico("%", $1.sem, $3.sem, yylineno);
            $$.no = no_binario("%", $1.no, $3.no, $$.sem, @$.first_line);
        }
    | NOT expressao
        {
            op_logica("!");
            $$.sem = semantico_checa_op_unario("!", $2.sem, yylineno);
            $$.no = no_tipado(ast_op(NO_UNARIO, "!", @$.first_line, $2.no, NULL), $$.sem);
        }
    | MINUS expressao %prec NOT
        {
            op_aritmetica("- (unario)");
            $$.sem = semantico_checa_op_unario("-", $2.sem, yylineno);
            $$.no = no_tipado(ast_op(NO_UNARIO, "-", @$.first_line, $2.no, NULL), $$.sem);
        }
    | INC IDENTIFIER
        {
            $$.sem = semantico_checa_inc_dec($2, "++", yylineno);
            printf("=> Incremento (pre): %s\n", $2);
            $$.no = no_incdec(NO_PRE_INCDEC, "++", $2, $$.sem, @$.first_line);
        }
    | IDENTIFIER INC
        {
            $$.sem = semantico_checa_inc_dec($1, "++", yylineno);
            printf("=> Incremento (pos): %s\n", $1);
            $$.no = no_incdec(NO_POS_INCDEC, "++", $1, $$.sem, @$.first_line);
        }
    | DEC IDENTIFIER
        {
            $$.sem = semantico_checa_inc_dec($2, "--", yylineno);
            printf("=> Decremento (pre): %s\n", $2);
            $$.no = no_incdec(NO_PRE_INCDEC, "--", $2, $$.sem, @$.first_line);
        }
    | IDENTIFIER DEC
        {
            $$.sem = semantico_checa_inc_dec($1, "--", yylineno);
            printf("=> Decremento (pos): %s\n", $1);
            $$.no = no_incdec(NO_POS_INCDEC, "--", $1, $$.sem, @$.first_line);
        }
    | LPAREN expressao RPAREN
        {
            $$ = $2;
        }
    | IDENTIFIER LPAREN argumentos_opt RPAREN
        {
            $$.sem = semantico_checa_chamada($1, &$3.sem, yylineno);
            printf("=> Chamada de funcao: %s\n", $1);
            semantico_libera_args(&$3.sem);
            $$.no = ast_texto(NO_CHAMADA, $1, @$.first_line);
            $$.no->filhos[0] = $3.no;
            no_tipado($$.no, $$.sem);
        }
    | IDENTIFIER
        {
            $$.sem = semantico_checa_identificador($1, yylineno);
            $$.no = no_tipado(ast_texto(NO_ID, $1, @$.first_line), $$.sem);
        }
    | INT_LITERAL
        {
            $$.sem = semantico_literal_int(yylineno);
            $$.no = ast_int($1, @$.first_line);
        }
    | FLOAT_LITERAL
        {
            $$.sem = semantico_literal_float(yylineno);
            $$.no = ast_float($1, @$.first_line);
        }
    | CHAR_LITERAL
        {
            $$.sem = semantico_literal_char(yylineno);
            $$.no = no_tipado(ast_texto(NO_LIT_CHAR, $1, @$.first_line), $$.sem);
        }
    | STRING_LITERAL
        {
            $$.sem = semantico_literal_string(yylineno);
            $$.no = no_tipado(ast_texto(NO_LIT_STRING, $1, @$.first_line), $$.sem);
        }
    ;

argumentos_opt
    : %empty
        {
            $$.sem.types = NULL;
            $$.sem.lines = NULL;
            $$.sem.func_names = NULL;
            $$.sem.count = 0;
            $$.sem.capacity = 0;
            $$.no = NULL;
        }
    | argumentos
        {
            $$ = $1;
        }
    ;

argumentos
    : expressao
        {
            $$.sem = semantico_cria_args($1.sem);
            $$.no = $1.no;
        }
    | argumentos COMMA expressao
        {
            $$.sem = semantico_adiciona_arg($1.sem, $3.sem);
            $$.no = ast_lista_adiciona($1.no, $3.no);
        }
    ;

%%

void yyerror(const char *msg) {
    fprintf(stderr, "Erro sintático (linha %d): %s\n", yylineno, msg);
}

void liberar_tabela_simbolos(void) {
    symtab_destroy(symbol_table);
    symbol_table = NULL;
}
