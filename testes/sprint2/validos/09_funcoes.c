int soma(int a, int b) {
    return a + b;
}

float metade(float valor) {
    return valor / 2.0;
}

void mostrar() {
    print(1);
    return;
}

int main() {
    int total = soma(2, 3);
    float resultado = metade(5.0);
    mostrar();
    print(total);
    print(resultado);
    return 0;
}
