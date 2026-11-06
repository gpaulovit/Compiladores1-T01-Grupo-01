float calcular_media(float a, float b) {
    return (a + b) / 2.0;
}

void mostrar(float valor) {
    print(valor);
    return;
}

int divisao_negativa() {
    return -7 / 2;
}

int main() {
    float media = calcular_media(8.0, 6.0);
    int resultado = divisao_negativa();

    mostrar(media);
    print(resultado);

    return 0;
}
