# Sprint 4 — Código Intermediário, TypeScript & P2

**Período:** 26/10/2026 – 06/11/2026 *(replanejado; ver [Ajustes no planejamento](../cronograma.md#ajustes-no-planejamento))*
**Marco:** [P2 / Entrega Final](../p2.md) em 06/11/2026 — 23:59
**Status:** Em andamento

## Ponto de partida

A Sprint 3 entregou a AST com os tipos anotados, a tabela de símbolos e a análise semântica, com os 37 casos do `make test` passando (ver [Resultados da Sprint 3](../resultados/sprint-3.md)). Falta transformar a árvore em código.

## Objetivos Principais

- Implementar a geração de **código intermediário** (código de três endereços).
- Implementar a geração de **TypeScript**, antecipada da Sprint 5 para a P2 mostrar o compilador funcionando de ponta a ponta.
- **Testar o código gerado executando-o**, e não apenas verificando que o compilador termina sem erro.
- Completar a **documentação** do repositório e preparar a **P2**.

## Frentes de trabalho

| Frente | Issue | Responsável | Foco |
|---|---|---|---|
| A | [#28](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/28) | Gabriel Di Angellis | Código intermediário: estrutura, `--tac`, expressões e atribuições |
| B | [#29](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/29) | Gabriel Diniz | Código intermediário: controle de fluxo, funções e `print` |
| C | [#30](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/30) | Gustavo Bonifácio | Geração de TypeScript (`--ts`) |
| D | [#31](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/31) | Gabriel Di Angellis | Testes executando o código gerado |
| E | [#32](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/32) | Paulo Vitor Gomes | Documentação final |
| P2 | [#33](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/33) | Paulo Vitor Gomes | Formulário e apresentação |

Os responsáveis foram definidos para equilibrar a distribuição de commits entre os quatro integrantes.

## Dependências

- A Frente B usa a interface `tac.h` da Frente A.
- A Frente C gera TypeScript a partir da AST, e não do código intermediário, então não depende de A e B (ver [Decisões técnicas](../decisoes-tecnicas.md)).
- A Frente D executa o que A, B e C geram; os programas e as saídas esperadas podem ser escritos antes.

## Principais Entregas

- `src/tac.h` e `src/tac.c`, com `./build/compilador --tac < programa.c`.
- `src/gerador_ts.h` e `src/gerador_ts.c`, com `./build/compilador --ts < programa.c`.
- `testes/sprint4/`, com a saída esperada de cada programa e o `make test` executando o TypeScript gerado.
- Documentação: arquitetura, decisões técnicas, problemas e soluções, resultados das sprints.
- Formulário do P2 preenchido: <https://forms.office.com/r/gNG6Eb7e71>
- Apresentação do ponto de controle P2.

## Fora desta sprint

- Análise semântica como passagem sobre a AST ([#26](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/26)), adiada para a Sprint 5.

## O que o P2 exige (ver [página do P2](../p2.md))

- Funcionalidades principais desenvolvidas
- Melhorias desde o P1
- Ajustes no planejamento

## Retrospectiva

`TODO: preencher ao final da sprint.`
