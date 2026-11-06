# Resultados — Sprint 4

**Período:** 26/10/2026 – 06/11/2026
**Sprint:** [Sprint 4 — Código Intermediário, TypeScript & P2](../sprints/sprint-4.md)

## Resumo

Com a Sprint 4, o compilador passou a funcionar de ponta a ponta: um programa Mini-C válido é traduzido para **código de três endereços** (`--tac`) e para **TypeScript** (`--ts`), e o TypeScript gerado executa com o mesmo resultado do programa original. Os testes deixaram de verificar apenas se o compilador aceita ou rejeita um programa: agora executam o código gerado e comparam a saída.

| Frente | Issue | Responsável | PR | Entrou no `main` |
|---|---|---|---|---|
| C — Geração de TypeScript | [#30](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/30) | Gustavo Bonifácio | [#34](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/34) | 06/11 |
| B — TAC: interface, controle de fluxo e funções | [#29](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/29) | Gabriel Diniz | [#35](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/35) | 06/11 |
| A — TAC: expressões e atribuições | [#28](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/28) | Gabriel Di Angellis | [#35](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/35) | 06/11 |
| D — Testes de geração de código | [#31](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/31) | Gabriel Di Angellis | [#35](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/35) | 06/11 |

O PR #35 integrou o trabalho das Frentes A, B e D preservando os commits originais de cada autor.

## Entregas

### Código intermediário — `src/intermediario/tac.c`

Percorre a AST anotada e gera código de três endereços: cada instrução tem no máximo um operador, e os resultados intermediários ficam em temporários (`t0`, `t1`...). As instruções e um exemplo completo estão em [Sobre o Projeto → Código intermediário](../projeto.md#código-intermediário-tac).

- Expressões aritméticas, relacionais e unárias, atribuições simples e compostas, pré e pós-incremento e decremento.
- `if`/`else`, `while`, `do-while` e `for` com rótulos e desvios condicionais.
- `&&` e `||` com avaliação em curto-circuito, como em C.
- Funções, parâmetros, chamadas com argumentos, recursão, `return` e `print`.
- Programas com erro léxico, sintático ou semântico não geram TAC.

### Geração de TypeScript — `src/gerador/gerador_ts.c`

Percorre a AST anotada e escreve TypeScript executável, preservando a semântica de C na divisão inteira e nos valores-verdade. O mapeamento está em [Sobre o Projeto → Linguagem-alvo](../projeto.md#linguagem-alvo-typescript), e o detalhamento da entrega em [Resultados — Geração TypeScript](sprint-4-geracao-typescript.md).

### Testes de geração de código — `testes/sprint4/`

Quatro programas, cada um com a saída esperada (`.saida`) e o TAC esperado (`.tac`):

| Programa | Cobertura |
|---|---|
| `expressoes.c` | Precedência, parênteses, divisão inteira, unários, atribuições compostas, pré e pós-incremento |
| `controle.c` | `for`, `while`, `do-while`, `if`/`else` |
| `funcoes.c` | Parâmetros, chamadas aninhadas, recursão, `return`, `print` |
| `curto_circuito.c` | `&&` e `\|\|` com efeito colateral no segundo operando |

Para cada programa, o executor gera o TypeScript, executa-o com Node e compara a saída completa com o `.saida`, e gera o TAC e compara com o `.tac`. Também confere que programas com erro léxico, sintático e semântico são rejeitados sem emitir TAC.

## Exemplo

`testes/sprint4/funcoes.c`:

```c
int soma(int a, int b) {
    return a + b;
}

int fatorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * fatorial(n - 1);
}

void mostrar(int valor) {
    print(valor);
    return;
}

int main() {
    mostrar(soma(2, fatorial(5)));
    return 0;
}
```

`./build/compilador --tac`:

```text
func soma:
  param a
  param b
  t0 = a
  t1 = b
  t2 = t0 + t1
  return t2
endfunc soma
func fatorial:
  param n
  t3 = n
  t4 = t3 <= 1
  ifz t4 goto L0
  return 1
L0:
  t5 = n
  t6 = n
  t7 = t6 - 1
  arg t7
  t8 = call fatorial, 1
  t9 = t5 * t8
  return t9
endfunc fatorial
func mostrar:
  param valor
  t10 = valor
  print t10
  return
endfunc mostrar
func main:
  arg 5
  t11 = call fatorial, 1
  arg 2
  arg t11
  t12 = call soma, 2
  arg t12
  call mostrar, 1
  return 0
endfunc main
```

O TypeScript gerado para o mesmo programa imprime `122` (`2 + fatorial(5)`), igual ao `funcoes.saida`.

## Validação

Executada no `main` em `2fb8284`, em 06/11/2026.

| Verificação | Resultado |
|---|---|
| Sprints 2 e 3 (`testes/run-tests.sh`) | 37 aprovados, 0 reprovados |
| Gerador TypeScript (`make test-gerador-ts`) | 8 aprovados, 0 reprovados |
| Sprint 4: TypeScript executado e saída comparada | 4 de 4 programas |
| Sprint 4: TAC comparado com o esperado | 4 de 4 programas |
| Sprint 4: programas inválidos rejeitados sem TAC | 3 de 3 |
| `make test-symtab` / `make test-runner` | Aprovados |
| `make check-grammar` | Zero conflitos |

Além da suíte, os 24 programas válidos das Sprints 2 e 3 e dos testes do gerador foram compilados com `--ts` e executados com Node, todos sem erro.

## Limitações conhecidas

- **O TAC é verificado como texto**, comparado com a referência `.tac`. Não existe um interpretador de TAC, então a execução é verificada pelo TypeScript.
- **A análise semântica ainda roda dentro das ações do parser** ([#26](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/26)), com a consequência descrita em [Problemas e soluções](../problemas-e-solucoes.md#linha-das-mensagens-semânticas-em-aberto).

## Próximos passos (Sprint 5)

- Mover a análise semântica para uma passagem sobre a AST ([#26](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/26)).
- Otimizações no código intermediário, se houver tempo.
- Preparação para as entrevistas finais.
