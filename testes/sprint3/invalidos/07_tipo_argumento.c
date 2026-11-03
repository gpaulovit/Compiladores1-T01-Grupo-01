// ESPERADO: tipo do argumento 1 incompativel na chamada de 'identidade'
int identidade(int valor) { return valor; }
int main() {
    print(identidade(1.5));
    return 0;
}
