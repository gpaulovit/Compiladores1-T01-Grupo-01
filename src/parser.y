/* Fase Sintática - Analisador sintático do Mini-C */

/* Gramática livre de contexto do Mini-C, conforme o escopo definido em
 * docs/projeto.md: declarações e tipos, expressões, estruturas de controle,
 * funções e o comando print. */

%{
#include <stdio.h>
#include <string.h>

int yylex(void);
void yyerror(const char *msg);

extern int yylineno;

/* Erros semânticos encontrados; lido por src/main.c para definir a saída. */
int sem_errors = 0;

/* Contadores das expressões reconhecidas, usados no resumo final. */
static int n_aritmeticas = 0, n_relacionais = 0, n_logicas = 0;

/* O escopo define void apenas como tipo de retorno de função .    */
static void checa_tipo_var(const char *tipo, const char *nome) {
    if (strcmp(tipo, "void") == 0) {
        fprintf(stderr, "Erro semantico (linha %d): variavel '%s' nao pode ter tipo void.\n",
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

/* Valor semântico dos tokens */
%union {
    int    ival;   /* INT_LITERAL */
    double fval;   /* FLOAT_LITERAL */
    char  *sval;   /* IDENTIFIER, CHAR_LITERAL, STRING_LITERAL (strdup do lexema) */
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
%type <sval> tipo


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
    : declaracoes
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
        { checa_tipo_var($1, $2); printf("=> Declaracao: %s %s\n", $1, $2); }
    | tipo IDENTIFIER ASSIGN expressao SEMI
        { checa_tipo_var($1, $2); printf("=> Declaracao com inicializacao: %s %s\n", $1, $2); }
    ;

tipo
    : INT    { $$ = "int"; }
    | FLOAT  { $$ = "float"; }
    | VOID   { $$ = "void"; }
    ;

/* --- REGRAS DE FUNÇÕES (Main, recursão, etc) --- */
fun_declaracao
    : tipo IDENTIFIER LPAREN parametros_opt RPAREN bloco
        { printf("=> Definicao de funcao: %s\n", $2); }
    ;

parametros_opt
    : %empty
    | parametros
    ;

parametros
    : parametro
    | parametros COMMA parametro
    ;

parametro
    : tipo IDENTIFIER
        { checa_tipo_var($1, $2); printf("=> Parametro: %s %s\n", $1, $2); }
    ;

bloco
    : LBRACE declaracoes_locais comandos RBRACE
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
    : IDENTIFIER ASSIGN expressao      { printf("=> Atribuicao: %s\n", $1); }
    | IDENTIFIER ADD_ASSIGN expressao  { printf("=> Atribuicao composta (+=): %s\n", $1); }
    | IDENTIFIER SUB_ASSIGN expressao  { printf("=> Atribuicao composta (-=): %s\n", $1); }
    | IDENTIFIER MUL_ASSIGN expressao  { printf("=> Atribuicao composta (*=): %s\n", $1); }
    | IDENTIFIER DIV_ASSIGN expressao  { printf("=> Atribuicao composta (/=): %s\n", $1); }
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
    | INC IDENTIFIER                   { printf("=> Incremento (pre): %s\n", $2); }
    | IDENTIFIER INC                   { printf("=> Incremento (pos): %s\n", $1); }
    | DEC IDENTIFIER                   { printf("=> Decremento (pre): %s\n", $2); }
    | IDENTIFIER DEC                   { printf("=> Decremento (pos): %s\n", $1); }
    | LPAREN expressao RPAREN
    | IDENTIFIER LPAREN argumentos_opt RPAREN  { printf("=> Chamada de funcao: %s\n", $1); }
    | IDENTIFIER
    | INT_LITERAL
    | FLOAT_LITERAL
    | CHAR_LITERAL
    | STRING_LITERAL
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
