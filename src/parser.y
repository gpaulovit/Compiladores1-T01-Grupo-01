/* Fase Sintática - Analisador sintático do Mini-C */

/* Gramática livre de contexto do Mini-C, conforme o escopo definido em
 * docs/projeto.md: declarações e tipos, expressões, estruturas de controle,
 * funções e o comando print. */

%{
#include <stdio.h>
#include <string.h>
#include "symtab.h"
#include <stdlib.h>

int yylex(void);
void yyerror(const char *msg);

extern int yylineno;

/* Erros semânticos encontrados; lido por src/main.c para definir a saída. */
int sem_errors = 0;

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

/* O escopo define void apenas como tipo de retorno de função .    */
static void checa_tipo_var(const char *tipo, const char *nome) {
    if (strcmp(tipo, "void") == 0) {
        fprintf(stderr, "Erro semantico (linha %d): variavel '%s' nao pode ter tipo void.\n",
                yylineno, nome);
        sem_errors++;
    }
}

static void registra_variavel(const char *tipo, const char *nome) {
    checa_tipo_var(tipo, nome);

    if (strcmp(tipo, "void") == 0) {
        return;
    }

    if (!symtab_insert_variable(symbol_table, nome, converte_tipo(tipo), yylineno)) {
        fprintf(stderr,
                "Erro semantico (linha %d): variavel '%s' ja declarada neste escopo.\n",
                yylineno, nome);
        sem_errors++;
    }
}

/* Relatam um operador reconhecido. A ação dispara na REDUÇÃO, então a ordem das
 * linhas reflete a precedência: em "1 + 2 * 3" o '*' é reduzido antes do '+'. */
static void op_aritmetica(const char *op) { n_aritmeticas++; printf("   . aritmetica: %s\n", op); }
static void op_relacional(const char *op) { n_relacionais++; printf("   . relacional: %s\n", op); }
static void op_logica(const char *op)     { n_logicas++;     printf("   . logica: %s\n", op); }
%}

