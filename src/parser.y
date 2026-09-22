/* FGA0003 - Compiladores 1 */
/* Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* Fase Sintática - Analisador sintático do Mini-C */

/* ATENÇÃO: este arquivo é apenas o esqueleto que liga o scanner ao parser.
 * A declaração dos tokens e do %union é definitiva; a gramática ao final é um
 * placeholder que aceita qualquer sequência de tokens e deve ser substituída
 * pelas regras reais. */

%{
#include <stdio.h>

int yylex(void);
void yyerror(const char *msg);

extern int yylineno;
%}

/* Valor semântico dos tokens (preenchido em yylval pelo scanner.l) */
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

/* ------------------------------------------------------------------------- */
/* PRECEDÊNCIA E ASSOCIATIVIDADE                                             */
/* ------------------------------------------------------------------------- */
/* Para resolver o conflito shift/reduce do "dangling else" */
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
    : declaracoes { printf("Analise sintatica concluida com sucesso.\n"); }
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
    | tipo IDENTIFIER ASSIGN expressao SEMI
    ;

tipo
    : INT
    | FLOAT
    | VOID
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
    : IDENTIFIER ASSIGN expressao
    | IDENTIFIER ADD_ASSIGN expressao
    | IDENTIFIER SUB_ASSIGN expressao
    | IDENTIFIER MUL_ASSIGN expressao
    | IDENTIFIER DIV_ASSIGN expressao
    | expressao OR expressao
    | expressao AND expressao
    | expressao EQ expressao
    | expressao NE expressao
    | expressao LT expressao
    | expressao GT expressao
    | expressao LE expressao
    | expressao GE expressao
    | expressao PLUS expressao
    | expressao MINUS expressao
    | expressao TIMES expressao
    | expressao DIVIDE expressao
    | expressao MOD expressao
    | NOT expressao
    | MINUS expressao %prec NOT  /* Negação unária */
    | INC IDENTIFIER
    | IDENTIFIER INC
    | DEC IDENTIFIER
    | IDENTIFIER DEC
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
