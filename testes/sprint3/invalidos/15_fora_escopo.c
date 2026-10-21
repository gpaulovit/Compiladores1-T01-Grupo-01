// ESPERADO: variavel 'interno' nao declarada
int main() {
    { int interno = 1; }
    print(interno);
    return 0;
}
