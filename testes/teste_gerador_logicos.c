int main() {
    int a = 5;
    int b = 3;

    int maior = a > b;
    int igual = a == b;
    int logico = a && b;
    int negacao = !a;

    print(maior);
    print(igual);
    print(logico);
    print(negacao);

    if (a > b && b > 0) {
        print(1);
    }

    return 0;
}
