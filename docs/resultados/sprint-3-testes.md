# Sprint 3 — ajustes de gramática e testes

Registro de 21/10/2026, relativo às issues #16 e #20. Base: `ba8ee5f`.
Este relatório descreve esta entrega; não declara concluída toda a Sprint 3.

## Alterações

- `lista_variaveis` aceita `int a, b = 2, c;` e propaga o tipo comum por
  valor semântico, sem variável global auxiliar. Cada declarador é processado
  na ordem da fonte e mantém sua inicialização opcional.
- `itens_bloco` alterna declarações e comandos em qualquer ordem.
- README principal e página inicial da documentação exibem os quatro
  integrantes atuais. A saída de Henrique é registrada preservando seu crédito
  na Sprint 1. O histórico de resultados da Sprint 1 foi mantido.
- A BNF em `docs/projeto.md` acompanha as regras novas. As limitações no
  relatório da Sprint 2 descrevem aquela versão histórica.
- `make test` verifica retorno e mensagens, grava evidências e falha diante de
  regressões, timeout ou erro inesperado. O corpus total contém 37 arquivos.

## Verificação realizada

Ambiente: Windows com MSYS2; ferramentas Flex 2.6.4, Bison 3.8.2,
GCC 15.2.0 e GNU Make 4.4.1.

| Verificação | Resultado |
| --- | --- |
| Compilação pelo Makefile | Sucesso, sem warnings emitidos pelo GCC. |
| Bison com conflitos tratados como erro | Zero conflitos shift/reduce e reduce/reduce. |
| Sprint 2 | 11 válidos aceitos, 4 inválidos rejeitados com diagnóstico esperado. |
| Sprint 3 válidos | 6 aceitos, incluindo listas e declarações intercaladas. |
| Sprint 3 inválidos | 2 de void rejeitados; 14 ainda aceitos indevidamente. |
| Executor | 9 autotestes: sucesso, mensagem incorreta, código incorreto, mensagem só em stdout, stderr inesperado, crash, timeout, cabeçalho ausente e conjunto ausente. |

As 14 falhas abrangem variável/função não declarada, redeclaração, argumentos,
retornos, void como valor, texto fora de print, conversão float → int e
visibilidade de escopo. Cada arquivo e expectativa está no
[guia do corpus](../../testes/sprint3/README.md).

## Integração com as outras frentes

- **#17 (AST, @gpaulovit):** ainda sem branch/PR publicado na consulta.
  Construir sobre `lista_variaveis` e `itens_bloco`, preservando declarações
  individuais, inicializadores e ordem dos itens na AST. O tipo propagado
  atualmente pela lista deverá ser adaptado ao valor semântico da AST.
- **#19 (semântica, @GabrielDiniz12):** ainda sem branch/PR publicado na consulta.
  Os textos novos em `ESPERADO` são uma proposta para revisão pelo responsável.
  As regras int → float / float → int seguem a sugestão da issue, pendente de
  confirmação. Não foram implementadas por esta entrega.
- **PR #23 (tabela de símbolos):** já altera `parser.y` e `Makefile`, mas usa
  as listas antigas de declarações/comandos. Ao integrar, aplicar o registro
  de símbolos a cada declarador e usar `itens_bloco` tanto no corpo da função
  quanto nos blocos internos, preservando abertura/fechamento dos escopos.
  Não sobrescrever suas ações de registro com as ações da base deste PR.
  Os casos de redeclaração usam o trecho de diagnóstico publicado nesse PR.
- **PR #21:** trata do planejamento/cronograma e não é requisito para compilar
  esta entrega. Suas alterações não foram incorporadas aqui.

A #16 está implementada e validada. A #20 permanece parcialmente concluída:
corpus e automação prontos, com validação final e confirmação das mensagens
dependentes das outras frentes. Não há evidência para marcar a #20 concluída.
