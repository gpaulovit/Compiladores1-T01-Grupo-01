# Decisões técnicas

Cada decisão traz o que foi escolhido, o motivo e a alternativa descartada. A especificação completa da linguagem e da gramática está em [Sobre o Projeto](projeto.md).

## Linguagens e ferramentas

### TypeScript como linguagem-alvo

O guia da disciplina sugere assembly ou código de máquina simples. A equipe escolheu TypeScript para concentrar o trabalho nas etapas de análise e de geração de código, e para que a saída do compilador possa ser executada e verificada diretamente nas demonstrações e nos testes.

### C, Flex e Bison

Flex e Bison são fixados pela disciplina. O compilador é escrito em C, a mesma linguagem do código que as duas ferramentas geram, o que evita uma camada de integração. O build é feito com GNU Make e GCC.

### Escopo da linguagem fechado na Sprint 1

Ponteiros, structs, arrays multidimensionais e strings como tipo completo ficaram fora. Ponteiros, em especial, não têm correspondente em TypeScript, que não tem modelo de memória manual. A lista completa está em [Fora de escopo](projeto.md#fora-de-escopo-decisão-consciente).

## Análise léxica

### Erros léxicos não interrompem a análise

Um caractere inválido ou um literal não fechado é reportado em `stderr`, com o número da linha, e a análise continua. Assim uma execução aponta vários erros de uma vez, em vez de parar no primeiro.

### Um driver separado para depurar o léxico

Quando o scanner passou a ser consumido pelo parser, ele deixou de imprimir os tokens. Para não perder os testes do léxico da Sprint 1, o `src/lexer/lex_debug.c` imprime o fluxo de tokens sem passar pelo parser (`make lex-debug`). A alternativa, manter `printf` condicionais dentro do scanner, duplicaria cada regra.

## Análise sintática

### Precedência por declaração, não por camadas

As regras de expressão são escritas de forma ambígua (`expressao : expressao PLUS expressao | ...`), e a ambiguidade é resolvida pelo bloco `%left`/`%right` do Bison, ordenado da menor para a maior precedência. A alternativa clássica, uma gramática em camadas (`expressao → termo → fator`), exigiria um não-terminal por nível de precedência e tornaria a gramática bem mais longa.

### *Dangling else* resolvido por precedência

Em `if (a) if (b) x = 1; else x = 2;` há duas leituras possíveis, porque o `else` poderia pertencer a qualquer um dos dois `if`. Para o parser, isso é um conflito shift/reduce. O token `LOWER_THAN_ELSE`, que nunca é produzido pelo scanner, dá ao `if` sem `else` uma precedência menor que a do `ELSE`. Com isso o parser liga o `else` ao `if` mais próximo, como em C, e o conflito fica resolvido explicitamente em vez de aceito por omissão.

### Gramática sem nenhum conflito

O `make test` roda antes o alvo `check-grammar`, que trata qualquer conflito shift/reduce ou reduce/reduce como erro. Uma alteração na gramática que introduza um conflito faz os testes falharem.

### `void` restringido pela semântica, não pela gramática

O escopo permite `void` apenas como tipo de retorno de função. Separar `tipo` (com `void`) de `tipo_var` (sem `void`) na gramática gerou dois conflitos reduce/reduce. Ao ler `int nome`, o parser precisa decidir entre variável e função, mas a informação que distingue os dois casos (`(` ou `;`) só aparece dois tokens depois. Nenhum parser com um único token de *lookahead* resolve isso, nem o LR(1) canônico, como foi verificado com o Bison. Por isso a gramática usa um único `tipo`, e `void x;` é rejeitado por checagem semântica.

## Análise semântica e AST

### Regras de tipo entre `int` e `float`

`int` é promovido implicitamente a `float` em expressões, atribuições, inicializações, argumentos e `return`. Atribuir `float` a `int` é erro semântico, porque perderia a parte fracionária sem aviso. As regras completas estão em [Regras de tipos](projeto.md#regras-de-tipos-e-análise-semântica-sprint-3).

### Literais de texto só no `print`

A linguagem não tem tipo `string`. Literais de texto e de caractere são aceitos apenas como argumento de `print`, onde servem para mensagens. Em qualquer outra expressão, geram erro semântico.

### Nó genérico para a AST

Todo nó da AST tem a mesma estrutura: categoria, linha, tipo, operador, valor e até quatro filhos, mais um ponteiro para o próximo item de uma lista. O significado de cada filho por categoria está documentado em `src/ast/ast.h`. A alternativa, uma estrutura própria para cada construção, daria mais verificação em tempo de compilação, mas exigiria muito mais código de criação, impressão e liberação.

### Análise semântica dentro das ações do parser

Hoje cada regra do `parser.y` chama a análise semântica e grava o tipo calculado no nó da AST. Isso aproveitou a análise semântica já existente quando a AST foi criada. A desvantagem é que as mensagens usam a linha do analisador léxico, que no momento da redução pode já apontar para o token seguinte. Mover a análise para uma passagem separada sobre a árvore, usando a linha exata de cada nó, está planejado para a Sprint 5 ([#26](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/26)).

### Linha exata em cada nó

O parser usa `%locations` do Bison, e o scanner preenche a posição de cada token. Assim cada nó da AST guarda a linha do primeiro token da construção, e não a linha em que o parser estava ao reduzir a regra.

## Geração de código

### Código de três endereços como representação intermediária

O guia da disciplina sugere código de três endereços. Cada instrução tem no máximo um operador, e os valores intermediários ficam em temporários, o que torna explícita a ordem de avaliação que a AST só expressa pela forma da árvore. As instruções estão em [Código intermediário](projeto.md#código-intermediário-tac).

### Desvios condicionais por zero

Como em C, qualquer valor diferente de zero é verdadeiro. Por isso os desvios condicionais são `ifz` (desvia se for zero) e `ifnz` (desvia se não for), em vez de um desvio que depende de um tipo booleano que a linguagem não tem.

### `&&` e `||` em curto-circuito

Os operadores lógicos não viram uma instrução `and`/`or`: são gerados com desvios, de modo que o segundo operando só é avaliado quando o primeiro não decide o resultado. Isso preserva o comportamento de C quando o segundo operando tem efeito colateral, como uma chamada de função, e é verificado pelo teste `curto_circuito.c`.

### Chamadas com o número de argumentos explícito

Os argumentos são emitidos com `arg`, um por instrução, e a chamada informa quantos usar (`call f, 2`). Assim chamadas aninhadas, como `soma(2, fatorial(5))`, avaliam primeiro a chamada interna sem misturar os argumentos das duas.

### TypeScript gerado a partir da AST

O guia da disciplina prevê traduzir o código final a partir do código intermediário. O código de três endereços usa rótulos e desvios (`goto`), que TypeScript não tem, e traduzi-los exigiria simular o fluxo de controle com uma máquina de estados, gerando um código difícil de ler. Por isso o TypeScript é gerado a partir da AST anotada, que já tem as estruturas de controle na forma original ([PR #34](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/34)). O código intermediário é uma etapa à parte, exibida com `--tac` ([#28](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/28), [#29](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/29)).

### Valores-verdade como em C

Em C, `a < b` e `!a` valem `1` ou `0`, e qualquer número diferente de zero é verdadeiro numa condição. Em TypeScript, comparações produzem `true`/`false`. Para manter o comportamento de C, o gerador envolve comparações e operadores lógicos em `Number(...)` e as condições de `if`, `while` e `for` em `Boolean(...)`. Assim `int c = a < b;` guarda `1` ou `0`, como no programa original.

### Divisão inteira preservada

Em TypeScript `7 / 2` vale `3.5`. Quando os dois operandos são `int`, o gerador emite `Math.trunc(...)`, tanto em `/` quanto em `/=`, e o resultado é `3`, como em C. `Math.trunc` também trunca em direção a zero nos negativos (`-7 / 2` dá `-3`), igual a C.

### Expressões sempre entre parênteses

Cada expressão é gerada entre parênteses. O código fica mais carregado, mas a ordem de avaliação é exatamente a da AST, sem depender de a precedência do TypeScript coincidir com a do Mini-C.

### Chamada automática de `main`

Se o programa define `main()` sem parâmetros, o gerador acrescenta `main();` ao final do arquivo, para que o TypeScript execute como o programa em C. Se `main` tiver parâmetros, a chamada não é gerada, porque não há valores para passar.

## Testes

### Executar o código gerado nos testes

A partir da Sprint 4, os testes não verificam só se o compilador aceita um programa: geram o TypeScript, executam-no com Node e comparam a saída com um arquivo `.saida`. O TAC é comparado com um arquivo `.tac` de referência, porque não há um interpretador de TAC.

### Mensagem esperada dentro do próprio teste

Cada programa inválido declara na primeira linha o erro que deve produzir (`// ESPERADO: ...`). O `make test` confere o código de saída e a mensagem em `stderr`, e grava as evidências em `build/test-results`. Assim o teste e sua expectativa ficam no mesmo arquivo.
