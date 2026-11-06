#ifndef GERADOR_TS_H
#define GERADOR_TS_H

#include <stdio.h>
#include "ast.h"

/* Gera código TypeScript a partir da AST do programa. */
void gerar_typescript(const No *raiz, FILE *saida);

#endif