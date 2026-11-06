# Compiladores1-T01-Grupo-01

## Compilador: Mini-C para TypeScript

Projeto da equipe 01 da disciplina de Compiladores 1. O objetivo é traduzir um subconjunto da linguagem C (mini-C) para TypeScript usando as etapas clássicas de um compilador: análise léxica, análise sintática, AST, validações semânticas básicas e geração de código final.

## Compilação e testes

Em terminal Linux/MSYS2 com GCC, Flex, Bison, GNU Make, Bash e coreutils:

```bash
make                                   # gera build/compilador
./build/compilador < programa.c        # análise léxica, sintática e semântica
./build/compilador --ast < programa.c  # imprime a árvore sintática abstrata

make test          # compila, verifica a gramática e roda todos os programas de teste
make test-symtab   # testes unitários da tabela de símbolos
make test-runner   # testes do próprio executor de testes
make lex-debug     # imprime os tokens, sem passar pelo parser
```

O compilador sai com código 1 se encontrar erro léxico, sintático ou semântico.
Os casos de teste estão em [testes/sprint2](testes/sprint2/README.md) e
[testes/sprint3](testes/sprint3/README.md). A documentação completa do projeto
está na [GitPage](https://gpaulovit.github.io/Compiladores1-T01-Grupo-01/).

## Equipe atual (4 integrantes)

<table align="center">
  <tr>
    <td align="center">
      <a href="https://github.com/diangellis">
        <img src="https://github.com/diangellis.png" width="120px;" alt="Foto de Gabriel Di Angellis"/><br>
        <sub><b>Gabriel Di Angellis</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/GabrielDiniz12">
        <img src="https://github.com/GabrielDiniz12.png" width="120px;" alt="Foto de Gabriel Diniz"/><br>
        <sub><b>Gabriel Diniz</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/Gustavo2703">
        <img src="https://github.com/Gustavo2703.png" width="120px;" alt="Foto de Gustavo Bonifácio"/><br>
        <sub><b>Gustavo Bonifácio</b></sub>
      </a>
    </td>
    <td align="center">
      <a href="https://github.com/gpaulovit">
        <img src="https://github.com/gpaulovit.png" width="120px;" alt="Foto de Paulo Vitor Gomes"/><br>
        <sub><b>Paulo Vitor Gomes</b></sub>
      </a>
    </td>
  </tr>
</table>

Henrique Mendes deixou a equipe após a Sprint 1. Sua contribuição ao reconhecimento de palavras reservadas e identificadores permanece creditada no [PR #6](https://github.com/gpaulovit/Compiladores1-T01-Grupo-01/pull/6) e no histórico da Sprint 1.
