# Testes de geração de código — Sprint 4

`make test-sprint4` roda a suíte desta sprint. `make test` inclui também as
suítes das Sprints 2 e 3 e as verificações textuais do gerador TypeScript.
Use Bash, Git, GNU coreutils e Node.js 24 ou superior, que executa
os arquivos TypeScript gerados removendo as anotações de tipo.

| Programa | Cobertura |
| --- | --- |
| `expressoes.c` | Precedência, parênteses, divisão inteira, unários, atribuições compostas e valores de pré/pós-incremento e decremento. |
| `controle.c` | Laços for, while, do-while e desvios if/else. |
| `funcoes.c` | Parâmetros, chamadas aninhadas, recursão, return e print. |
| `curto_circuito.c` | Operadores lógicos com efeitos colaterais e preservação do valor de um operando antes de incrementar a variável. |

Cada `.saida` contém a saída prevista do programa. O executor gera o
TypeScript, roda o Node e compara a saída completa. Para `--tac`, compara
as instruções com as referências `.tac`, incluindo temporários, rótulos e
desvios. Essa comparação verifica o texto do TAC. A execução é feita sobre
o TypeScript. Entradas com erro léxico, sintático ou semântico também
verificam se o compilador rejeita o programa sem emitir TAC.

Os artefatos de cada execução ficam em `build/test-sprint4.*`.

Material de apoio: [Aula 08 — Geração de código intermediário](https://slides.cedis.tec.br/comp1-aula08-codigo-intermediario/)
e [Aula 10 — Geração de código final](https://slides.cedis.tec.br/comp1-aula10-geracao-codigo-final/).
