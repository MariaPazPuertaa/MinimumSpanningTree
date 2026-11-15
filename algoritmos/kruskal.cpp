// Kruskal(V, E, c) siguiendo el pseudocódigo visto en clase.
// Uso: ./kruskal entrada.txt salida.txt
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <vector>
using namespace std;

typedef long long ll;

struct Arista {
    int u, v;
    ll costo;
};

ll instrucciones = 0;   // Cuenta las instrucciones simples
vector<int> padre;      // padre[x] es el padre de x en su conjunto

// Compara dos aristas por costo (para ordenarlas)
bool comparar(Arista a, Arista b) {
    instrucciones++;
    return a.costo < b.costo;
}

void make_set(int x) {
    padre[x] = x;
    instrucciones++;
}

int find_set(int x) {
    instrucciones++;
    if (padre[x] != x) {
        padre[x] = find_set(padre[x]);   // Compresión de caminos
        instrucciones++;
    }
    return padre[x];
}

void union_set(int u, int v) {
    padre[find_set(u)] = find_set(v);
    instrucciones++;
}

// Kruskal(V, E, c): n es el número de nodos y m el número de aristas
vector<Arista> Kruskal(int n, int m, vector<Arista>& E) {
    // sort m edges by cost and renumber them
    sort(E.begin(), E.end(), comparar);

    // T = vacío
    vector<Arista> T;

    // for each v in V: make-set(v)
    padre.resize(n);
    for (int v = 0; v < n; v++) {
        make_set(v);
    }

    // for i = 1 to m
    for (int i = 0; i < m; i++) {
        // (u, v) = e_i
        int u = E[i].u;
        int v = E[i].v;
        instrucciones++;
        // if find-set(u) != find-set(v)
        if (find_set(u) != find_set(v)) {
            // T = T unión {e_i}
            T.push_back(E[i]);
            // union(u, v)
            union_set(u, v);
        }
    }

    // return T
    return T;
}

int main(int argc, char** argv) {
    // Leer la entrada
    FILE* entrada = fopen(argv[1], "r");
    int n, m;
    fscanf(entrada, "%d %d", &n, &m);
    vector<Arista> E(m);
    for (int i = 0; i < m; i++) {
        fscanf(entrada, "%d %d %lld", &E[i].u, &E[i].v, &E[i].costo);
    }
    fclose(entrada);

    // Medir el tiempo del algoritmo
    auto inicio = chrono::steady_clock::now();
    vector<Arista> T = Kruskal(n, m, E);
    auto fin = chrono::steady_clock::now();
    ll tiempo = chrono::duration_cast<chrono::nanoseconds>(fin - inicio).count();

    // Costo total del árbol
    ll total = 0;
    for (int i = 0; i < (int)T.size(); i++) {
        total += T[i].costo;
    }

    // Escribir la salida
    FILE* salida = fopen(argv[2], "w");
    fprintf(salida, "costo %lld\n", total);
    fprintf(salida, "aristas %d\n", (int)T.size());
    for (int i = 0; i < (int)T.size(); i++) {
        fprintf(salida, "%d %d %lld\n", T[i].u, T[i].v, T[i].costo);
    }
    fprintf(salida, "instrucciones %lld\n", instrucciones);
    fprintf(salida, "tiempo_ns %lld\n", tiempo);
    fclose(salida);
    return 0;
}