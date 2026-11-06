# Sobre o Projeto

## Escopo

Compilador que traduz um subconjunto da linguagem C (**Mini-C**) para **TypeScript**, cobrindo as etapas clássicas descritas no [Guia do Compilador](referencias/guia-compilador.md) do professor:

1. Definição da linguagem-fonte (sintaxe e semântica básica, gramática livre de contexto)
2. Análise léxica (Flex)
3. Análise sintática (Bison)
4. Análise semântica básica (tipos, escopo)
5. Geração de código intermediário
6. Otimização do código intermediário (opcional)
7. Geração de código final (TypeScript)

## Tecnologias fixadas

- **Flex** — geração do analisador léxico
- **Bison** — geração do analisador sintático
- **C** — linguagem de implementação, a mesma do código gerado pelo Flex e pelo Bison; compilado com GCC
- **GNU Make** — build e testes (`make`, `make test`)

## Linguagem-fonte: Mini-C

Subconjunto de C definido pela equipe na Sprint 1. Escopo fechado em 02/09/2026.

### Tipos e variáveis

- Tipos suportados: `int`, `float` e `void` (apenas como tipo de retorno de função).
- Declaração e atribuição de variáveis (ex.: `int x = 10;`).
- Declarações múltiplas (ex.: `int a, b = 2, c;`) e intercaladas com comandos.
- Variáveis locais (escopo de função/bloco) e globais.

#### Regras de tipos e análise semântica (Sprint 3)

- **Compatibilidade e promoção implícita:** 
  - `int` é promovido implicitamente para `float` quando necessário (em expressões aritméticas binárias, inicializações de variáveis, atribuições, argumentos de função e comandos `return`).
  - Atribuir um valor `float` a uma variável `int` é considerado **erro semântico** (`atribuicao incompativel para '<nome>'`).
  - Retornar um valor `float` em uma função de retorno `int` é considerado **erro semântico** (`retorno incompativel na funcao '<nome>'`).
- **Restrições do tipo `void`:**
  - `void` é proibido na declaração de variáveis e parâmetros.
  - Funções `void` não podem retornar expressões com valor (`funcao void '<nome>' nao pode retornar valor`).
  - Funções não-void (`int`, `float`) são obrigadas a retornar um valor (`funcao '<nome>' deve retornar valor`).
  - O resultado de uma função `void` não pode ser usado como operando/valor em expressões (`funcao void '<nome>' usada como valor`).
- **Escopos e Identificadores:**
  - Identificadores de variáveis e funções devem ser declarados antes do uso (`variavel/funcao '<nome>' nao declarada`).
  - Não é permitida a redeclaração de identificadores dentro do mesmo escopo (`variavel '<nome>' ja declarada neste escopo`). Parâmetros pertencem ao escopo do corpo da função e não podem ser redeclarados como variáveis locais no primeiro nível da função.
  - Variáveis declaradas em blocos internos `{ ... }` deixam de ser visíveis após o encerramento do bloco.
- **Chamadas de Função:**
  - O número de argumentos fornecidos deve corresponder exatamente à quantidade de parâmetros na assinatura da função (`numero de argumentos incompativel na chamada de '<nome>'`).
  - Os tipos dos argumentos devem ser compatíveis com os tipos esperados pelos parâmetros, respeitando as regras de promoção `int` → `float`.
- **Literais de Texto (`string` e `char`):**
  - Literais `string` e `char` são permitidos **exclusivamente** dentro da instrução de saída `print(...)`. Seu uso em qualquer outra expressão gera erro semântico (`string/char fora do print`).

### Operadores

- Aritméticos: `+`, `-`, `*`, `/`, `%` (resto).
- Incremento/decremento: `++`, `--`.
- Comparação: `==`, `!=`, `>`, `<`, `>=`, `<=`.
- Lógicos: `&&`, `||`.
- Unário: `-` (negação), `!` (negação lógica).
- Atribuição composta: `+=`, `-=`, `*=`, `/=`.

### Estruturas de controle e laços

- Condicionais: `if` / `else`.
- Laços: `while`, `for`, `do-while`.

### Funções

- Função principal `main()`.
- Funções customizadas com parâmetros e retorno via `return`.
- Recursão suportada.

### Entrada e saída

