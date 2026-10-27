/* Ponto de entrada do compilador Mini-C: lê o código-fonte da entrada padrão.
 * Uso: ./build/compilador [--ast] < programa.c */

#include <stdio.h>
#include <string.h>
#include "ast.h"

int yyparse(void);
void liberar_tabela_simbolos(void);
extern int lex_errors;   /* definido em scanner.l */
extern int sem_errors;   /* definido em parser.y */
extern No *ast_raiz;     /* definido em parser.y */

int main(int argc, char **argv) {
    int mostra_ast = 0;
    if (argc == 2 && strcmp(argv[1], "--ast") == 0) {
        mostra_ast = 1;
    } else if (argc > 1) {
        fprintf(stderr, "Uso: %s [--ast] < programa.c\n", argv[0]);
        return 2;
    }

    int erro_sintatico = yyparse();
    liberar_tabela_simbolos();

    /* Com erro léxico ou sintático a árvore está incompleta e não é impressa. */
    if (mostra_ast && erro_sintatico == 0 && lex_errors == 0) {
        ast_imprime(ast_raiz, stdout);
    }
    ast_libera(ast_raiz);

    return (erro_sintatico != 0 || lex_errors > 0 || sem_errors > 0) ? 1 : 0;
}
