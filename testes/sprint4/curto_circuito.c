int efeito() {
    print(99);
    return 1;
}

int main() {
    int x = 1;
    print(0 && efeito());
    print(1 || efeito());
    print(x + x++);
    print(x);
    return 0;
}
