// ESPERADO: SUCESSO
float identidade(float valor) { return valor; }
float inteiro_como_float() { return 2; }
int main() {
    int inteiro = 2;
    float real = inteiro;
    real = inteiro + 0.5;
    real = identidade(inteiro);
    print(real);
    print(inteiro_como_float());
    return 0;
}
