# Resultados — Sprint 3

**Período planejado:** 14/10/2026 – 25/10/2026 *(replanejado; originalmente 24/09 – 14/10)*
**Período real:** a última entrega entrou no `main` em 03/11/2026
**Sprint:** [Sprint 3 — AST & Semântica Inicial](../sprints/sprint-3.md)

## Resumo

A Sprint 3 deu ao compilador a estrutura interna que as etapas de geração de código precisam. O parser agora monta uma **árvore sintática abstrata** com o tipo de cada expressão anotado, mantém uma **tabela de símbolos** com escopos aninhados e faz a **análise semântica**, reportando erros com o número da linha. A suíte automatizada passou de 15 para 37 programas, todos aprovados.

Todas as entregas planejadas foram concluídas, mas com atraso em relação aos prazos internos: o que estava previsto para 16/10, 20/10 e 25/10 entrou no `main` em 22/10, 27/10 e 03/11.

| Frente | Issue | Responsável | PR | Entrou no `main` |
|---|---|---|---|---|
| B — Tabela de símbolos | [#18](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/18) | Gustavo Bonifácio | [#23](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/23) | 22/10 |
| C — Análise semântica | [#19](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/19) | Gabriel Diniz | [#25](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/25) | 27/10 |
| Ajustes — gramática e equipe | [#16](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/16) | Gabriel Di Angellis | [#24](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/24), reintegrado no [#27](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/27) | 03/11 |
| D — Testes semânticos | [#20](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/20) | Gabriel Di Angellis | [#24](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/24), reintegrado no [#27](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/27) | 03/11 |
| A — AST | [#17](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/17) | Paulo Vitor Gomes | [#27](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/27) | 03/11 |

O PR #24 ficou em conflito com a tabela de símbolos e foi fechado; seu conteúdo entrou junto com a AST (ver [Problemas e soluções](../problemas-e-solucoes.md#pr-em-conflito-com-o-restante-da-sprint-3)).

## Entregas

### Tabela de símbolos — `src/symtab.c`

Pilha de escopos com abertura e fechamento, inserção de variáveis, parâmetros e funções (com tipo de retorno e tipos dos parâmetros), busca no escopo atual e em todos os escopos visíveis. Detecta redeclaração no mesmo escopo e permite sombreamento em escopo interno. Tem testes unitários próprios, que não dependem do parser (`make test-symtab`).

### Análise semântica — `src/semantico.c`

Detecta, com o número da linha:

- variável ou função não declarada, e uso fora do escopo;
- redeclaração no mesmo escopo, inclusive de parâmetro;
- `void` em variável ou parâmetro;
- número ou tipo de argumentos errado numa chamada;
- `return` incompatível com o tipo da função, com valor numa função `void` ou sem valor numa função que deve retornar;
- função `void` usada como valor;
- atribuição de `float` a `int`;
- literal de texto ou de caractere fora do `print`.

As regras de tipo estão em [Sobre o Projeto](../projeto.md#regras-de-tipos-e-análise-semântica-sprint-3).

### Árvore sintática abstrata — `src/ast.c`

Cada regra do `parser.y` constrói seu nó, e a raiz fica disponível para as etapas seguintes. Cada nó guarda a linha exata da construção e, nas expressões, o tipo calculado pela análise semântica. `./build/compilador --ast < programa.c` imprime a árvore; há um exemplo em [Arquitetura do compilador](../projeto.md#arquitetura-do-compilador).

### Ajustes de gramática

- Declarações múltiplas: `int a, b = 2, c;`.
- Declarações e comandos intercalados dentro de um bloco.

### Testes automatizados

`make test` compila o compilador, verifica que a gramática não tem conflitos e executa todos os programas de teste, conferindo o código de saída e a mensagem de erro esperada. Os programas inválidos declaram o erro esperado na primeira linha. O detalhamento de cada caso está no [guia do corpus](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/blob/main/testes/sprint3/README.md).

## Validação

Executada no `main` em `6416857`, em 04/11/2026.

| Verificação | Resultado |
|---|---|
| `make test`, Sprint 2 | 11 válidos aceitos, 4 inválidos rejeitados |
| `make test`, Sprint 3 | 6 válidos aceitos, 16 inválidos rejeitados com a mensagem esperada |
| **Total** | **37 aprovados, 0 reprovados** |
| `make test-runner` | 9 verificações do próprio executor de testes aprovadas |
| `make test-symtab` | Testes unitários da tabela de símbolos aprovados |
| `make check-grammar` | Zero conflitos shift/reduce e reduce/reduce; autômato LALR com 154 estados |

## Limitações conhecidas

- **A análise semântica roda dentro das ações do parser**, e não numa passagem sobre a AST. A mudança está planejada para a Sprint 5 ([#26](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/26)).
- **A linha de algumas mensagens semânticas pode ser a seguinte à do erro**, porque usa a posição do analisador léxico no momento da redução. A AST já tem a linha correta; a correção vem com a #26.

## Próximos passos (Sprint 4)

- Gerar o código intermediário e o TypeScript a partir da AST.
- Testar o código gerado executando-o.
