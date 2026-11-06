# Compilador Mini-C -> TypeScript
#
#   make            gera build/compilador (scanner + parser + semântico)
#   make lex-debug  gera build/lex_debug (imprime os tokens, sem parser)
#   make test       executa todos os testes automatizados
#   make test-symtab executa testes unitários da tabela de símbolos
#   make clean      remove build/

CC     = gcc
CFLAGS = -Wall -Ibuild -Isrc
B      = build
TEST_SUITES ?= testes/sprint2 testes/sprint3

all: $(B)/compilador

lex-debug: $(B)/lex_debug

# Falha se houver conflito, inclusive quando o parser ja estiver compilado.
check-grammar: | $(B)
	bison -Werror=conflicts-sr -Werror=conflicts-rr -Wcounterexamples -d -o $(B)/check-parser.c src/parser.y

test: all check-grammar test-gerador-ts test-sprint4
	bash testes/run-tests.sh ./$(B)/compilador $(TEST_SUITES)

test-runner:
	bash testes/test-runner.sh

test-gerador-ts: all
	bash testes/run-gerador-ts.sh ./$(B)/compilador

test-sprint4: all
	bash testes/sprint4/run.sh ./$(B)/compilador

# O Bison roda primeiro: gera y.tab.h, que o scanner inclui.
$(B)/y.tab.c $(B)/y.tab.h: src/parser.y | $(B)
	bison -d -o $(B)/y.tab.c src/parser.y

$(B)/lex.yy.c: src/scanner.l $(B)/y.tab.h | $(B)
	flex -o $@ src/scanner.l

# Módulos chamados pelas ações do parser; todo binário que linka y.tab.c precisa deles.
MODULOS = src/symtab.c src/semantico.c src/ast.c src/gerador_ts.c src/tac.c
HEADERS = src/symtab.h src/semantico.h src/ast.h src/gerador_ts.h src/tac.h

$(B)/compilador: $(B)/y.tab.c $(B)/lex.yy.c src/main.c $(MODULOS) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(B)/y.tab.c $(B)/lex.yy.c src/main.c $(MODULOS)

# lex_debug reaproveita y.tab.c para fornecer yylval/yyerror; o main é o do driver.
$(B)/lex_debug: $(B)/y.tab.c $(B)/lex.yy.c src/lex_debug.c $(MODULOS) $(HEADERS)
	$(CC) $(CFLAGS) -o $@ $(B)/y.tab.c $(B)/lex.yy.c src/lex_debug.c $(MODULOS)

$(B):
	mkdir -p $(B)

# Testes unitários da tabela de símbolos.
$(B)/teste_symtab: testes/teste_symtab.c src/symtab.c src/symtab.h | $(B)
	$(CC) $(CFLAGS) -o $@ testes/teste_symtab.c src/symtab.c

test-symtab: $(B)/teste_symtab
	./$(B)/teste_symtab

clean:
	rm -rf $(B)

.PHONY: all lex-debug test-symtab clean test check-grammar test-runner test-gerador-ts test-sprint4
