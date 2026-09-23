# Exemplos Mini-C — Sprint 2, Frente C

Conjunto de entradas para a [issue #10](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/10).
Esta entrega cobre a criação dos exemplos e a definição dos resultados esperados.
A execução da integração e o relatório dos resultados ficam com o responsável
pela segunda parte da Frente C. Cada arquivo deve ser analisado separadamente.

## Referências e limites

- Escopo da linguagem: [documentação do projeto](../../docs/projeto.md).
- Base desta entrega: `main` no commit `d3b47e5`.
- Regras consultadas: `src/parser.y` da branch `feat/parser-regras`, commit
  `72b2686`, proposta no [PR #13](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/13).

**Atenção:** o parser da base `d3b47e5` é um esqueleto que aceita qualquer
sequência de tokens conhecidos. Ele pode aceitar os três exemplos com erro
sintático. Isso é uma limitação do parser provisório, e não um resultado
correto para esses exemplos. A validação sintática depende das regras reais.

Os programas usam `int`, `float` e `void` apenas como retorno de função.
As declarações locais aparecem antes dos comandos de cada bloco, e o `for`
usa uma variável já declarada, conforme as regras consultadas. Não são usados
arrays, ponteiros, structs ou recursos opcionais de texto.

Nesta etapa, aceitar uma expressão não comprova sua precedência, o valor
calculado ou o vínculo correto de um `else`. Isso exige inspeção da árvore ou
outra validação posterior. Também não se verifica execução da recursão,
análise semântica ou geração de TypeScript. `print` é uma construção Mini-C;
a saída numérica do programa não é a saída esperada do parser.

## Casos válidos

Todos devem terminar com código **0**, sem diagnóstico léxico ou sintático,
quando a gramática correspondente estiver implementada.

| Arquivo em `validos/` | O que exercita |
| --- | --- |
| `01_declaracoes.c` | Variáveis globais e locais; `int` e `float`; declaração com e sem inicialização; atribuição. |
| `02_expressoes_aritmeticas.c` | `+`, `-`, `*`, `/`, `%`, negação unária, parênteses e literais inteiros/decimais. |
| `03_expressoes_logicas_relacionais.c` | `==`, `!=`, `<`, `<=`, `>`, `>=`, `&&`, `||` e `!`. |
| `04_atribuicoes_incrementos.c` | `+=`, `-=`, `*=`, `/=`; incremento/decremento prefixado e pós-fixado. |
| `05_condicionais.c` | `if`, `if/else` e condicionais aninhadas sem chaves (caso de dangling else). |
| `06_while.c` | Laço `while` com bloco e incremento. |
| `07_for.c` | Laço `for` com inicialização, condição e incremento. |
| `08_do_while.c` | Laço `do-while` com o `;` final obrigatório. |
| `09_funcoes.c` | Parâmetros e argumentos separados por vírgula; chamadas com/sem argumentos; retornos `int`, `float` e `void`; `return;` e `print`. |
| `10_recursao.c` | Definição e chamada recursiva de função. |
| `11_comentarios_blocos.c` | Comentários de linha/bloco, símbolos inválidos dentro de comentário e declaração em bloco interno. |

## Casos inválidos

Cada arquivo isola um erro. Todos devem terminar com código **1**, conforme
o contrato de `src/main.c`, e emitir o diagnóstico indicado em `stderr`.
A linha do diagnóstico pode ser posterior à origem do erro, dependendo do
token em que o parser detecta a falha.

| Arquivo em `invalidos/` | Erro introduzido | Diagnóstico esperado |
| --- | --- | --- |
| `01_sem_ponto_virgula.c` | Falta `;` após a declaração da linha 2. | Erro sintático. |
| `02_expressao_incompleta.c` | Falta operando depois de `+` na linha 3. | Erro sintático. |
| `03_parentese_ausente.c` | Falta `)` antes de `{` na linha 2. | Erro sintático. |
| `04_caractere_invalido.c` | `@` fora de comentário na linha 3. | Erro léxico de caractere desconhecido. |

No último caso, o scanner pode descartar `@` e o parser aceitar o restante.
Mesmo assim, `lex_errors` deve fazer o executável retornar **1**. Uma mensagem
de sucesso do parser, sozinha, não comprova aprovação.

## Como executar e registrar (segunda parte da Frente C)

Em um ambiente com Bash, Make, GCC, Flex e Bison, a partir da raiz do repositório:

```bash
make
./build/compilador < testes/sprint2/validos/01_declaracoes.c
echo $?
```

Para coletar os resultados individuais, preservando stdout, stderr e código de
saída, execute após uma compilação bem-sucedida:

```bash
mkdir -p build/resultados-sprint2
git rev-parse HEAD > build/resultados-sprint2/revisao.txt
printf 'arquivo\tesperado\tobtido\n' > build/resultados-sprint2/resultados.tsv
for grupo in validos invalidos; do
    esperado=0
    if [ "$grupo" = invalidos ]; then esperado=1; fi
    for arquivo in testes/sprint2/"$grupo"/*.c; do
        nome=$(basename "$arquivo" .c)
        obtido=0
        ./build/compilador < "$arquivo" \
            > "build/resultados-sprint2/${grupo}_${nome}.stdout" \
            2> "build/resultados-sprint2/${grupo}_${nome}.stderr" || obtido=$?
        printf '%s\t%s\t%s\n' "$arquivo" "$esperado" "$obtido" \
            >> build/resultados-sprint2/resultados.tsv
    done
done
```

O laço coleta evidências; não substitui a comparação dos códigos e a revisão
dos diagnósticos. Os arquivos em `build/` são ignorados pelo Git. O responsável
pela execução deve consolidar as evidências no relatório da Frente C, indicando
o commit testado e os problemas a reportar às Frentes A/B.

## Estado desta entrega

- [x] Exemplos revisados contra o escopo documentado e as produções do PR #13.
- [x] Resultados esperados definidos por caso.
- [ ] Executar os 15 arquivos com lexer + parser real.
- [ ] Comparar códigos de saída e diagnósticos com esta especificação.
- [ ] Registrar resultados e reportar divergências às Frentes A/B.

Os casos ainda não foram executados com Flex/Bison nesta entrega: essas
ferramentas não estavam disponíveis no ambiente de preparação. As tabelas
acima são expectativas de teste, não um relatório de testes aprovados.
