// ESPERADO: SUCESSO
int valor = 1;
int main() {
    int valor = 2;
    {
        int valor = 3;
        print(valor);
    }
    print(valor);
    {
        int temporario = 4;
        print(temporario);
    }
    {
        int temporario = 5;
        print(temporario);
    }
    return 0;
}
