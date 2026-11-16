// Generador de instancias aleatorias conexas para el árbol de mínima cobertura.
//
// Se genera un grafo conexo con n nodos y m aristas, con costos aleatorios, y se escribe en un
// archivo de texto con el mismo formato que leen kruskal.cpp y fuerza_bruta.cpp. Con la misma
// semilla siempre se obtiene la misma instancia.

// Uso: ./generador n m semilla salida.txt
// Se necesita que n-1 <= m <= n(n-1)/2.
// La salida tiene "n m" en la primera línea y luego m líneas "u v costo".

// Se pone para utilizar las funciones de printf, fopen, fprintf, entre otras. Que son para leer y escribir.
#include <cstdio>

// Se utiliza para la función atoi, que convierte los argumentos de texto en números.
#include <cstdlib>

// Se utiliza para el generador de números aleatorios mt19937.
#include <random>

// Se utiliza para guardar las aristas ya generadas y así no repetirlas.
#include <set>

// Se utiliza para hacer un vector.
#include <vector>
using namespace std;

// Es un número entero muy grande, que se utiliza para calcular el máximo de aristas para que un int normal
// no se desborde.
typedef long long ll;

// Es la función principal: Lee los parámetros, genera un árbol aleatorio para que el grafo sea conexo,
// agrega aristas distintas al azar hasta tener m y escribe el resultado en el archivo de salida.
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

    // Es el generador de números aleatorios, que con la misma semilla siempre produce la misma secuencia.
    mt19937 azar(semilla);

    // Son las aristas que ya se generaron, para no repetirlas. Cada arista se guarda con el nodo menor primero.
    set<pair<int, int>> usadas;  
    
    // Son todas las aristas generadas, en el orden en que se crearon.
    vector<pair<int, int>> aristas;

    // Primero un árbol aleatorio: Cada nodo v se conecta con un nodo anterior, así el grafo es conexo.
    for (int v = 1; v < n; v++) {
        int u = azar() % v;
        usadas.insert({u, v});
        aristas.push_back({u, v});
    }

    // Después se agregan aristas distintas al azar hasta tener m.
    while ((int)aristas.size() < m) {
        int a = azar() % n;
        int b = azar() % n;
        if (a == b) continue;
        int u = min(a, b), v = max(a, b);
        if (usadas.count({u, v})) continue;
        usadas.insert({u, v});
        aristas.push_back({u, v});
    }

    // Escribir el archivo con costos aleatorios entre 1 y 1000000.
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
