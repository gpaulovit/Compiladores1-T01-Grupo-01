function total(): number {
    return 9;
}

function valor(i: number): number {
    if (Boolean(Number((i == 0)))) {
        return 50;
    }
    if (Boolean(Number((i == 1)))) {
        return 30;
    }
    if (Boolean(Number((i == 2)))) {
        return 90;
    }
    if (Boolean(Number((i == 3)))) {
        return 20;
    }
    if (Boolean(Number((i == 4)))) {
        return 40;
    }
    if (Boolean(Number((i == 5)))) {
        return 95;
    }
    if (Boolean(Number((i == 6)))) {
        return 10;
    }
    if (Boolean(Number((i == 7)))) {
        return 35;
    }
    if (Boolean(Number((i == 8)))) {
        return 45;
    }
    return 0;
}

function raiz(inicio: number, lo: number, hi: number): number {
    let i: number = inicio;
    while (Boolean(Number((i < total())))) {
        if (Boolean(Number(Boolean(Number((valor(i) > lo))) && Boolean(Number((valor(i) < hi)))))) {
            return i;
        }
        (i++);
    }
    return (-1);
}

function esq(no: number, lo: number): number {
    return raiz((no + 1), lo, valor(no));
}

function dir(no: number, hi: number): number {
    return raiz((no + 1), valor(no), hi);
}

function pre_ordem(no: number, lo: number, hi: number): void {
    if (Boolean(Number((no != (-1))))) {
        console.log(valor(no));
        pre_ordem(esq(no, lo), lo, valor(no));
        pre_ordem(dir(no, hi), valor(no), hi);
    }
}

function in_ordem(no: number, lo: number, hi: number): void {
    if (Boolean(Number((no != (-1))))) {
        in_ordem(esq(no, lo), lo, valor(no));
        console.log(valor(no));
        in_ordem(dir(no, hi), valor(no), hi);
    }
}

function pos_ordem(no: number, lo: number, hi: number): void {
    if (Boolean(Number((no != (-1))))) {
        pos_ordem(esq(no, lo), lo, valor(no));
        pos_ordem(dir(no, hi), valor(no), hi);
        console.log(valor(no));
    }
}

function buscar(no: number, lo: number, hi: number, v: number): number {
    if (Boolean(Number((no == (-1))))) {
        return 0;
    }
    else {
        if (Boolean(Number((v < valor(no))))) {
            return buscar(esq(no, lo), lo, valor(no), v);
        }
        else {
            if (Boolean(Number((v > valor(no))))) {
                return buscar(dir(no, hi), valor(no), hi, v);
            }
            else {
                return 1;
            }
        }
    }
}

function min(no: number, lo: number): number {
    let e: number = esq(no, lo);
    if (Boolean(Number((e != (-1))))) {
        return min(e, lo);
    }
    else {
        return valor(no);
    }
}

function max(no: number, hi: number): number {
    let d: number = dir(no, hi);
    if (Boolean(Number((d != (-1))))) {
        return max(d, hi);
    }
    else {
        return valor(no);
    }
}

function contarNos(no: number, lo: number, hi: number): number {
    if (Boolean(Number((no == (-1))))) {
        return 0;
    }
    else {
        return ((1 + contarNos(esq(no, lo), lo, valor(no))) + contarNos(dir(no, hi), valor(no), hi));
    }
}

function maior(a: number, b: number): number {
    if (Boolean(Number((a > b)))) {
        return a;
    }
    else {
        return b;
    }
}

function maior_ramo(no: number, lo: number, hi: number): number {
    if (Boolean(Number((no == (-1))))) {
        return 0;
    }
    else {
        return maior((maior_ramo(dir(no, hi), valor(no), hi) + valor(no)), (maior_ramo(esq(no, lo), lo, valor(no)) + valor(no)));
    }
}

function main(): number {
    let lo: number = (-1000000);
    let hi: number = 1000000;
    let a: number = raiz(0, lo, hi);
    console.log("pre-ordem:");
    pre_ordem(a, lo, hi);
    console.log("in-ordem:");
    in_ordem(a, lo, hi);
    console.log("pos-ordem:");
    pos_ordem(a, lo, hi);
    console.log("min, max, nos:");
    console.log(min(a, lo));
    console.log(max(a, hi));
    console.log(contarNos(a, lo, hi));
    console.log("buscar 35 e 60:");
    console.log(buscar(a, lo, hi, 35));
    console.log(buscar(a, lo, hi, 60));
    console.log("maior ramo:");
    console.log(maior_ramo(a, lo, hi));
    return 0;
}

main();
