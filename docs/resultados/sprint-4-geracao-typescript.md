# Sprint 4 — geração de código TypeScript

Registro de 05/11/2026, referente à implementação da geração de TypeScript a partir da AST.

## Objetivo

Implementar a geração de código TypeScript para o subconjunto Mini-C desenvolvido pelo compilador, integrando a funcionalidade ao executável por meio da opção `--ts`.

## Alterações

- Criação de `src/gerador_ts.h` e `src/gerador_ts.c`, responsáveis por percorrer a AST e emitir código TypeScript.
- Integração do gerador ao ponto de entrada em `src/main.c`, disponibilizando a opção `./build/compilador --ts`.
- Ajustes em `src/parser.y` para preservar a AST e evitar mensagens de análise sintática na saída TypeScript.
- Atualização do `Makefile` para compilar o novo módulo e executar os testes automatizados do gerador.
- Criação de casos de teste para funções, declarações, expressões, divisão inteira, operadores lógicos, estruturas de controle e função `main`.

## Funcionalidades implementadas

- Conversão dos tipos Mini-C `int` e `float` para `number`, e `void` para `void`.
- Geração de variáveis com `let` e declarações de funções com parâmetros e tipos de retorno.
- Tradução de `if/else`, `while`, `do-while` e `for`.
- Tradução de chamadas de função, retornos, atribuições e operadores.
- Conversão de `print(expr)` para `console.log(expr)`.
- Geração da chamada automática de `main()` quando a função existe sem parâmetros.
- Preservação da semântica de divisão inteira por meio de `Math.trunc`.
- Conversão dos resultados de expressões relacionais e lógicas para números, preservando a representação inteira utilizada pelo Mini-C.

## Decisão técnica: AST em vez de TAC

A geração de TypeScript percorre diretamente a AST anotada semanticamente.

Essa abordagem foi escolhida porque o TypeScript possui estruturas explícitas de controle, como `if`, `while`, `do-while` e `for`, mas não oferece uma instrução `goto` equivalente à utilizada pelo TAC.

Traduzir diretamente os desvios e rótulos do TAC exigiria reconstruir as estruturas de controle ou implementar uma máquina de estados, aumentando a complexidade da implementação.

A AST preserva essas estruturas e permite gerar construções equivalentes de forma mais direta.

Portanto, esta entrega implementa a geração de TypeScript a partir da AST, não uma etapa de geração de TAC.

## Verificação realizada

Ambiente: Linux, GCC, GNU Make, Flex e Bison.

| Verificação | Resultado |
| --- | --- |
| Compilação após `make clean` | Sucesso, sem warnings emitidos pelo GCC. |
| Bison com conflitos tratados como erro | Zero conflitos shift/reduce e reduce/reduce. |
| Testes específicos do gerador TypeScript | 8 aprovados; 0 reprovados. |
| Testes das Sprints 2 e 3 | 37 aprovados; 0 reprovados. |
| `git diff --check` | Nenhum problema de whitespace. |

A validação foi realizada por meio da saída produzida pelo compilador e das expectativas automatizadas dos testes.

Não foi realizada validação externa com `tsc` ou execução com Node.js, pois essas ferramentas não estavam instaladas no ambiente utilizado.

## Limitações e observações

- A chamada automática de `main()` é emitida somente quando a função não possui parâmetros.
- A geração está limitada às construções representadas e suportadas pela AST atual.
- A compilação e a execução independente do TypeScript gerado ainda precisam de validação com ferramentas próprias do ecossistema TypeScript.

