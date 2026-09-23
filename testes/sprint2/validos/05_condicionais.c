int main() {
    int valor = 3;
    if (valor > 0) {
        print(valor);
    }
    if (valor == 0) {
        print(0);
    } else {
        print(1);
    }
    if (valor > 0)
        if (valor > 5)
            print(5);
        else
            print(3);
    return 0;
}
