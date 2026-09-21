/* FGA0003 - Compiladores 1 */
/* Driver de depuração do analisador léxico: imprime cada token reconhecido,
 * sem passar pelo parser. Uso: make lex-debug && ./build/lex_debug < arquivo.txt */

#include <stdio.h>
#include "y.tab.h"

int yylex(void);
extern char *yytext;

/* Esta tabela precisa ser mantida em sincronia com os %token de src/parser.y. */
static const char *token_name(int t) {
#define T(x) case x: return #x;
    switch (t) {
    T(INT_LITERAL) T(FLOAT_LITERAL) T(IDENTIFIER) T(CHAR_LITERAL) T(STRING_LITERAL)
    T(INT) T(FLOAT) T(VOID)
    T(IF) T(ELSE) T(WHILE) T(FOR) T(DO) T(RETURN) T(PRINT)
    T(INC) T(DEC)
    T(ADD_ASSIGN) T(SUB_ASSIGN) T(MUL_ASSIGN) T(DIV_ASSIGN)
    T(EQ) T(NE) T(LE) T(GE) T(LT) T(GT)
    T(AND) T(OR) T(NOT)
    T(ASSIGN)
    T(PLUS) T(MINUS) T(TIMES) T(DIVIDE) T(MOD)
    T(LBRACE) T(RBRACE) T(LPAREN) T(RPAREN) T(SEMI) T(COMMA)
    default: return "DESCONHECIDO";
    }
#undef T
}

int main(void) {
    int t;
    while ((t = yylex()) != 0) {
        printf("Token: %s (Lexema: %s)\n", token_name(t), yytext);
    }
    return 0;
}