- `print(expr);` — imprime o valor de uma expressão (mapeado para `console.log` no TypeScript gerado). Sem E/S o programa não tem como demonstrar resultado, então este item é tratado como parte do núcleo, não como extra.

### Léxico auxiliar

- Comentários de linha (`//`) e de bloco (`/* */`), ignorados pelo analisador léxico.

##### Especificação de Símbolos Fixos (Operadores e Delimitadores)

Como parte do desenvolvimento da fase léxica (Frente 1: Operadores e Delimitadores), foi mapeada a especificação formal de todos os símbolos exatos suportados pela linguagem Mini-C. Estes símbolos literais exigem reconhecimento fixo no arquivo do Flex (`scanner.l`) e geram os seguintes tokens:

##### Tabela de Operadores
| Categoria | Lexema (Símbolo) | Token Gerado | Descrição | Exemplo Mini-C |
| :--- | :---: | :--- | :--- | :--- |
| **Aritméticos** | `+` | `PLUS` | Adição | `a + b` |
| | `-` | `MINUS` | Subtração / Negação Unária | `a - b` / `-a` |
| | `*` | `TIMES` | Multiplicação | `a * b` |
| | `/` | `DIVIDE` | Divisão | `a / b` |
| | `%` | `MOD` | Resto da divisão | `a % b` |
| **Incremento/Decremento** | `++` | `INC` | Incremento unitário | `i++` |
| | `--` | `DEC` | Decremento unitário | `i--` |
| **Atribuição** | `=` | `ASSIGN` | Atribuição simples | `x = 10` |
| **Atribuição Composta** | `+=` | `ADD_ASSIGN` | Atribuição com adição | `total += 5` |
| | `-=` | `SUB_ASSIGN` | Atribuição com subtração | `total -= 2` |
| | `*=` | `MUL_ASSIGN` | Atribuição com multiplicação| `total *= 2` |
| | `/=` | `DIV_ASSIGN` | Atribuição com divisão | `total /= 2` |
| **Relacionais** | `==` | `EQ` | Igualdade | `i == 2` |
| | `!=` | `NE` | Diferença | `i != 2` |
| | `<` | `LT` | Menor que | `i < 5` |
| | `<=` | `LE` | Menor ou igual a | `i <= 5` |
| | `>` | `GT` | Maior que | `i > 0` |
| | `>=` | `GE` | Maior ou igual a | `i >= 0` |
| **Lógicos** | `&&` | `AND` | Conjunção lógica (E) | `a && b` |
| | `||` | `OR` | Disjunção lógica (OU) | `a \|\| b` |
| | `!` | `NOT` | Negação lógica | `!valido` |

##### Tabela de Delimitadores e Pontuação
| Lexema | Token Gerado | Descrição | Finalidade no Mini-C |
| :---: | :--- | :--- | :--- |
| `{` | `LBRACE` | Abre chaves | Delimita início de escopo de funções ou blocos condicionais/laços. |
| `}` | `RBRACE` | Fecha chaves | Delimita fim de escopo de funções ou blocos condicionais/laços. |
| `(` | `LPAREN` | Abre parênteses | Envolve parâmetros de funções e condições de laços/condicionais. |
| `)` | `RPAREN` | Fecha parênteses | Envolve parâmetros de funções e condições de laços/condicionais. |
| `;` | `SEMI` | Ponto e vírgula | Encerra instruções e declarações obrigatórias. |
| `,` | `COMMA` | Vírgula | Separa argumentos em funções e listas de variáveis. |

##### Decisões de Precedência Léxica
Para assegurar que símbolos compostos não sejam erroneamente quebrados em múltiplos tokens simples (por exemplo, impedir que o operador de igualdade `==` seja interpretado como duas atribuições simples `=`), as regras no Flex foram ordenadas de forma que os padrões mais longos e específicos (como `==`, `!=`, `+=`, `++`) sejam declarados **antes** de operadores mais curtos ou genéricos (como `=`, `+`, `-`).

### Fora de escopo (decisão consciente)

Os itens abaixo foram avaliados e descartados para este projeto, para manter o escopo executável dentro do cronograma (Sprint 4 — geração de código até a P2, em 06/11/2026) e por não se encaixarem bem no alvo TypeScript:

