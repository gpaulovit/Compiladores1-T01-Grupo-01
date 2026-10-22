/* Ponto de entrada do compilador Mini-C: lê o código-fonte da entrada padrão. */

int yyparse(void);
void liberar_tabela_simbolos(void);
extern int lex_errors;   /* definido em scanner.l */
extern int sem_errors;   /* definido em parser.y */

int main(void) {
    int erro_sintatico = yyparse();
    liberar_tabela_simbolos();
    return (erro_sintatico != 0 || lex_errors > 0 || sem_errors > 0) ? 1 : 0;
}
