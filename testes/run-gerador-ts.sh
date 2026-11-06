#!/usr/bin/env bash
set -u

compiler=${1:-./build/compilador}

if [[ ! -x "$compiler" ]]; then
    echo "Compilador nao encontrado: $compiler" >&2
    exit 2
fi

tmp_dir=$(mktemp -d)
trap 'rm -rf "$tmp_dir"' EXIT

passed=0
failed=0

run_case() {
    local file=$1
    shift
    local output="$tmp_dir/saida"
    local errors="$tmp_dir/erros"
    local reason=""

    if ! "$compiler" --ts < "$file" > "$output" 2> "$errors"; then
        reason="compilador retornou erro"
    elif [[ -s "$errors" ]]; then
        reason="diagnostico inesperado em stderr"
    else
        for expected in "$@"; do
            if ! grep -Fq -- "$expected" "$output"; then
                reason="trecho ausente: $expected"
                break
            fi
        done
    fi

    if [[ -z "$reason" ]]; then
        ((passed+=1))
        echo "OK    $file"
    else
        ((failed+=1))
        echo "FALHA $file ($reason)"
        echo "--- stdout ---"
        cat "$output"
        echo "--- stderr ---"
        cat "$errors"
    fi
}

run_case testes/teste_gerador.c \
    "function somar(a: number, b: number): number" \
    "let resultado: number = somar(x, y);" \
    "console.log(resultado);" \
    "main();"

run_case testes/teste_gerador_controle.c \
    "Math.trunc((x) / (y))" \
    "Math.trunc(x / 2)" \
    "while (Boolean(Number((i < 2))))" \
    "do {" \
    "for ((i = 0); Boolean(Number((i < 3))); (i++))"

run_case testes/teste_gerador_for.c \
    "for ((i = 0); Boolean(Number((i < 3))); (i++))" \
    "for (; ; )"

run_case testes/teste_gerador_funcoes.c \
    "function calcular_media(a: number, b: number): number" \
    "function mostrar(valor: number): void" \
    "Math.trunc(((-7)) / (2))"

run_case testes/teste_gerador_logicos.c \
    "Number((a > b))" \
    "Boolean(a) && Boolean(b)" \
    "Number(!(a))"

run_case testes/teste_gerador_while.c \
    "while (Boolean(" \
    "do {" \
    "console.log(i);"

run_case testes/teste_gerador_main_parametros.c \
    "function main(x: number): number" \
    "console.log(x);"

if grep -Fq "main();" "$tmp_dir/saida"; then
    echo "FALHA: main com parametros recebeu chamada automatica."
    ((failed+=1))
else
    echo "OK    main com parametros sem chamada automatica"
    ((passed+=1))
fi

echo
echo "$passed aprovados; $failed reprovados."

(( failed == 0 ))
