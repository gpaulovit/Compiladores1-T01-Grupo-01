# Testes — Sprint 3, Frente D

Relacionados às issues [#16](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/16)
e [#20](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/20).
São 6 programas válidos e 16 inválidos. Cada inválido isola uma causa semântica;
os exemplos de string/char usam uma expressão isolada para não introduzir
também incompatibilidade de atribuição.

## Dependências e contrato das mensagens

A main consultada (`ba8ee5f`) só verifica o uso de `void` em variáveis e
parâmetros. A AST (#17, @gpaulovit) e a passagem semântica (#19,
@GabrielDiniz12) ainda não têm implementação publicada nas branches consultadas
em 21/10/2026. Não é possível declarar concluída a validação dessas frentes.

Cada arquivo começa com `// ESPERADO: ...`. Para válidos, o valor é `SUCESSO`.
Para inválidos, é um **trecho literal obrigatório em stderr**, com diferenciação
de maiúsculas/minúsculas. O executor permite prefixo, linha e contexto adicionais.
O código deve ser 0 nos válidos, sem stderr, e exatamente 1 nos inválidos.

As mensagens de `void` vêm do parser atual; a de redeclaração vem do PR #23.
As demais são uma **proposta de contrato para a Frente C**, não mensagens
implementadas ou acordadas. Confirmar o texto com o responsável pela #19 e
atualizar as expectativas junto à implementação, sem relaxar o teste para
aceitar qualquer erro. Os casos de conversão adotam a sugestão da #19:
promoção implícita `int` → `float` e rejeição de `float` → `int`.

## Casos válidos

| Arquivo em `validos/` | Cobertura |
| --- | --- |
| `01_declaracoes_multiplas.c` | Listas globais/locais, com/sem inicialização; chamada com vírgula dentro da inicialização. |
| `02_declaracoes_intercaladas.c` | Declarações após comandos e dentro de bloco interno; lista de variáveis. |
| `03_escopos_sombreamento.c` | Global/local/bloco interno e mesmo nome em blocos irmãos. |
| `04_recursao.c` | Referência à própria função e uso de parâmetro. |
| `05_promocao_int_float.c` | Promoção em inicialização, expressão, argumento e retorno. |
| `06_void_e_print.c` | Função void como comando; `return;`; texto/char dentro de print. |

## Casos inválidos

| Arquivo em `invalidos/` | Única causa intencional |
| --- | --- |
| `01_variavel_nao_declarada.c` | Uso de variável ausente. |
| `02_funcao_nao_declarada.c` | Chamada de função ausente. |
| `03_redeclaracao.c` | Duas variáveis com o mesmo nome no mesmo escopo. |
| `04_variavel_void.c` | Variável void. |
| `05_parametro_void.c` | Parâmetro void. |
| `06_numero_argumentos.c` | Argumento faltante. |
| `07_tipo_argumento.c` | Argumento float para parâmetro int. |
| `08_return_incompativel.c` | Retorno float em função int. |
| `09_return_valor_em_void.c` | Retorno com valor em função void. |
| `10_return_sem_valor.c` | Retorno vazio em função int. |
| `11_void_em_expressao.c` | Uso do resultado de função void como valor. |
| `12_string_fora_print.c` | Literal string fora de print. |
| `13_char_fora_print.c` | Literal char fora de print. |
| `14_atribuicao_float_int.c` | Atribuição de float para variável int. |
| `15_fora_escopo.c` | Uso de variável após encerrar seu bloco. |
| `16_redeclaracao_parametro.c` | Variável com mesmo nome do parâmetro no corpo da função. |

## Execução automatizada

Na raiz do repositório, em Linux ou terminal MSYS2 com GCC, Flex, Bison,
GNU Make, Bash e GNU coreutils (`timeout`, `grep`, `basename`, `mkdir`):

```bash
make test                              # Sprint 2 + Sprint 3, sem ocultar falhas
make test TEST_SUITES=testes/sprint2    # somente regressão da Sprint 2
make test TEST_SUITES=testes/sprint3    # somente Sprint 3
make test-runner                       # 9 verificações do próprio executor
```

O alvo também gera a gramática com conflitos shift/reduce e reduce/reduce
tratados como erro. Cada programa tem limite de 10 segundos. Para alterar:
`TEST_TIMEOUT=20 make test`. Timeout, falha de execução ou crash nunca contam
como uma rejeição semântica correta.

Os logs ficam em `build/test-results/`: stdout/stderr individuais,
`resultados.tsv`, revisão Git e diff dos arquivos rastreados ainda não
commitados. O diretório é ignorado pelo Git e reutilizado em cada execução;
o TSV da execução atual é a referência dos casos realmente executados.
Use `TEST_LOG_DIR=build/outro-resultado make test` para preservar uma execução.

O script retorna 0 somente quando todos os casos passam, 1 se algum reprova
e 2 para configuração inválida (por exemplo, conjunto vazio ou expectativa
ausente). O Make também termina com falha quando seu comando falha.

## Resultado observado e pendências

Em 21/10/2026, com os ajustes da #16 sobre `ba8ee5f`:

- Sprint 2: **15/15 aprovados**.
- Sprint 3: **8/22 aprovados** (6 válidos e os 2 inválidos de void).
- Total: **23 aprovados e 14 reprovados**. `make test` termina com falha.
- Os 14 inválidos restantes foram aceitos com código 0, pois as respectivas
  verificações semânticas ainda não existem na base. Não foram ignorados.

Aceitar os válidos não prova resolução de símbolos, promoção ou execução da
recursão; os casos negativos correspondentes e a implementação da #19 são
necessários. O relatório detalhado está em
[Sprint 3 — ajustes e testes](../../docs/resultados/sprint-3-testes.md).

- [x] Ajustar gramática e testar regressão (#16).
- [x] Criar corpus, expectativas e executor (#20).
- [x] Executar com Flex/Bison reais e registrar falhas.
- [ ] Confirmar mensagens e regras de conversão com a Frente C.
- [ ] Executar contra os futuros PRs de AST e semântica.
- [ ] Obter suíte completa aprovada após integração das dependências.
