int soma(int a, int b) {
    return a + b;
}

int fatorial(int n) {
    if (n <= 1) {
        return 1;
    }
    return n * fatorial(n - 1);
}

void mostrar(int valor) {
    print(valor);
    return;
}

int main() {
    mostrar(soma(2, fatorial(5)));
    return 0;
}
