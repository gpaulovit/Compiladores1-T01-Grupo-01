# Sprint 3 — AST & Semântica Inicial

**Período:** 14/10/2026 – 25/10/2026 
**Status:** Em andamento

## Ponto de partida

A Sprint 2 entregou o léxico e a gramática completos, com zero conflitos (ver [Resultados da Sprint 2](../resultados/sprint-2.md)). As ações semânticas do `parser.y` apenas imprimem o que foi reconhecido, e a única checagem semântica existente rejeita `void` como tipo de variável.

O período original desta sprint terminou sem que nenhuma de suas entregas tivesse começado, e a equipe passou de 5 para 4 integrantes. Por isso a sprint foi replanejada, e o trabalho foi redistribuído entre os quatro.

## Objetivos Principais

- Construir a **AST** (árvore sintática abstrata), substituindo os `printf` das ações semânticas pela construção de nós.
- Implementar a **tabela de símbolos**, com escopos aninhados.
- Implementar a **análise semântica**: declarações, escopos e tipos.
- **Automatizar os testes**, cobrindo também os erros semânticos.
- Remover duas limitações da gramática registradas na Sprint 2.

## Frentes de trabalho

| Frente | Issue | Responsável | Foco |
|---|---|---|---|
| Ajustes | [#16](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/16) | Gabriel Di Angellis | Listas de declaração (`int a, b;`), declarações intercaladas com comandos e equipe atualizada na documentação |
| A | [#17](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/17) | Paulo Vitor Gomes | Árvore sintática abstrata |
| B | [#18](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/18) | Gustavo Bonifácio | Tabela de símbolos |
| C | [#19](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/19) | Gabriel Diniz | Análise semântica |
| D | [#20](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/20) | Gabriel Di Angellis | Testes semânticos e `make test` |

## Ordem e prazos internos

As frentes dependem umas das outras, então a ordem importa:

| Até | Marco |
|---|---|
| 16/10 | PR de Ajustes mergeado, pois mexe no mesmo `parser.y` que a AST vai reescrever. Interfaces `ast.h` e `symtab.h` definidas, o que desbloqueia a Frente C. |
| 20/10 | AST e tabela de símbolos completas. |
| 25/10 | Análise semântica e suíte de testes semânticos concluídas, com `make test` passando. |

A tabela de símbolos (B) não depende de nenhuma outra frente e pode começar no primeiro dia. A Frente C começa pelas regras de tipo e pelo texto das mensagens de erro, combinado com a Frente D, enquanto A e B entregam as interfaces.

## Principais Entregas

- `src/ast.h` e `src/ast.c`, com a AST construída pelo `parser.y` e impressa com `--ast`.
- `src/symtab.h` e `src/symtab.c`, com pilha de escopos e testes unitários.
- Passagem semântica detectando, com o número da linha:
  - identificador não declarado;
  - redeclaração;
  - `void` em variável ou parâmetro;
  - aridade e tipos de argumentos;
  - `return` incompatível;
  - função `void` usada como valor;
  - `string`/`char` fora do `print`.
- Regras de tipo entre `int` e `float` documentadas em [Sobre o Projeto](../projeto.md).
- `testes/sprint3/` e o alvo `make test`, rodando as suítes da Sprint 2 e da Sprint 3.

## Observações

- As quartas-feiras continuam sendo o dia de integração.

## Retrospectiva

`TODO: preencher ao final da sprint.`
