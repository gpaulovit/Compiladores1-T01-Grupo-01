# Problemas encontrados e soluções adotadas

Registro dos principais problemas do projeto, em ordem cronológica, com a solução adotada em cada caso.

## Equipe e processo

### Saída de um integrante

**Problema.** Henrique Mendes deixou a disciplina depois da Sprint 1, e a equipe passou de 5 para 4 integrantes.

**Solução.** O trabalho passou a ser dividido em quatro frentes por sprint, uma por integrante, com dependências explícitas entre elas. A contribuição de Henrique na Sprint 1 continua creditada (ver [Equipe](equipe.md)).

### Sobreposição entre frentes

**Problema.** Na Sprint 2, o PR da Frente B ([#13](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/13)) implementou também as regras de declarações e expressões, que eram da Frente A.

**Solução.** A Frente A revisou a gramática já mergeada, validou-a contra o escopo da linguagem e entregou apenas o que faltava: as ações semânticas e a checagem de `void` ([PR #15](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/15)).

### Sprint 3 sem início no período previsto

**Problema.** O período original da Sprint 3 (24/09 – 14/10) terminou sem que AST, tabela de símbolos ou análise semântica tivessem começado.

**Solução.** As Sprints 3 e 4 foram replanejadas em 13/10, com prazos internos e uma ordem explícita entre as frentes (ver [Ajustes no planejamento](cronograma.md#ajustes-no-planejamento)).

## Gramática e build

### Conflito reduce/reduce ao restringir `void`

**Problema.** Separar na gramática o tipo de função (com `void`) do tipo de variável (sem `void`) gerou dois conflitos reduce/reduce e quebrou programas válidos.

**Solução.** A restrição virou checagem semântica (ver [Decisões técnicas](decisoes-tecnicas.md#void-restringido-pela-semântica-não-pela-gramática)).

### Build quebrado por um commit de documentação

**Problema.** Uma edição de comentários no `parser.y` deixou um comentário de bloco sem o `*/` de fechamento, e o Bison deixou de compilar a gramática.

**Solução.** O comentário foi corrigido no commit seguinte. Desde a Sprint 3, o `make test` roda o `check-grammar`, que falha com qualquer erro ou conflito no `parser.y`.

### `make lex-debug` quebrado

**Problema.** Depois que a tabela de símbolos entrou, o `lex_debug` deixou de compilar: ele linkava o parser, mas não os módulos que as ações do parser chamam.

**Solução.** O `Makefile` passou a definir a lista `MODULOS`, usada por todo binário que linka o parser ([PR #27](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/27)).

## Análise semântica

### Vazamento de memória em chamadas de função

**Problema.** A análise de chamadas copiava o nome da função com `strdup`, e nenhuma função liberava essa cópia.

**Solução.** O parser passou a liberar o resultado semântico de cada expressão depois de usá-lo ([PR #27](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/27)).

### Linha das mensagens semânticas (em aberto)

**Problema.** As mensagens de erro semântico usam a linha atual do analisador léxico. No momento em que o parser reduz uma regra, essa linha pode já ser a do token seguinte, uma linha depois do trecho com erro.

**Situação.** A AST já guarda a linha exata de cada construção. A correção depende de mover a análise semântica para uma passagem sobre a árvore, planejada para a Sprint 5 ([#26](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/issues/26)).
