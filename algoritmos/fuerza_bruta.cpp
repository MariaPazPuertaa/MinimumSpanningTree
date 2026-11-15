// Arbol de minima cobertura por fuerza bruta.
// Se prueban todos los subconjuntos de n-1 aristas y se queda el de menor costo que no tenga ciclos.
// Uso: ./fuerza_bruta entrada.txt salida.txt
//
// Regla de conteo (la misma en kruskal.cpp): se suma 1 a "instrucciones" por cada
// instrucción simple ejecutada, es decir, una línea de código simple (asignación,
// comparación, condición de un if o while, llamada a función). El encabezado de un
// for cuenta 1 por cada iteración.
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
    instrucciones++;                        // llamada
    while (padre[x] != x) {
        instrucciones += 2;                 // condicion y asignacion
        x = padre[x];
    }
    instrucciones++;                        // ultima evaluacion de la condicion
    return x;
}

// Revisa si las n-1 aristas elegidas forman un arbol (sin ciclos) y guarda la mejor
void revisar() {
    instrucciones++;                        // llamada
    for (int v = 0; v < n; v++) {
        instrucciones += 2;                 // encabezado y asignacion
        padre[v] = v;
    }
    instrucciones++;
    ll costo = 0;
    instrucciones++;
    for (int j = 0; j < (int)elegidas.size(); j++) {
        instrucciones += 5;                 // encabezado, e = ..., a = ..., b = ..., if
        Arista e = E[elegidas[j]];
        int a = find_set(e.u);
        int b = find_set(e.v);
        if (a == b) {
            return;   // Esta arista cierra un ciclo, se descarta el conjunto
        }
        instrucciones += 2;                 // padre[a] = b y costo += ...
        padre[a] = b;
        costo += e.costo;
    }
    instrucciones += 2;                     // ultima evaluacion del for y el if
    if (costo < mejorCosto) {
        instrucciones += 1 + (ll)elegidas.size();   // asignacion de costo y copia del vector
        mejorCosto = costo;
        mejor = elegidas;
    }
}

// Elige aristas con indice desde 'desde' en adelante hasta tener n-1
void elegir(int desde) {
    instrucciones += 2;                     // llamada y if
    if ((int)elegidas.size() == n - 1) {
        revisar();
        return;
    }
    int faltan = n - 1 - (int)elegidas.size();
    instrucciones++;
    // Solo se sigue si quedan suficientes aristas para completar el conjunto
    for (int i = desde; i <= m - faltan; i++) {
        instrucciones += 3;                 // encabezado, push_back y pop_back
        elegidas.push_back(i);
        elegir(i + 1);
        elegidas.pop_back();
    }
    instrucciones++;
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
    if (fscanf(entrada, "%d %d", &n, &m) != 2 || n < 1 || m < 0) {
        printf("Formato de entrada invalido (primera linea: n m)\n");
        return 1;
    }
    E.resize(m);
    for (int i = 0; i < m; i++) {
        if (fscanf(entrada, "%d %d %lld", &E[i].u, &E[i].v, &E[i].costo) != 3 ||
            E[i].u < 0 || E[i].u >= n || E[i].v < 0 || E[i].v >= n) {
            printf("Formato de entrada invalido en la arista %d\n", i + 1);
            return 1;
        }
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
    if (!salida) {
        printf("No se pudo crear el archivo de salida: %s\n", argv[2]);
        return 1;
    }
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