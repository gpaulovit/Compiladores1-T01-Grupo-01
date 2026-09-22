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

%%

programa
    : %empty
    | programa qualquer_token
    ;

qualquer_token
    : INT_LITERAL | FLOAT_LITERAL | IDENTIFIER | CHAR_LITERAL | STRING_LITERAL
    | INT | FLOAT | VOID
    | IF | ELSE | WHILE | FOR | DO | RETURN | PRINT
    | INC | DEC
    | ADD_ASSIGN | SUB_ASSIGN | MUL_ASSIGN | DIV_ASSIGN
    | EQ | NE | LE | GE | LT | GT
    | AND | OR | NOT
    | ASSIGN
    | PLUS | MINUS | TIMES | DIVIDE | MOD
    | LBRACE | RBRACE | LPAREN | RPAREN | SEMI | COMMA
    ;

%%

void yyerror(const char *msg) {
    fprintf(stderr, "Erro sintático (linha %d): %s\n", yylineno, msg);
}
