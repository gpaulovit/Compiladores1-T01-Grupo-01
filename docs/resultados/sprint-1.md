# Resultados — Sprint 1

**Período:** 24/08/2026 – 06/09/2026
**Sprint:** [Sprint 1 — Formação & Definição da Linguagem](../sprints/sprint-1.md)
**Issues:** [#2](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/2) · [#3](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/3) · [#4](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/4)

## Resumo

Além do planejamento previsto (formação da equipe, definição do escopo do Mini-C), a equipe adiantou entregas antes previstas apenas para a Sprint 2: o **analisador léxico completo** (Flex) do Mini-C, cobrindo a totalidade dos tokens definidos no escopo — palavras reservadas, identificadores, operadores, delimitadores, literais, comentários e erros léxicos.

O trabalho da sprint foi dividido em três frentes (issues), cada uma correspondendo a um PR:

| Frente | Issue | PR | Status |
|---|---|---|---|
| Frente 1 — Palavras reservadas, identificadores e tipos | [#2](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/2) | [#6](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/6) | ✅ Concluída |
| Frente 2 — Operadores, delimitadores e literais | [#3](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/3) | [#5](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/5) | ✅ Concluída |
| Frente 3 — Definição do Mini-C e documentação da linguagem | [#4](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/4) | [#4](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/4) | [#7](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/7) | ✅ Concluída |
## Entregas

### 1. Definição do escopo do Mini-C

- Escopo (tipos, operadores, controle, funções, E/S) e itens fora de escopo documentados em [Sobre o Projeto → Linguagem-fonte: Mini-C](../projeto.md#linguagem-fonte-mini-c).

### 2. Analisador léxico (Flex) — [`src/scanner.l`](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/blob/main/src/scanner.l)

#### PR #5 — Operadores, delimitadores, literais e comentários

- **Autor:** Gabriel Diniz ([@GabrielDiniz12](https://github.com/GabrielDiniz12))
- **Merge:** 09/09/2026 · [gpaulovit/Compiladores1-T01-Grupo-01#5](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/5) · fecha a issue [#3](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/3)
- Reconhecimento de todos os símbolos fixos: aritméticos (`+ - * / %`), relacionais (`== != < <= > >=`), lógicos (`&& || !`), atribuição simples e composta (`= += -= *= /=`), incremento/decremento (`++ --`) e delimitadores (`{ } ( ) ; ,`).
- Expressões regulares para literais numéricos (`int`, `float`) e de texto (`char`, `string`).
- Descarte de comentários de linha (`//`) e de bloco (`/* */`).
- Tratamento de erro léxico com número da linha para strings e chars não fechados.
- Testes: [`testes/op_validos_lexico.txt`](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/blob/main/testes/op_validos_lexico.txt), [`testes/op_invalidos_lexico.txt`](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/blob/main/testes/op_invalidos_lexico.txt), [`testes/literais_comentarios_validos.txt`](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/blob/main/testes/literais_comentarios_validos.txt), [`testes/literais_comentarios_invalidos.txt`](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/blob/main/testes/literais_comentarios_invalidos.txt).

#### PR #6 — Palavras reservadas e identificadores

- **Autor:** Henrique Mendes ([@henriquemendeselias](https://github.com/henriquemendeselias)), com testes de Gabriel Di Angellis ([@diangellis](https://github.com/diangellis))
- **Merge:** 09/09/2026 · [gpaulovit/Compiladores1-T01-Grupo-01#6](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/6) · fecha a issue [#2](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/2)
- Reconhecimento dos tipos primitivos (`int`, `float`, `void`) e das palavras-chave de controle/E-S (`if`, `else`, `while`, `for`, `do`, `return`, `print`).
- Identificadores via `[a-zA-Z_][a-zA-Z0-9_]*`, declarados **depois** das palavras reservadas no `.l` para que estas tenham prioridade em caso de empate de lexema.
- Testes: [`testes/frente1_validos.txt`](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/blob/main/testes/frente1_validos.txt), [`testes/frente1_invalidos.txt`](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/blob/main/testes/frente1_invalidos.txt).

A tabela completa de tokens gerados por cada símbolo está em [Sobre o Projeto → Especificação de Símbolos Fixos](../projeto.md#especificação-de-símbolos-fixos-operadores-e-delimitadores).

## Como rodar localmente

```bash
flex -o lex.yy.c src/scanner.l
gcc lex.yy.c -o scanner

./scanner < testes/op_validos_lexico.txt
./scanner < testes/op_invalidos_lexico.txt
./scanner < testes/literais_comentarios_validos.txt
./scanner < testes/literais_comentarios_invalidos.txt
./scanner < testes/frente1_validos.txt
./scanner < testes/frente1_invalidos.txt
```

## Próximos passos (Sprint 2)

- Fechar a issue [#4](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/4) (revisão final da definição da linguagem).
- Iniciar a análise sintática (Bison), conforme [Sprint 2](../sprints/sprint-2.md).
