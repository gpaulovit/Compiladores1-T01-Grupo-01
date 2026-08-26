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

`TODO: descrever aqui a sintaxe e semântica básica do Mini-C definidas pela equipe (tokens, estruturas suportadas, exemplos de código-fonte).`

## Gramática formal

`TODO: incluir/linkar a gramática livre de contexto (arquivo .y do Bison) conforme evoluir na Sprint 1 e Sprint 3.`

## Linguagem-alvo: TypeScript

`TODO: descrever o subconjunto/estilo de TypeScript gerado como saída (ex.: mapeamento de tipos, estruturas de controle, funções).`

## Arquitetura do compilador

`TODO: diagrama/descrição das etapas internas (lexer → parser → AST → tabela de símbolos → análise semântica → código intermediário → código final) conforme forem implementadas.`