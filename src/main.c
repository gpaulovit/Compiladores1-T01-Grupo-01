
/* Ponto de entrada do compilador Mini-C.
 * Uso:
 *   ./build/compilador < programa.c
 *   ./build/compilador --ast < programa.c
 *   ./build/compilador --ts < programa.c
 *   ./build/compilador --tac < programa.c
 */

#include <stdio.h>
#include <string.h>
#include "ast.h"
#include "gerador_ts.h"
#include "tac.h"

int yyparse(void);
void liberar_tabela_simbolos(void);

extern int lex_errors;
extern int sem_errors;
extern No *ast_raiz;

extern int modo_saida_codigo;

int main(int argc, char **argv) {
    int mostra_ast = 0;
    int mostra_ts = 0;
    int mostra_tac = 0;

    if (argc == 2 && strcmp(argv[1], "--ast") == 0) {
        mostra_ast = 1;
    } else if (argc == 2 && strcmp(argv[1], "--ts") == 0) {
        mostra_ts = 1;
    } else if (argc == 2 && strcmp(argv[1], "--tac") == 0) {
        mostra_tac = 1;
    } else if (argc > 1) {
        fprintf(stderr,
                "Uso: %s [--ast | --ts | --tac] < programa.c\n",
                argv[0]);
        return 2;
    }

    modo_saida_codigo = mostra_ts || mostra_tac;
    int erro_sintatico = yyparse();

    liberar_tabela_simbolos();

    int houve_erros =
        erro_sintatico != 0 ||
        lex_errors > 0 ||
        sem_errors > 0;

    if (!houve_erros) {
        if (mostra_ast) {
            ast_imprime(ast_raiz, stdout);
        } else if (mostra_ts) {
            gerar_typescript(ast_raiz, stdout);
        } else if (mostra_tac) {
            TacPrograma *tac = tac_gera(ast_raiz);
            tac_imprime(tac, stdout);
            tac_libera(tac);
        }
    }

    ast_libera(ast_raiz);

    return houve_erros ? 1 : 0;
}