%code requires {
    #include "symtab.h"

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
%type <tipo> tipo
%type <parametros> parametros parametros_opt
%type <parametro> parametro

%destructor { free($$); } <sval>

%destructor {
    for (size_t i = 0; i < $$.count; i++) {
        free($$.items[i].name);
    }
    free($$.items);
} <parametros>

%destructor { free($$.name); } <parametro>


/* PRECEDÊNCIA E ASSOCIATIVIDADE                                             */
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
    : tipo IDENTIFIER SEMI
        {
            registra_variavel($1, $2);
            printf("=> Declaracao: %s %s\n", $1, $2);
            free($2);
        }
    | tipo IDENTIFIER ASSIGN expressao SEMI
        {
            registra_variavel($1, $2);
            printf("=> Declaracao com inicializacao: %s %s\n", $1, $2);
            free($2);
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
                                "Erro semantico (linha %d): parametro '%s' invalido ou redeclarado.\n",
                                $4.items[i].line,
                                $4.items[i].name);
                        sem_errors++;
                    }
                }
            }
        }
      LBRACE declaracoes_locais comandos RBRACE
        {
            if ($<ival>6) {
                symtab_close_scope(symbol_table);
            }

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
            checa_tipo_var($1, $2);
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
      declaracoes_locais comandos RBRACE
        {
            if ($<ival>2) {
                symtab_close_scope(symbol_table);
            }
        }
    ;

declaracoes_locais
    : %empty
    | declaracoes_locais var_declaracao
    ;

comandos
    : %empty
    | comandos comando
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

expressao_comando
    : expressao SEMI
    | SEMI
    ;

/* --- ESTRUTURAS DE CONTROLE (if/else) --- */
comando_if
    : IF LPAREN expressao RPAREN comando %prec LOWER_THAN_ELSE
        { printf("=> Reconheceu IF\n"); }
    | IF LPAREN expressao RPAREN comando ELSE comando
        { printf("=> Reconheceu IF-ELSE\n"); }
    ;

/* --- ESTRUTURAS DE LAÇO --- */
comando_while
    : WHILE LPAREN expressao RPAREN comando
        { printf("=> Reconheceu WHILE\n"); }
    ;

comando_do_while
    : DO comando WHILE LPAREN expressao RPAREN SEMI
        { printf("=> Reconheceu DO-WHILE\n"); }
    ;

comando_for
    : FOR LPAREN expressao_opt SEMI expressao_opt SEMI expressao_opt RPAREN comando
        { printf("=> Reconheceu FOR\n"); }
    ;

comando_return
    : RETURN expressao_opt SEMI
    ;

/* --- REGRA DE SAÍDA (print) --- */
comando_print
    : PRINT LPAREN expressao RPAREN SEMI
        { printf("=> Reconheceu comando PRINT\n"); }
    ;

expressao_opt
    : %empty
    | expressao
    ;

/* --- EXPRESSÕES --- */
expressao
    : IDENTIFIER ASSIGN expressao      { printf("=> Atribuicao: %s\n", $1); free($1); }
    | IDENTIFIER ADD_ASSIGN expressao  { printf("=> Atribuicao composta (+=): %s\n", $1); free($1); }
    | IDENTIFIER SUB_ASSIGN expressao  { printf("=> Atribuicao composta (-=): %s\n", $1); free($1); }
    | IDENTIFIER MUL_ASSIGN expressao  { printf("=> Atribuicao composta (*=): %s\n", $1); free($1); }
    | IDENTIFIER DIV_ASSIGN expressao  { printf("=> Atribuicao composta (/=): %s\n", $1); free($1); }
    | expressao OR expressao           { op_logica("||"); }
    | expressao AND expressao          { op_logica("&&"); }
    | expressao EQ expressao           { op_relacional("=="); }
    | expressao NE expressao           { op_relacional("!="); }
    | expressao LT expressao           { op_relacional("<"); }
    | expressao GT expressao           { op_relacional(">"); }
    | expressao LE expressao           { op_relacional("<="); }
    | expressao GE expressao           { op_relacional(">="); }
    | expressao PLUS expressao         { op_aritmetica("+"); }
    | expressao MINUS expressao        { op_aritmetica("-"); }
    | expressao TIMES expressao        { op_aritmetica("*"); }
    | expressao DIVIDE expressao       { op_aritmetica("/"); }
    | expressao MOD expressao          { op_aritmetica("%"); }
    | NOT expressao                    { op_logica("!"); }
    | MINUS expressao %prec NOT        { op_aritmetica("- (unario)"); }
    | INC IDENTIFIER                   { printf("=> Incremento (pre): %s\n", $2); free($2); }
    | IDENTIFIER INC                   { printf("=> Incremento (pos): %s\n", $1); free($1); }
    | DEC IDENTIFIER                   { printf("=> Decremento (pre): %s\n", $2); free($2); }
    | IDENTIFIER DEC                   { printf("=> Decremento (pos): %s\n", $1); free($1); }
    | LPAREN expressao RPAREN
    | IDENTIFIER LPAREN argumentos_opt RPAREN { printf("=> Chamada de funcao: %s\n", $1); free($1); }
    | IDENTIFIER { free($1); }
    | INT_LITERAL
    | FLOAT_LITERAL
    | CHAR_LITERAL   { free($1); }
    | STRING_LITERAL { free($1); }
    ;

argumentos_opt
    : %empty
    | argumentos
    ;

argumentos
    : expressao
    | argumentos COMMA expressao
    ;

%%

void yyerror(const char *msg) {
    fprintf(stderr, "Erro sintático (linha %d): %s\n", yylineno, msg);
}

void liberar_tabela_simbolos(void) {
    symtab_destroy(symbol_table);
    symbol_table = NULL;
}