- **Ponteiros** — TypeScript não tem modelo de endereço/memória manual; simular isso seria trabalho artificial sem valor didático proporcional.
- **Structs/records**.
- **Arrays multi-dimensionais**.
- **Strings como tipo completo** (concatenação, mutação, biblioteca padrão).

Arrays 1-D de tamanho fixo (`int`/`float`) e `string`/`char` como literal (só para uso em `print`) ficam como meta opcional, a avaliar durante a Sprint 3/4 conforme o progresso do parser/semântica — não bloqueiam o P1.

### Exemplo de código-fonte

```c
int soma(int a, int b) {
    return a + b;
}

int main() {
    int i = 0;
    int total = 0;

    for (i = 0; i < 5; i++) {
        if (i != 2) {
            total += soma(i, 1);
        }
    }

    print(total);
    return 0;
}
```

## Gramática formal

Gramática livre de contexto implementada em [`src/parser.y`](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/blob/main/src/parser.y) (Bison). Gerada com **zero conflitos** shift/reduce e reduce/reduce. Os símbolos em MAIÚSCULAS são os tokens produzidos pelo analisador léxico; `ε` indica produção vazia.

```bnf
programa           ::= declaracoes

declaracoes        ::= declaracao | declaracoes declaracao
declaracao         ::= var_declaracao | fun_declaracao

var_declaracao     ::= lista_variaveis ';'
lista_variaveis    ::= tipo IDENTIFIER
                     | tipo IDENTIFIER '=' expressao
                     | lista_variaveis ',' IDENTIFIER
                     | lista_variaveis ',' IDENTIFIER '=' expressao

tipo               ::= 'int' | 'float' | 'void'

fun_declaracao     ::= tipo IDENTIFIER '(' parametros_opt ')' bloco
parametros_opt     ::= ε | parametros
parametros         ::= parametro | parametros ',' parametro
parametro          ::= tipo IDENTIFIER

bloco              ::= '{' itens_bloco '}'
itens_bloco        ::= ε | itens_bloco item_bloco
item_bloco         ::= var_declaracao | comando

comando            ::= expressao_comando | bloco
                     | comando_if    | comando_while | comando_for
                     | comando_do_while | comando_return | comando_print

expressao_comando  ::= expressao ';' | ';'
comando_if         ::= 'if' '(' expressao ')' comando
                     | 'if' '(' expressao ')' comando 'else' comando
comando_while      ::= 'while' '(' expressao ')' comando
comando_do_while   ::= 'do' comando 'while' '(' expressao ')' ';'
comando_for        ::= 'for' '(' expressao_opt ';' expressao_opt ';' expressao_opt ')' comando
comando_return     ::= 'return' expressao_opt ';'
comando_print      ::= 'print' '(' expressao ')' ';'
expressao_opt      ::= ε | expressao

expressao          ::= IDENTIFIER ('=' | '+=' | '-=' | '*=' | '/=') expressao
                     | expressao ('||' | '&&') expressao
                     | expressao ('==' | '!=' | '<' | '>' | '<=' | '>=') expressao
                     | expressao ('+' | '-' | '*' | '/' | '%') expressao
                     | '!' expressao | '-' expressao
                     | '++' IDENTIFIER | IDENTIFIER '++'
                     | '--' IDENTIFIER | IDENTIFIER '--'
                     | '(' expressao ')'
                     | IDENTIFIER '(' argumentos_opt ')'
                     | IDENTIFIER | INT_LITERAL | FLOAT_LITERAL
                     | CHAR_LITERAL | STRING_LITERAL

argumentos_opt     ::= ε | argumentos
argumentos         ::= expressao | argumentos ',' expressao
```

### Precedência e associatividade

As regras de `expressao` são propositalmente ambíguas; a ambiguidade é resolvida por declarações de precedência do Bison, listadas **da menor para a maior** (o que vem depois liga mais forte):

| Nível | Associatividade | Operadores |
|---|---|---|
| 1 (menor) | à direita | `=` `+=` `-=` `*=` `/=` |
| 2 | à esquerda | `\|\|` |
| 3 | à esquerda | `&&` |
| 4 | à esquerda | `==` `!=` |
| 5 | à esquerda | `<` `>` `<=` `>=` |
| 6 | à esquerda | `+` `-` |
| 7 | à esquerda | `*` `/` `%` |
| 8 (maior) | à direita | `!`, `-` unário |

