#!/usr/bin/env bash
# Uso: bash testes/run-tests.sh ./build/compilador testes/sprint2 testes/sprint3
set -u
set -o pipefail
shopt -s nullglob

compiler=${1:-./build/compilador}
if (( $# )); then shift; fi
if (( $# == 0 )); then set -- testes/sprint2 testes/sprint3; fi
if [[ ! -x "$compiler" && ! -x "${compiler}.exe" ]]; then
    echo "Compilador nao encontrado: $compiler" >&2
    exit 2
fi
if ! command -v timeout > /dev/null; then
    echo 'Dependencia ausente: timeout (GNU coreutils).' >&2
    exit 2
fi

log_dir=${TEST_LOG_DIR:-build/test-results}
mkdir -p "$log_dir" || exit 2
summary="$log_dir/resultados.tsv"
printf 'arquivo\tesperado\tobtido\tresultado\n' > "$summary" || exit 2
git rev-parse HEAD > "$log_dir/revisao.txt" 2>/dev/null || :
git diff -- src Makefile testes > "$log_dir/alteracoes.patch" 2>/dev/null || :
passed=0
failed=0

for suite in "$@"; do
    for group in validos invalidos; do
        files=("$suite/$group/"*.c)
        if (( ${#files[@]} == 0 )); then
            echo "Conjunto ausente ou vazio: $suite/$group" >&2
            exit 2
        fi
        for file in "${files[@]}"; do
            expected_status=0
            [[ "$group" == invalidos ]] && expected_status=1
            first_line=''
            IFS= read -r first_line < "$file" || :
            first_line=${first_line%$'\r'}
            expected_message=''
            if [[ "$first_line" == '// ESPERADO: '* ]]; then
                expected_message=${first_line#'// ESPERADO: '}
            fi
            # Sprint 2 preserva os arquivos originais e seus diagnosticos.
            if [[ -z "$expected_message" && "${suite%/}" == testes/sprint2 ]]; then
                case "$group/$(basename "$file")" in
                    validos/*) expected_message=SUCESSO ;;
                    invalidos/04_caractere_invalido.c) expected_message='Caractere desconhecido' ;;
                    invalidos/*) expected_message='Erro sintático' ;;
                esac
            fi
            if [[ -z "$expected_message" ]] ||
               { [[ "$group" == validos && "$expected_message" != SUCESSO ]]; } ||
               { [[ "$group" == invalidos && "$expected_message" == SUCESSO ]]; }; then
                echo "Cabecalho ESPERADO ausente ou inconsistente: $file" >&2
                exit 2
            fi
            destination="$log_dir/$(basename "${suite%/}")/$group"
            mkdir -p "$destination" || exit 2
            stem="$destination/$(basename "$file" .c)"
            status=0
            timeout "${TEST_TIMEOUT:-10}" "$compiler" < "$file" \
                > "$stem.stdout" 2> "$stem.stderr" || status=$?
            reason=''
            if (( status != expected_status )); then
                reason="codigo $status; esperado $expected_status"
            elif [[ "$group" == validos && -s "$stem.stderr" ]]; then
                reason='diagnostico inesperado em stderr'
            elif [[ "$group" == invalidos ]] &&
                 ! grep -Fq -- "$expected_message" "$stem.stderr"; then
                reason="mensagem ausente em stderr: $expected_message"
            fi
            if [[ -z "$reason" ]]; then
                ((passed+=1))
                result=APROVADO
                printf 'OK    %s\n' "$file"
            else
                ((failed+=1))
                result=REPROVADO
                printf 'FALHA %s (%s)\n' "$file" "$reason"
            fi
            printf '%s\t%s\t%s\t%s\n' "$file" "$expected_status" "$status" "$result" >> "$summary"
        done
    done
done
printf '\n%d aprovados; %d reprovados. Evidencias: %s\n' "$passed" "$failed" "$log_dir"
(( failed == 0 ))
