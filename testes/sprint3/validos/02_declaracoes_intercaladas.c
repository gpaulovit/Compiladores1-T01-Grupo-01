// ESPERADO: SUCESSO
int main() {
    int x;
    x = 1;
    int y = x + 1, z;
    z = y;
    {
        print(z);
        int interno = z + 1;
        print(interno);
    }
    print(x);
    int final = y + z;
    print(final);
    return 0;
}
