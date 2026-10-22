# Compilador Mini-C -> TypeScript
#
#   make            gera build/compilador (scanner + parser)
#   make lex-debug  gera build/lex_debug (imprime os tokens, sem parser)
#   make clean      remove build/

CC     = gcc
CFLAGS = -Wall -Ibuild -Isrc
B      = build

all: $(B)/compilador

lex-debug: $(B)/lex_debug

# O Bison roda primeiro: gera y.tab.h, que o scanner inclui.
$(B)/y.tab.c $(B)/y.tab.h: src/parser.y | $(B)
	bison -d -o $(B)/y.tab.c src/parser.y

$(B)/lex.yy.c: src/scanner.l $(B)/y.tab.h | $(B)
	flex -o $@ src/scanner.l

$(B)/compilador: $(B)/y.tab.c $(B)/lex.yy.c src/main.c src/symtab.c src/symtab.h
	$(CC) $(CFLAGS) -o $@ $(B)/y.tab.c $(B)/lex.yy.c src/main.c src/symtab.c

# lex_debug reaproveita y.tab.c para fornecer yylval/yyerror; o main é o do driver.
$(B)/lex_debug: $(B)/y.tab.c $(B)/lex.yy.c src/lex_debug.c
	$(CC) $(CFLAGS) -o $@ $^

$(B):
	mkdir -p $(B)

# Testes unitários da tabela de símbolos.
$(B)/teste_symtab: testes/teste_symtab.c src/symtab.c src/symtab.h | $(B)
	$(CC) $(CFLAGS) -o $@ testes/teste_symtab.c src/symtab.c

test-symtab: $(B)/teste_symtab
	./$(B)/teste_symtab

clean:
	rm -rf $(B)

.PHONY: all lex-debug test-symtab clean
