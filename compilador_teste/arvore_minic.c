
int total() {
    return 9;
}


int valor(int i) {
    if (i == 0) return 50;
    if (i == 1) return 30;
    if (i == 2) return 90;
    if (i == 3) return 20;
    if (i == 4) return 40;
    if (i == 5) return 95;
    if (i == 6) return 10;
    if (i == 7) return 35;
    if (i == 8) return 45;
    return 0;
}


int raiz(int inicio, int lo, int hi) {
    int i = inicio;
    while (i < total()) {
        if (valor(i) > lo && valor(i) < hi) {
            return i;
        }
        i++;
    }
    return -1;
}

int esq(int no, int lo) {
    return raiz(no + 1, lo, valor(no));
}

int dir(int no, int hi) {
    return raiz(no + 1, valor(no), hi);
}

void pre_ordem(int no, int lo, int hi) {
    if (no != -1) {
        print(valor(no));
        pre_ordem(esq(no, lo), lo, valor(no));
        pre_ordem(dir(no, hi), valor(no), hi);
    }
}

void in_ordem(int no, int lo, int hi) {
    if (no != -1) {
        in_ordem(esq(no, lo), lo, valor(no));
        print(valor(no));
        in_ordem(dir(no, hi), valor(no), hi);
    }
}

void pos_ordem(int no, int lo, int hi) {
    if (no != -1) {
        pos_ordem(esq(no, lo), lo, valor(no));
        pos_ordem(dir(no, hi), valor(no), hi);
        print(valor(no));
    }
}

int buscar(int no, int lo, int hi, int v) {
    if (no == -1) {
        return 0;
    } else if (v < valor(no)) {
        return buscar(esq(no, lo), lo, valor(no), v);
    } else if (v > valor(no)) {
        return buscar(dir(no, hi), valor(no), hi, v);
    } else {
        return 1;
    }
}

int min(int no, int lo) {
    int e = esq(no, lo);
    if (e != -1) {
        return min(e, lo);
    } else {
        return valor(no);
    }
}

int max(int no, int hi) {
    int d = dir(no, hi);
    if (d != -1) {
        return max(d, hi);
    } else {
        return valor(no);
    }
}

int contarNos(int no, int lo, int hi) {
    if (no == -1) {
        return 0;
    } else {
        return 1 + contarNos(esq(no, lo), lo, valor(no))
                 + contarNos(dir(no, hi), valor(no), hi);
    }
}

int maior(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

int maior_ramo(int no, int lo, int hi) {
    if (no == -1) {
        return 0;
    } else {
        return maior(maior_ramo(dir(no, hi), valor(no), hi) + valor(no),
                     maior_ramo(esq(no, lo), lo, valor(no)) + valor(no));
    }
}

int main() {
    int lo = -1000000;
    int hi = 1000000;
    int a = raiz(0, lo, hi);

    print("pre-ordem:");
    pre_ordem(a, lo, hi);
    print("in-ordem:");
    in_ordem(a, lo, hi);
    print("pos-ordem:");
    pos_ordem(a, lo, hi);

    print("min, max, nos:");
    print(min(a, lo));
    print(max(a, hi));
    print(contarNos(a, lo, hi));

    print("buscar 35 e 60:");
    print(buscar(a, lo, hi, 35));
    print(buscar(a, lo, hi, 60));

    print("maior ramo:");
    print(maior_ramo(a, lo, hi));
    return 0;
}
