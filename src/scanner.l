/* FGA0003 - Compiladores 1 */
/* Curso de Engenharia de Software */
/* Universidade de Brasília (UnB) */
/* Fase Léxica - Analisador léxico do Mini-C, integrado ao Bison */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"   /* códigos de token e YYSTYPE */

/* Quantidade de erros léxicos encontrados; lido por src/main.c */
int lex_errors = 0;
%}

%option noyywrap
%option yylineno
%option noinput nounput

/* DEFINIÇÕES DE EXPRESSÕES REGULARES */
DIGITO      [0-9]
INT_LIT     {DIGITO}+
FLOAT_LIT   {DIGITO}+\.{DIGITO}+
CHAR_LIT    \'([^'\\\n]|\\.)\'
STRING_LIT  \"([^"\\\n]|\\.)*\"

%%

    /* 1. COMENTÁRIOS (IGNORADOS) */
"//".*                      { /* Ignora comentário de linha */ }
"/*"([^*]|\*+[^/*])*"*"+"/" { /* Ignora comentário de bloco */ }

    /* 2. LITERAIS NUMÉRICOS E DE TEXTO */
{FLOAT_LIT}  { yylval.fval = atof(yytext);  return FLOAT_LITERAL; }
{INT_LIT}    { yylval.ival = atoi(yytext);  return INT_LITERAL; }
{CHAR_LIT}   { yylval.sval = strdup(yytext); return CHAR_LITERAL; }
{STRING_LIT} { yylval.sval = strdup(yytext); return STRING_LITERAL; }

    /* 3. PALAVRAS RESERVADAS (TIPOS DE DADOS) */
"int"       { return INT; }
"float"     { return FLOAT; }
"void"      { return VOID; }

    /* 4. PALAVRAS RESERVADAS (ESTRUTURAS DE CONTROLE E E/S) */
"if"        { return IF; }
"else"      { return ELSE; }
"while"     { return WHILE; }
"for"       { return FOR; }
"do"        { return DO; }
"return"    { return RETURN; }
"print"     { return PRINT; }

    /* 5. IDENTIFICADORES */
[a-zA-Z_][a-zA-Z0-9_]* { yylval.sval = strdup(yytext); return IDENTIFIER; }

    /* 6. OPERADORES DE INCREMENTO E DECREMENTO */
"++"        { return INC; }
"--"        { return DEC; }

    /* 7. OPERADORES DE ATRIBUIÇÃO COMPOSTA */
"+="        { return ADD_ASSIGN; }
"-="        { return SUB_ASSIGN; }
"*="        { return MUL_ASSIGN; }
"/="        { return DIV_ASSIGN; }

    /* 8. OPERADORES RELACIONAIS (COMPARAÇÃO) */
"=="        { return EQ; }
"!="        { return NE; }
"<="        { return LE; }
">="        { return GE; }
"<"         { return LT; }
">"         { return GT; }

    /* 9. OPERADORES LÓGICOS */
"&&"        { return AND; }
"||"        { return OR; }
"!"         { return NOT; }

    /* 10. OPERADOR DE ATRIBUIÇÃO SIMPLES */
"="         { return ASSIGN; }

    /* 11. OPERADORES ARITMÉTICOS */
"+"         { return PLUS; }
"-"         { return MINUS; }
"*"         { return TIMES; }
"/"         { return DIVIDE; }
"%"         { return MOD; }

    /* 12. DELIMITADORES E PONTUAÇÃO */
"{"         { return LBRACE; }
"}"         { return RBRACE; }
"("         { return LPAREN; }
")"         { return RPAREN; }
";"         { return SEMI; }
","         { return COMMA; }

    /* TRATAMENTO DE ERROS LÉXICOS PARA LITERAIS NÃO FECHADOS */
    /* Erros vão para stderr; o trecho inválido é descartado e a análise continua. */
\"([^"\\\n]|\\.)*   { fprintf(stderr, "ERRO LÉXICO (Linha %d): String não fechada '%s'\n", yylineno, yytext); lex_errors++; }
'([^'\\\n]|\\.)*    { fprintf(stderr, "ERRO LÉXICO (Linha %d): Char não fechado '%s'\n", yylineno, yytext); lex_errors++; }

    /* Ignora espaços em branco, tabulações e quebras de linha */
[ \t\n\r]+  { /* ignorar */ }

    /* Regra de erro léxico para caracteres inválidos */
.           { fprintf(stderr, "ERRO LÉXICO (Linha %d): Caractere desconhecido '%s'\n", yylineno, yytext); lex_errors++; }

%%
