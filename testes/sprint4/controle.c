int main() {
    int i = 0, total = 0;
    for (i = 0; i < 3; i++) {
        total += i;
    }
    while (i > 1) {
        i--;
    }
    do {
        total++;
    } while (total < 5);
    if (i == 1) {
        print(total);
    } else {
        print(0);
    }
    return 0;
}
