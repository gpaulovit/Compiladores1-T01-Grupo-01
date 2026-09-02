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
- Linguagem de implementação: `TODO: confirmar (C/C++, conforme saída padrão do Flex/Bison)`

## Linguagem-fonte: Mini-C

Subconjunto de C definido pela equipe na Sprint 1. Escopo fechado em 02/09/2026.

### Tipos e variáveis

- Tipos suportados: `int`, `float` e `void` (apenas como tipo de retorno de função).
- Declaração e atribuição de variáveis (ex.: `int x = 10;`).
- Variáveis locais (escopo de função/bloco) e globais.

### Operadores

- Aritméticos: `+`, `-`, `*`, `/`.
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

### Fora de escopo (decisão consciente)

Os itens abaixo foram avaliados e descartados para este projeto, para manter o escopo executável dentro do cronograma (Sprint 4 — codegen intermediário até 04/11/2026) e por não se encaixarem bem no alvo TypeScript:

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

`TODO: incluir/linkar a gramática livre de contexto (arquivo .y do Bison) conforme evoluir na Sprint 1 e Sprint 3.`

## Linguagem-alvo: TypeScript

`TODO: descrever o subconjunto/estilo de TypeScript gerado como saída (ex.: mapeamento de tipos, estruturas de controle, funções).`

## Arquitetura do compilador

`TODO: diagrama/descrição das etapas internas (lexer → parser → AST → tabela de símbolos → análise semântica → código intermediário → código final) conforme forem implementadas.`