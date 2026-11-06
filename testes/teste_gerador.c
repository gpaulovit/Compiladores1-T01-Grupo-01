int somar(int a, int b) {
    return a + b;
}

int main() {
    int x = 10;
    int y = 3;
    int resultado = somar(x, y);

    if (resultado > 10) {
        print(resultado);
    } else {
        print(0);
    }

    return 0;
}
