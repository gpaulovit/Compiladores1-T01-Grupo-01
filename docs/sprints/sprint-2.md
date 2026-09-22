# Sprint 2 — Sintático & P1

**Período:** 07/09/2026 – 23/09/2026
**Marco:** [P1](../p1.md) em 23/09/2026 — 23:59
**Status:** `TODO: Planejado / Em andamento / Concluído`

## Ponto de partida

O **analisador léxico (Flex) já foi concluído na [Sprint 1](../resultados/sprint-1.md)** (PRs #5 e #6), então esta sprint não retrabalha o léxico: ela parte dele. O protótipo da gramática Bison, previsto originalmente para a Sprint 1, não chegou a ser entregue e passa a ser o foco desta sprint.

## Objetivos Principais

- Adaptar o `scanner.l` existente para integração com o Bison (hoje ele só imprime tokens e tem `main()` próprio).
- Iniciar a **análise sintática** (Bison): gramática do Mini-C para declarações, expressões, estruturas de controle, funções e `print`.
- Validar lexer + parser em conjunto com programas Mini-C completos.
- Preparar o material para o ponto de controle **P1**.

## Frentes de trabalho

| Frente | Issue | Foco |
|---|---|---|
| A | [#8](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/8) | Adaptação do `scanner.l` para o Bison + gramática de declarações, tipos e expressões |
| B | [#9](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/9) | Gramática de estruturas de controle, funções e `print` |
| C | [#10](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/10) | Testes de integração léxico + sintático |
| P1 | [#11](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/11) | Checkpoint P1 — apresentação e formulário |

## Principais Entregas

- `scanner.l` adaptado para o Bison: retorna os códigos dos tokens, preenche `yylval` para identificadores e literais e não possui mais `main()`. **Pré-requisito das Frentes B e C.**
- Arquivo `.y` com regras para declarações, tipos, expressões (aritméticas, relacionais, lógicas, incremento/decremento, atribuição composta), `if/else`, `while`, `for`, `do-while`, funções (incluindo `main` e recursão) e `print`, com ações semânticas simples.
- Conjunto de programas de teste em Mini-C, executados sobre lexer + parser, com registro dos resultados.
- Formulário do P1 preenchido: <https://forms.office.com/r/MyKh4HiAAu>
- Apresentação do ponto de controle P1.

## Tarefas e Atividades

1. Adaptar o `scanner.l` para integração com o Bison (Frente A — deve ser feito primeiro).
2. Escrever as regras gramaticais no Bison (Frentes A e B), tratando precedência/associatividade e o *dangling else*.
3. Criar exemplos de código-fonte Mini-C e rodar lexer + parser sobre eles, registrando problemas encontrados (Frente C).
4. Preparar a apresentação para o P1, conforme orientações do professor.
5. Preencher o formulário do P1 no link fornecido.

## O que o P1 exige (ver [página do P1](../p1.md))

- Definição do projeto
- Linguagem de programação escolhida
- Planejamento das sprints
- O que foi implementado até o momento

## Observações

- A sprint termina no dia da apresentação do P1 — reservar tempo para testar e ensaiar antes da data.
- As Frentes B e C dependem do `scanner.l` adaptado (Frente A); concluir esse item logo no início.
- Aproveitar as quartas-feiras para a daily meeting e verificação de tarefas pendentes.

## Retrospectiva

`TODO: preencher ao final da sprint.`
