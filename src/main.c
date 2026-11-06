
/* Ponto de entrada do compilador Mini-C.
 * Uso:
 *   ./build/compilador < programa.c
 *   ./build/compilador --ast < programa.c
 *   ./build/compilador --ts < programa.c
 */

#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "gerador_ts.h"

int yyparse(void);
void liberar_tabela_simbolos(void);

extern int lex_errors;
extern int sem_errors;
extern No *ast_raiz;

/* Indica ao parser que a saída será TypeScript. */
int modo_typescript = 0;

int main(int argc, char **argv) {
    int mostra_ast = 0;

    if (argc == 2 && strcmp(argv[1], "--ast") == 0) {
        mostra_ast = 1;
    } else if (argc == 2 && strcmp(argv[1], "--ts") == 0) {
        modo_typescript = 1;
    } else if (argc > 1) {
        fprintf(stderr,
                "Uso: %s [--ast | --ts] < programa.c\n",
                argv[0]);
        return 2;
    }

    int erro_sintatico = yyparse();

    liberar_tabela_simbolos();

    int houve_erros =
        erro_sintatico != 0 ||
        lex_errors > 0 ||
        sem_errors > 0;

    if (!houve_erros) {
        if (mostra_ast) {
            ast_imprime(ast_raiz, stdout);
        } else if (modo_typescript) {
            gerar_typescript(ast_raiz, stdout);
        }
    }

    ast_libera(ast_raiz);

    return houve_erros ? 1 : 0;
}
