int main() {
    int i = 0;
    int limite = 2;

    while (i < limite && limite > 0) {
        print(i);
        i++;
    }

    do {
        i--;
    } while (!(i == 0));

    return 0;
}
