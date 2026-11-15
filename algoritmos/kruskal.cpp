// Kruskal(V, E, c) siguiendo el pseudocódigo visto en clase.
// Uso: ./kruskal entrada.txt salida.txt
//
// Regla de conteo (la misma en fuerza_bruta.cpp): se suma 1 a "instrucciones" por cada
// instrucción simple que se ejecuta, es decir, por cada línea simple (asignación,
// comparación, condición de un if, llamada a función). El encabezado de un for
// cuenta 1 por cada iteración. En el ordenamiento se cuenta 1 por cada comparación
// que hace sort (así se mide normalmente el costo de ordenar).
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
vector<int> padre;      // padre[x] es el padre de x en su conjunto (union-find)

// Compara dos aristas por costo (para ordenarlas)
bool comparar(const Arista& a, const Arista& b) {
    instrucciones++;
    return a.costo < b.costo;
}

void make_set(int x) {
    instrucciones += 2;                 // llamada y asignación
    padre[x] = x;
}

int find_set(int x) {
    instrucciones += 2;                 // llamada y if
    if (padre[x] != x) {
        instrucciones++;
        padre[x] = find_set(padre[x]);  // Compresión de caminos
    }
    return padre[x];
}

void union_set(int u, int v) {
    instrucciones += 2;                 // llamada y asignación
    padre[find_set(u)] = find_set(v);
}

// Kruskal(V, E, c): n es el número de nodos y m el número de aristas
vector<Arista> Kruskal(int n, int m, vector<Arista>& E) {
    // sort m edges by cost and renumber them
    sort(E.begin(), E.end(), comparar);

    // T = vacío
    vector<Arista> T;
    padre.resize(n);
    instrucciones += 2;

    // for each v in V: make-set(v)
    for (int v = 0; v < n; v++) {
        instrucciones++;                // encabezado del for
        make_set(v);
    }

    // for i = 1 to m
    for (int i = 0; i < m; i++) {
        instrucciones += 4;             // encabezado, u, v y el if
        // (u, v) = e_i
        int u = E[i].u;
        int v = E[i].v;
        // if find-set(u) != find-set(v)
        if (find_set(u) != find_set(v)) {
            instrucciones++;
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
    if (argc != 3) {
        printf("Uso: %s entrada.txt salida.txt\n", argv[0]);
        return 1;
    }

    // Leer la entrada
    FILE* entrada = fopen(argv[1], "r");
    if (!entrada) {
        printf("No se pudo abrir el archivo de entrada: %s\n", argv[1]);
        return 1;
    }
    int n, m;
    if (fscanf(entrada, "%d %d", &n, &m) != 2 || n < 1 || m < 0) {
        printf("Formato de entrada invalido (primera linea: n m)\n");
        return 1;
    }
    vector<Arista> E(m);
    for (int i = 0; i < m; i++) {
        if (fscanf(entrada, "%d %d %lld", &E[i].u, &E[i].v, &E[i].costo) != 3 ||
            E[i].u < 0 || E[i].u >= n || E[i].v < 0 || E[i].v >= n) {
            printf("Formato de entrada invalido en la arista %d\n", i + 1);
            return 1;
        }
    }
    fclose(entrada);

    // Medir el tiempo del algoritmo
    auto inicio = chrono::steady_clock::now();
    vector<Arista> T = Kruskal(n, m, E);
    auto fin = chrono::steady_clock::now();
    ll tiempo = chrono::duration_cast<chrono::nanoseconds>(fin - inicio).count();

    // Escribir la salida
    FILE* salida = fopen(argv[2], "w");
    if (!salida) {
        printf("No se pudo crear el archivo de salida: %s\n", argv[2]);
        return 1;
    }
    if ((int)T.size() != n - 1) {
        // Un árbol de cobertura tiene n-1 aristas; si hay menos, el grafo no es conexo
        fprintf(salida, "grafo no conexo\n");
    } else {
        ll total = 0;
        for (int i = 0; i < (int)T.size(); i++) {
            total += T[i].costo;
        }
        fprintf(salida, "costo %lld\n", total);
        fprintf(salida, "aristas %d\n", (int)T.size());
        for (int i = 0; i < (int)T.size(); i++) {
            fprintf(salida, "%d %d %lld\n", T[i].u, T[i].v, T[i].costo);
        }
    }
    fprintf(salida, "instrucciones %lld\n", instrucciones);
    fprintf(salida, "tiempo_ns %lld\n", tiempo);
    fclose(salida);
    return 0;
}
