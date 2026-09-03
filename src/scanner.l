/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* Fase Léxica - Operadores e Delimitadores do Mini-C */

%{
#include <stdio.h>
%}

%option noyywrap

%%

    /* 1. OPERADORES DE INCREMENTO E DECREMENTO */
"++"        { printf("Token: INC (Lexema: %s)\n", yytext); }
"--"        { printf("Token: DEC (Lexema: %s)\n", yytext); }

    /* 2. OPERADORES DE ATRIBUIÇÃO COMPOSTA */
"+="        { printf("Token: ADD_ASSIGN (Lexema: %s)\n", yytext); }
"-="        { printf("Token: SUB_ASSIGN (Lexema: %s)\n", yytext); }
"*="        { printf("Token: MUL_ASSIGN (Lexema: %s)\n", yytext); }
"/="        { printf("Token: DIV_ASSIGN (Lexema: %s)\n", yytext); }

    /* 3. OPERADORES RELACIONAIS (COMPARAÇÃO) */
"=="        { printf("Token: EQ (Lexema: %s)\n", yytext); }
"!="        { printf("Token: NE (Lexema: %s)\n", yytext); }
"<="        { printf("Token: LE (Lexema: %s)\n", yytext); }
">="        { printf("Token: GE (Lexema: %s)\n", yytext); }
"<"         { printf("Token: LT (Lexema: %s)\n", yytext); }
">"         { printf("Token: GT (Lexema: %s)\n", yytext); }

    /* 4. OPERADORES LÓGICOS */
"&&"        { printf("Token: AND (Lexema: %s)\n", yytext); }
"||"        { printf("Token: OR (Lexema: %s)\n", yytext); }
"!"         { printf("Token: NOT (Lexema: %s)\n", yytext); }

    /* 5. OPERADOR DE ATRIBUIÇÃO SIMPLES */
"="         { printf("Token: ASSIGN (Lexema: %s)\n", yytext); }

    /* 6. OPERADORES ARITMÉTICOS */
"+"         { printf("Token: PLUS (Lexema: %s)\n", yytext); }
"-"         { printf("Token: MINUS (Lexema: %s)\n", yytext); }
"*"         { printf("Token: TIMES (Lexema: %s)\n", yytext); }
"/"         { printf("Token: DIVIDE (Lexema: %s)\n", yytext); }

    /* 7. DELIMITADORES E PONTUAÇÃO */
"{"         { printf("Token: LBRACE (Lexema: %s)\n", yytext); }
"}"         { printf("Token: RBRACE (Lexema: %s)\n", yytext); }
"("         { printf("Token: LPAREN (Lexema: %s)\n", yytext); }
")"         { printf("Token: RPAREN (Lexema: %s)\n", yytext); }
";"         { printf("Token: SEMI (Lexema: %s)\n", yytext); }
","         { printf("Token: COMMA (Lexema: %s)\n", yytext); }

    /* Ignora espaços em branco, tabulações e quebras de linha */
[ \t\n\r]+  { /* ignorar */ }

    /* Regra de erro léxico para caracteres inválidos */
.           { printf("ERRO LÉXICO: Caractere desconhecido '%s'\n", yytext); }

%%

int main(void) {
    yylex();
    return 0;
}
