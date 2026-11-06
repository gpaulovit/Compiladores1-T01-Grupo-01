int main() {
    int i = 0;
    int x = 7;
    int y = 2;
    int divisao = x / y;
    float decimal = x / 2.0;

    x += 2;
    x -= 1;
    x *= 2;
    x /= 2;

    while (i < 2) {
        print(i);
        i++;
    }

    do {
        x--;
    } while (x > 5);

    for (i = 0; i < 3; i++) {
        print(i);
    }

    print(divisao);
    print(decimal);

    return 0;
}
