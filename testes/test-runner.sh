#!/usr/bin/env bash
# Autoteste do executor, sem depender da implementacao semantica futura.
set -eu
mkdir -p build
work=$(mktemp -d build/runner-check.XXXXXX)
mkdir -p "$work/suite/validos" "$work/suite/invalidos"
printf '// ESPERADO: SUCESSO\n' > "$work/suite/validos/ok.c"
printf '// ESPERADO: erro de teste\n' > "$work/suite/invalidos/erro.c"
cat > "$work/compiler" <<'COMPILER'
#!/usr/bin/env bash
IFS= read -r expectation
case "${MODE:-correct}" in
    crash) exit 139 ;;
    hang) sleep 3; exit 0 ;;
esac
if [[ "$expectation" == *SUCESSO ]]; then
    [[ "${MODE:-correct}" == warning ]] && echo 'erro inesperado' >&2
    exit 0
fi
case "${MODE:-correct}" in
    wrong_message) echo 'outro erro' >&2; exit 1 ;;
    wrong_status) echo 'erro de teste' >&2; exit 0 ;;
    stdout_only) echo 'erro de teste'; exit 1 ;;
    *) echo 'erro de teste' >&2; exit 1 ;;
esac
COMPILER
chmod +x "$work/compiler"
for mode in correct wrong_message wrong_status stdout_only warning crash hang; do
    status=0
    MODE="$mode" TEST_TIMEOUT=1 TEST_LOG_DIR="$work/log-$mode" \
        bash testes/run-tests.sh "$work/compiler" "$work/suite" > "$work/$mode.txt" 2>&1 || status=$?
    expected=1
    [[ "$mode" == correct ]] && expected=0
    if (( status != expected )); then
        cat "$work/$mode.txt"
        echo "Autoteste falhou: $mode (codigo $status, esperado $expected)" >&2
        exit 1
    fi
done
printf '// sem cabecalho\n' > "$work/suite/invalidos/erro.c"
status=0
TEST_LOG_DIR="$work/log-header" bash testes/run-tests.sh "$work/compiler" "$work/suite" \
    > "$work/header.txt" 2>&1 || status=$?
test "$status" -eq 2
status=0
TEST_LOG_DIR="$work/log-missing" bash testes/run-tests.sh "$work/compiler" "$work/inexistente" \
    > "$work/missing.txt" 2>&1 || status=$?
test "$status" -eq 2
echo "Executor: 9 verificacoes aprovadas. Evidencias: $work"
