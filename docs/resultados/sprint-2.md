# Resultados — Sprint 2

**Período:** 07/09/2026 – 23/09/2026
**Sprint:** [Sprint 2 — Sintático & P1](../sprints/sprint-2.md)
**Marco:** [P1](../p1.md) em 23/09/2026

## Resumo

A Sprint 2 integrou o analisador léxico ao Bison e implementou a **gramática livre de contexto completa do Mini-C**, cobrindo todo o escopo definido na [Sprint 1](sprint-1.md): declarações e tipos, expressões com precedência, estruturas de controle, funções com recursão e o comando `print`.

Ao fim da sprint o compilador reconhece programas Mini-C inteiros, reporta erros léxicos, sintáticos e o primeiro erro semântico, e a gramática é gerada pelo Bison com **zero conflitos** shift/reduce ou reduce/reduce.

| Frente | Issue | PR | Status |
|---|---|---|---|
| Frente A — Gramática Bison: declarações, tipos e expressões | [#8](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/8) | [#12](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/12), [#15](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/15) | ✅ Concluída |
| Frente B — Estruturas de controle, funções e `print` | [#9](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/9) | [#13](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/13) | ✅ Concluída |
| Frente C — Testes de integração léxico + sintático | [#10](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/10) | [#14](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/14) | 🔄 Em revisão |
| Checkpoint P1 | [#11](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/11) | — | 🔄 Em andamento |

## Entregas

### 1. Integração léxico → sintático (PR #12)

O `scanner.l` deixou de ser um programa autônomo e passou a ser consumido pelo parser: inclui `y.tab.h`, cada ação retorna o código do token, `yylval` é preenchido para identificadores e literais, e o `main()` saiu para `src/main.c`. Foram criados o `Makefile` e o driver `src/lex_debug.c`, que preserva os testes léxicos da Sprint 1.

### 2. Gramática do Mini-C (PR #13)

Arquivo [`src/parser.y`](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/blob/main/src/parser.y) com a gramática completa. Dois pontos de projeto:

- **Precedência por declaração, não por camadas.** As expressões são escritas de forma ambígua (`expressao : expressao PLUS expressao | ...`) e a ambiguidade é resolvida pelo bloco `%left`/`%right`, ordenado da menor para a maior precedência. É o que mantém a gramática legível e com zero conflitos.
- **`%prec LOWER_THAN_ELSE`** resolve o *dangling else*, fazendo o `else` ligar-se ao `if` mais próximo.

### 3. Ações semânticas e checagem de `void` (PR #15)

- Ações semânticas nas regras de declarações, parâmetros e expressões: cada declaração, atribuição, incremento e operador reconhecido é relatado, e o fim da análise imprime um resumo com a contagem de expressões aritméticas, relacionais e lógicas.
- Primeira **checagem semântica**: `void` é rejeitado como tipo de variável e de parâmetro, conforme o escopo, que o define apenas como tipo de retorno de função. A checagem é semântica e não sintática de propósito — separar os tipos na gramática criaria conflito reduce/reduce, já que ao ver `int nome` o parser LALR(1) ainda não sabe se vem uma variável ou uma função.
- `src/main.c` passou a considerar erros semânticos no código de saída.

## Validação

Executada sobre o corpus de 15 programas Mini-C da Frente C (PR #14), que até então nunca havia sido compilado.

| Conjunto | Arquivos | Resultado |
|---|---|---|
| `testes/sprint2/validos/` | 11 | ✅ todos aceitos (saída 0) |
| `testes/sprint2/invalidos/` | 4 | ✅ todos rejeitados (saída 1) |

Verificações adicionais:

- **Zero conflitos:** `bison -Wcounterexamples` não reporta nenhum conflito nem contraexemplo.
- **Precedência, de forma observável:** como as ações disparam na redução, a ordem das linhas revela a árvore. Em `r = 1 + 2 * 3` o `*` é reduzido antes do `+`; em `r = (1 + 2) * 3` a ordem inverte.
- **`void`:** `void x;` é rejeitado com erro semântico e saída 1, enquanto `void mostrar() { ... }` continua válido.
- **Sem regressão no léxico:** os 6 arquivos de `testes/` produzem no `lex_debug` saída idêntica à da Sprint 1.
- **Exemplo da documentação:** o programa de [Sobre o Projeto](../projeto.md#exemplo-de-código-fonte) é aceito, com a contagem correta de 1 expressão aritmética e 2 relacionais.

## Limitações conhecidas

Registradas de forma consciente, para não serem confundidas com defeitos:

- **Declarações antes dos comandos.** `bloco : LBRACE declaracoes_locais comandos RBRACE`, no estilo C89, então `int x; x = 1; int y;` é rejeitado.
- **Sem listas de variáveis.** `int a, b;` não é aceito; cada variável exige sua própria declaração. A vírgula só separa parâmetros e argumentos.
- **Literais de texto são aceitos em qualquer expressão**, embora o escopo restrinja `string`/`char` ao uso em `print`. Restringir isso depende de verificação de tipos, prevista para a Sprint 3.
- **`int main(void)` é rejeitado**, pois `parametro` exige um identificador. O escopo e o corpus de testes usam `main()`.
- **Sem verificação de tipos, escopo ou declaração prévia.** Nenhuma tabela de símbolos ainda: é o objeto da Sprint 3.

## Como rodar localmente

```bash
make                                  # gera build/compilador
./build/compilador < programa.c       # análise léxica + sintática

make lex-debug                        # gera build/lex_debug
./build/lex_debug < testes/frente1_validos.txt
```

## Próximos passos (Sprint 3)

- Construir a **AST** e a **tabela de símbolos**, substituindo os `printf` das ações semânticas por construção de nós.
- Verificação de tipos e de escopo, incluindo variável não declarada e uso de `string`/`char` fora do `print`.
- Concluir a Frente C (#10) com o merge do corpus de testes.
