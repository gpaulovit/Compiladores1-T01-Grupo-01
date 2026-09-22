/* Comentarios devem ser ignorados pelo scanner.
   Estes simbolos nao fazem parte do programa: @ # [ ] */
int main() {
    int valor = 2;
    // Bloco interno com declaracao local antes dos comandos.
    {
        int dobro = valor * 2;
        print(dobro);
    }
    print(valor /* comentario dentro de uma expressao */ + 1);
    return 0;
}