O conflito do *dangling else* (`if (a) if (b) x; else y;`) é resolvido por `%prec LOWER_THAN_ELSE`, que liga o `else` ao `if` mais próximo.

### Restrições não expressas na gramática

- **`void` só como tipo de retorno.** A gramática usa um único não-terminal `tipo`, então `void x;` é sintaticamente aceito e rejeitado por **checagem semântica**. Separar os tipos na gramática criaria conflito reduce/reduce, porque ao ver `int nome` o parser LALR(1) ainda não sabe se vem uma variável ou uma função.
- **Itens de bloco em ordem livre:** declarações e comandos podem ser intercalados; sua ordem no código-fonte é preservada.
- **Listas de variáveis:** `int a, b = 2, c;` compartilha o tipo, com inicialização individual opcional. A vírgula não é um operador de expressão.

## Linguagem-alvo: TypeScript

> **Situação:** em implementação na Sprint 4 ([#30](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/30)). O mapeamento abaixo é o planejado.

| Mini-C | TypeScript |
|---|---|
| `int`, `float` | `number` |
| `void` (retorno de função) | `void` |
| `int x = 10;` | `let x: number = 10;` |
| `int soma(int a, int b) { ... }` | `function soma(a: number, b: number): number { ... }` |
| `if`/`else`, `while`, `do-while`, `for` | as mesmas construções |
| `print(expr);` | `console.log(expr);` |
| `a / b` entre `int` | `Math.trunc(a / b)`, porque em TypeScript a divisão seria fracionária |
| função `main()` | chamada `main();` ao final do arquivo |

O TypeScript é gerado a partir da AST anotada com tipos, e não do código intermediário. A justificativa está em [Decisões técnicas](decisoes-tecnicas.md).

## Arquitetura do compilador

```text
programa.c ──► scanner.l (Flex) ──tokens──► parser.y (Bison, LALR(1))
                                               │  ações de cada regra
                                               ├──► semantico.c ◄──► symtab.c
                                               │      (tipos, escopos, erros)
                                               └──► ast.c  (nó com o tipo já anotado)
                                                        │
                                       main.c ◄─────────┘
                                         ├── --ast  imprime a árvore
                                         ├── --tac  código de três endereços   (Sprint 4, #28/#29)
                                         └── --ts   TypeScript                  (Sprint 4, #30)
```

| Módulo | Responsabilidade |
|---|---|
| `src/scanner.l` | Analisador léxico: 42 tokens, descarte de comentários, erros léxicos com número da linha. Preenche a posição de cada token para o `%locations` do parser. |
| `src/parser.y` | Gramática do Mini-C, gerada com zero conflitos. Cada regra chama a análise semântica e constrói o nó da AST. |
| `src/symtab.c` | Tabela de símbolos: pilha de escopos, variáveis, parâmetros e funções (com tipo de retorno e parâmetros). |
| `src/semantico.c` | Regras semânticas: declaração prévia, redeclaração, tipos, chamadas, `return`, `void` e literais de texto fora do `print`. |
| `src/ast.c` | Árvore sintática abstrata: criação, impressão indentada (`--ast`) e liberação. |
| `src/main.c` | Ponto de entrada: lê da entrada padrão e sai com 1 se houver erro léxico, sintático ou semântico. |
| `src/lex_debug.c` | Ferramenta de depuração: imprime os tokens sem passar pelo parser (`make lex-debug`). |

Hoje a análise semântica roda dentro das ações do parser, durante a análise sintática. Movê-la para uma passagem separada sobre a AST está planejado para a Sprint 5 ([#26](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/26)).

### Exemplo: `--ast`

```text
$ echo 'int main() { int r = 1 + 2 * 3; print(r); return 0; }' | ./build/compilador --ast

Analise sintatica concluida com sucesso.
Programa @1
  Funcao main (int) @1
    -
    Bloco @1
      DeclVar r (int) @1
        Binario + (int) @1
          LitInt 1 (int) @1
          Binario * (int) @1
            LitInt 2 (int) @1
            LitInt 3 (int) @1
      Print @1
        Id r (int) @1
      Return @1
        LitInt 0 (int) @1
```

A precedência aparece na forma da árvore: o `*` fica abaixo do `+`, então é avaliado primeiro.