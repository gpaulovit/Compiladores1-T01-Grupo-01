# Sprint 3 — Sintático & Semântica Inicial

**Período:** 24/09/2026 – 14/10/2026
**Status:** `TODO: Planejado / Em andamento / Concluído`

## Objetivos Principais

- Evoluir o **analisador sintático** com novas produções gramaticais do Mini-C.
- Iniciar a estrutura interna do compilador (AST, tabela de símbolos).
- Dar os primeiros passos na **análise semântica** (tipos de variáveis, escopos).

## Principais Entregas

- Parser com cobertura mais ampla da gramática do Mini-C (estruturas de controle, declarações etc.).
- Estruturas de dados (AST e tabela de símbolos) definidas e parcialmente implementadas.
- Analisador semântico inicial identificando erros básicos (variáveis não declaradas, incompatibilidade de tipos simples).

## Tarefas e Atividades

1. Estender as regras gramaticais no Bison, cobrindo as principais construções do Mini-C.
2. Criar e popular a tabela de símbolos durante a análise sintática.
3. Construir a AST (árvore sintática abstrata) para facilitar a análise semântica e as etapas seguintes.
4. Implementar verificação de tipos e de escopo simples, reportando erros quando algo estiver fora das regras da linguagem.

## Observações

- As quartas-feiras continuam essenciais para o trabalho prático no compilador.
- Manter testes de unidade (pequenos trechos de código-fonte Mini-C) para validar o parser e a semântica.
- Já começar a planejar a geração de código intermediário, que será o foco da Sprint 4.

## Retrospectiva

`TODO: preencher ao final da sprint.`
