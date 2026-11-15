// Arbol de minima cobertura por fuerza bruta.
// Se prueban todos los subconjuntos de n-1 aristas y se queda el de menor costo que no tenga ciclos.
// Uso: ./fuerza_bruta entrada.txt salida.txt
#include <chrono>
#include <climits>
#include <cstdio>
#include <vector>
using namespace std;

typedef long long ll;

struct Arista {
    int u, v;
    ll costo;
};

ll instrucciones = 0;       // Cuenta las instrucciones simples
int n, m;                   // Numero de nodos y de aristas
vector<Arista> E;           // Todas las aristas del grafo
vector<int> padre;          // Para detectar ciclos (conjuntos disjuntos)
vector<int> elegidas;       // Indices de las aristas que se estan probando
vector<int> mejor;          // Mejor conjunto encontrado hasta ahora
ll mejorCosto = LLONG_MAX;  // Costo del mejor conjunto

// Find-set sin compresion: sube hasta el representante
int find_set(int x) {
    instrucciones++;
    while (padre[x] != x) {
        x = padre[x];
        instrucciones += 3;
    }
    return x;
}

// Revisa si las n-1 aristas elegidas forman un arbol (sin ciclos) y guarda la mejor
void revisar() {
    for (int v = 0; v < n; v++) {
        padre[v] = v;
        instrucciones += 3;
    }
    ll costo = 0;
    for (int j = 0; j < (int)elegidas.size(); j++) {
        Arista e = E[elegidas[j]];
        int a = find_set(e.u);
        int b = find_set(e.v);
        instrucciones += 3;
        if (a == b) {
            return;   // Esta arista cierra un ciclo, se descarta el conjunto
        }
        padre[a] = b;
        costo += e.costo;
        instrucciones += 2;
    }
    instrucciones++;
    if (costo < mejorCosto) {
        mejorCosto = costo;
        mejor = elegidas;
        instrucciones += 2;
    }
}

// Elige aristas con indice desde 'desde' en adelante hasta tener n-1
void elegir(int desde) {
    instrucciones++;
    if ((int)elegidas.size() == n - 1) {
        revisar();
        return;
    }
    int faltan = n - 1 - (int)elegidas.size();
    instrucciones += 2;
    // Solo se sigue si quedan suficientes aristas para completar el conjunto
    for (int i = desde; i <= m - faltan; i++) {
        instrucciones += 2;
        elegidas.push_back(i);
        elegir(i + 1);
        elegidas.pop_back();
    }
}

int main(int argc, char** argv) {
    // Leer la entrada
    FILE* entrada = fopen(argv[1], "r");
    fscanf(entrada, "%d %d", &n, &m);
    E.resize(m);
    for (int i = 0; i < m; i++) {
        fscanf(entrada, "%d %d %lld", &E[i].u, &E[i].v, &E[i].costo);
    }
    fclose(entrada);

    padre.resize(n);

    // Medir el tiempo del algoritmo
    auto inicio = chrono::steady_clock::now();
    elegir(0);
    auto fin = chrono::steady_clock::now();
    ll tiempo = chrono::duration_cast<chrono::nanoseconds>(fin - inicio).count();

    // Escribir la salida
    FILE* salida = fopen(argv[2], "w");
    if (mejorCosto == LLONG_MAX) {
        fprintf(salida, "grafo no conexo\n");
    } else {
        fprintf(salida, "costo %lld\n", mejorCosto);
        fprintf(salida, "aristas %d\n", (int)mejor.size());
        for (int i = 0; i < (int)mejor.size(); i++) {
            Arista e = E[mejor[i]];
            fprintf(salida, "%d %d %lld\n", e.u, e.v, e.costo);
        }
    }
    fprintf(salida, "instrucciones %lld\n", instrucciones);
    fprintf(salida, "tiempo_ns %lld\n", tiempo);
    fclose(salida);
    return 0;
}