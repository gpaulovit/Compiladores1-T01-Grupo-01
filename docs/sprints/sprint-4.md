# Sprint 4 — Código Intermediário, TypeScript & P2

**Período:** 26/10/2026 – 06/11/2026
**Marco:** [P2 / Entrega Final](../p2.md) em 06/11/2026 — 23:59
**Status:** Concluído (ver [Resultados da Sprint 4](../resultados/sprint-4.md))

## Ponto de partida

A Sprint 3 entregou a AST com os tipos anotados, a tabela de símbolos e a análise semântica, com os 37 casos do `make test` passando (ver [Resultados da Sprint 3](../resultados/sprint-3.md)). Faltava transformar a árvore em código.

## Objetivos Principais

- Implementar a geração de **código intermediário** (código de três endereços, TAC).
- Implementar a geração de **TypeScript**, para a P2 mostrar o compilador funcionando de ponta a ponta.
- **Testar o código gerado executando-o**, e não apenas verificando que o compilador termina sem erro.
- Completar a **documentação** do repositório e preparar a **P2**.

## Frentes de trabalho

| Frente | Issue | Responsável | Entrega |
|---|---|---|---|
| A | [#28](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/28) | Gabriel Di Angellis | Código intermediário: expressões e atribuições — [PR #35](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/35) |
| B | [#29](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/29) | Gabriel Diniz | Código intermediário: interface `tac.h`, controle de fluxo, funções e `print` — [PR #35](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/35) |
| C | [#30](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/30) | Gustavo Bonifácio | Geração de TypeScript (`--ts`) — [PR #34](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/34) |
| D | [#31](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/31) | Gabriel Di Angellis | Testes executando o código gerado — [PR #35](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/35) |
| E | [#32](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/32) | Paulo Vitor Gomes | Documentação final |
| P2 | [#33](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/33) | Paulo Vitor Gomes | Formulário e apresentação |

Os responsáveis foram definidos para equilibrar a distribuição de commits entre os quatro integrantes.

## Dependências

- A Frente A usa a interface `tac.h`, que acabou sendo definida pela Frente B junto com o controle de fluxo.
- A Frente C gera TypeScript a partir da AST, e não do código intermediário, então não dependeu de A e B (ver [Decisões técnicas](../decisoes-tecnicas.md#typescript-gerado-a-partir-da-ast)).
- A Frente D executa o que A, B e C geram.

## Principais Entregas

- [x] `src/intermediario/tac.h` e `src/intermediario/tac.c`, com `./build/compilador --tac < programa.c`.
- [x] `src/gerador/gerador_ts.h` e `src/gerador/gerador_ts.c`, com `./build/compilador --ts < programa.c`.
- [x] `testes/sprint4/`, com a saída esperada (`.saida`) e o TAC esperado (`.tac`) de cada programa; `make test` executa o TypeScript gerado com Node e compara os resultados.
- [x] Documentação: arquitetura, código intermediário, linguagem-alvo, decisões técnicas, problemas e soluções, resultados das sprints.
- [ ] Formulário e apresentação do P2 (#33).

## Fora desta sprint

- Análise semântica como passagem sobre a AST ([#26](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/26)), planejada para a Sprint 5.

## O que o P2 exige (ver [página do P2](../p2.md))

- Funcionalidades principais desenvolvidas
- Melhorias desde o P1
- Ajustes no planejamento

## Retrospectiva

- **Funcionou:** gerar o TypeScript a partir da AST tirou a Frente C da fila de dependências, e ela pôde avançar em paralelo ao código intermediário. Os testes passaram a executar o código gerado e a comparar a saída com a esperada, o que verifica o comportamento e não só o texto.
