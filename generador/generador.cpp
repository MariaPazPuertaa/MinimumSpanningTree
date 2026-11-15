// Generador de instancias aleatorias conexas para el arbol de minima cobertura.
// Uso: ./generador n m semilla salida.txt
// Se necesita que n-1 <= m <= n(n-1)/2.
#include <cstdio>
#include <cstdlib>
#include <random>
#include <set>
#include <vector>
using namespace std;

typedef long long ll;

int main(int argc, char** argv) {
    if (argc != 5) {
        printf("Uso: %s n m semilla salida.txt\n", argv[0]);
        return 1;
    }
    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    int semilla = atoi(argv[3]);

    ll maximo = (ll)n * (n - 1) / 2;
    if (n < 2 || m < n - 1 || m > maximo) {
        printf("Parametros invalidos: se necesita n-1 <= m <= n(n-1)/2\n");
        return 1;
    }

    mt19937 azar(semilla);
    set<pair<int, int>> usadas;                 // Aristas ya generadas (para no repetir)
    vector<pair<int, int>> aristas;

    // Primero un arbol aleatorio: cada nodo v se conecta con un nodo anterior, asi el grafo es conexo
    for (int v = 1; v < n; v++) {
        int u = azar() % v;
        usadas.insert({u, v});
        aristas.push_back({u, v});
    }

    // Despues se agregan aristas distintas al azar hasta tener m
    while ((int)aristas.size() < m) {
        int a = azar() % n;
        int b = azar() % n;
        if (a == b) continue;
        int u = min(a, b), v = max(a, b);
        if (usadas.count({u, v})) continue;
        usadas.insert({u, v});
        aristas.push_back({u, v});
    }

    // Escribir el archivo con costos aleatorios entre 1 y 1000000
    FILE* salida = fopen(argv[4], "w");
    if (!salida) {
        printf("No se pudo crear el archivo de salida: %s\n", argv[4]);
        return 1;
    }
    fprintf(salida, "%d %d\n", n, m);
    for (int i = 0; i < m; i++) {
        fprintf(salida, "%d %d %d\n", aristas[i].first, aristas[i].second, (int)(azar() % 1000000) + 1);
    }
    fclose(salida);
    return 0;
}
