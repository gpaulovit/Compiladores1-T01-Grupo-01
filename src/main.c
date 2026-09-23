/* Ponto de entrada do compilador Mini-C: lê o código-fonte da entrada padrão. */

int yyparse(void);
extern int lex_errors;   /* definido em scanner.l */
extern int sem_errors;   /* definido em parser.y */

int main(void) {
    int erro_sintatico = yyparse();
    return (erro_sintatico != 0 || lex_errors > 0 || sem_errors > 0) ? 1 : 0;
}
