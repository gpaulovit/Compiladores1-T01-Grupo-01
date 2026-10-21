// ESPERADO: SUCESSO
int global_a, global_b = 2, global_c;

int soma(int a, int b) {
    return a + b;
}

int main() {
    int a, b = soma(1, 2), c;
    float x = 1.5, y, z = 2.0;
    a = b;
    c = global_b;
    y = x + z;
    print(a + c);
    print(y);
    return 0;
}
