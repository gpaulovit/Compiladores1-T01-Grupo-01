#!/usr/bin/env bash
set -u

compiler=${1:-./build/compilador}
suite=testes/sprint4
if ! node -e 'process.exit(Number(process.versions.node.split(".")[0]) >= 24 ? 0 : 1)' 2>/dev/null; then
    echo 'Os testes da Sprint 4 requerem Node.js 24 ou superior.' >&2
    exit 2
fi

mkdir -p build || exit 2
logs=$(mktemp -d build/test-sprint4.XXXXXX) || exit 2
failed=0
for source in "$suite"/*.c; do
    name=${source##*/}
    output="$logs/${name%.c}"
    if timeout 10 "$compiler" --ts < "$source" > "$output.ts" 2> "$output.ts.stderr" &&
       [[ ! -s "$output.ts.stderr" ]] &&
       timeout 10 node "$output.ts" > "$output.saida" 2> "$output.node.stderr" &&
       [[ ! -s "$output.node.stderr" ]] &&
       git -c core.autocrlf=false diff --no-index --ignore-cr-at-eol -- "${source%.c}.saida" "$output.saida"; then
        echo "OK TypeScript: $name"
    else
        echo "FALHA TypeScript: $name (consulte $logs)"
        ((failed+=1))
    fi

    if timeout 10 "$compiler" --tac < "$source" > "$output.tac" 2> "$output.tac.stderr" &&
       [[ ! -s "$output.tac.stderr" ]] &&
       git -c core.autocrlf=false diff --no-index --ignore-cr-at-eol -- "${source%.c}.tac" "$output.tac"; then
        echo "OK TAC: $name"
    else
        echo "FALHA TAC: $name (consulte $logs)"
        ((failed+=1))
    fi
done

for source in testes/sprint2/invalidos/01_sem_ponto_virgula.c \
              testes/sprint2/invalidos/04_caractere_invalido.c \
              testes/sprint3/invalidos/01_variavel_nao_declarada.c; do
    name=${source##*/}
    status=0
    timeout 10 "$compiler" --tac < "$source" > "$logs/$name.stdout" \
        2> "$logs/$name.stderr" || status=$?
    if [[ "$status" == 1 && ! -s "$logs/$name.stdout" && -s "$logs/$name.stderr" ]]; then
        echo "OK TAC bloqueado: $name"
    else
        echo "FALHA TAC bloqueado: $name (consulte $logs)"
        ((failed+=1))
    fi
done

echo "$failed falhas na Sprint 4. Evidências em $logs"
(( failed == 0 ))
