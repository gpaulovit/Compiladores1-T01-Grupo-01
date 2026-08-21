# Sprint 3 — Sintático & Semântica Inicial

**Período:** 24/09/2026 – 14/10/2026
**Status:** `TODO: Planejado / Em andamento / Concluído`

## Objetivos Principais

- Evoluir o **analisador sintático** com novas produções gramaticais.
- Iniciar a **estrutura interna** do compilador (árvore sintática/AST, tabela de símbolos).
- Dar os primeiros passos na **análise semântica** (tipo de variáveis, escopos).

## Principais Entregas

- Parser com cobertura mais ampla da gramática do Mini-C (incluindo estruturas de controle, declarações etc.).
- Estrutura de dados (AST e tabela de símbolos) definidas e parcialmente implementadas.
- Analisador semântico inicial identificando erros básicos (variáveis não declaradas, tipos simples).

## Tarefas e Atividades

1. Estender as regras gramaticais no Bison, cobrindo as principais construções do Mini-C.
2. Criar e popular a tabela de símbolos durante a análise sintática.
3. Construir a AST (árvore sintática abstrata) para facilitar a análise semântica e a futura geração para TypeScript.
4. Implementar verificação de tipos e de escopo simples (reportar erros se algo estiver fora das regras).

## Observações

- As quartas-feiras continuam sendo essenciais para o hands-on do compilador.
- Manter testes de unidade (pequenos trechos de código-fonte) para validar o parser e a semântica.
- Já começar a pensar em como integrar a geração de código intermediário, que virá na Sprint 4.

## Retrospectiva

`TODO: preencher ao final da sprint.`
