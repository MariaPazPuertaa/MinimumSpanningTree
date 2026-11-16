// Árbol de mínima cobertura por fuerza bruta.
// Se prueban todos los subconjuntos de n-1 aristas y se queda el de menor costo que no tenga ciclos.
//
// Se utiliza la regla de conteo: Donde se suma 1 a "instrucciones" por cada
// instrucción simple ejecutada, es decir, una línea de código simple (asignación,
// comparación, condición de un if o while, llamada a función, entre otras). Además, el encabezado de un
// for cuenta 1 por cada iteración.

// Uso: ./fuerza_bruta entrada.txt salida.txt
// La entrada tiene "n m" en la primera línea y luego m líneas "u v costo".

// Se utiliza para medir el tiempo.
#include <chrono>

// Se utiliza para tener LLONG_MAX, que es el valor más grande de un long long.
#include <climits>

// Se pone para utilizar las funciones de fopen, fscanf, fprintf, entre otras. Que son para leer y escribir.
#include <cstdio>

// Se utiliza para hacer un vector.
#include <vector>

using namespace std;

// Es un número entero muy grande, que se utiliza en los costos y el contador para que un int normal
// no se desborde.
typedef long long ll;

// Es una arista del grafo: Que une los nodos u y v y tiene un costo.
// Por ejemplo, la línea "0 3 148568" de la entrada es u = 0, v = 3 y
// costo = 148568.
struct Arista {
    int u, v;
    ll costo;
};

// Es un contador de instrucciones simples.
ll instrucciones = 0;  

// Es el número de nodos y de aristas.
int n, m;     

// Son todas las aristas del grafo.
vector<Arista> E; 

// Se utiliza para detectar ciclos.
vector<int> padre;  

// Son los índices de las aristas que se están probando.
vector<int> elegidas; 

// Guarda el mejor conjunto encontrado hasta ahora.
vector<int> mejor;  

// Es el costo del mejor conjunto.
ll mejorCosto = LLONG_MAX;  

// Es una función que devuelve el jefe del grupo al que pertenece x, subiendo por el arreglo padre
// hasta llegar al nodo que es su propio jefe.
// Find-set sin compresión: sube hasta el representante.
int find_set(int x) {
    instrucciones++;                        
    while (padre[x] != x) {
        instrucciones += 2;                 
        x = padre[x];
    }
    instrucciones++;                        
    return x;
}

// Es un procedimiento que se ejecuta cuando ya hay n-1 aristas elegidas: Deja a cada nodo en su propio
// grupo, va agregando las aristas elegidas y si alguna cierra un ciclo descarta el conjunto. Si no hay
// ciclos, compara el costo con el mejor que se tenga y lo guarda si es menor.
void revisar() {
    instrucciones++;                        
    for (int v = 0; v < n; v++) {
        instrucciones += 2;                 
        padre[v] = v;
    }
    instrucciones++;
    ll costo = 0;
    instrucciones++;
    for (int j = 0; j < (int)elegidas.size(); j++) {
        instrucciones += 5;                 
        Arista e = E[elegidas[j]];
        int a = find_set(e.u);
        int b = find_set(e.v);
        if (a == b) {
            // Esta arista cierra un ciclo.
            return;   
        }
        instrucciones += 2;                 
        padre[a] = b;
        costo += e.costo;
    }
    instrucciones += 2;                     
    if (costo < mejorCosto) {
        // Es la asignación de costo y copia del vector.
        instrucciones += 1 + (ll)elegidas.size();   
        mejorCosto = costo;
        mejor = elegidas;
    }
}

// Es un procedimiento recursivo que arma todos los subconjuntos de n-1 aristas: En cada llamada prueba, una por una,
// las aristas con índice mayor a las que ya fueron elegidas, para no repetir combinaciones, y cuando ya hay n-1
// aristas elegidas, llama a revisar.
void elegir(int desde) {
    instrucciones += 2;                     
    if ((int)elegidas.size() == n - 1) {
        revisar();
        return;
    }
    int faltan = n - 1 - (int)elegidas.size();
    instrucciones++;
    // Solo se sigue si quedan suficientes aristas para completar el conjunto.
    for (int i = desde; i <= m - faltan; i++) {
        instrucciones += 3;                 
        elegidas.push_back(i);
        elegir(i + 1);
        elegidas.pop_back();
    }
    instrucciones++;
}

// Es la función principal: Lee el grafo del archivo de entrada, calcula el árbol de mínima cobertura
// con fuerza bruta, mide el tiempo y escribe el resultado en el archivo de salida.
int main(int argc, char** argv) {
    if (argc != 3) {
        printf("Uso: %s entrada.txt salida.txt\n", argv[0]);
        return 1;
    }

    // Leer la entrada: Primero n y m, y luego las m aristas.
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

    // Medir el tiempo del algoritmo.
    auto inicio = chrono::steady_clock::now();
    elegir(0);
    auto fin = chrono::steady_clock::now();
    ll tiempo = chrono::duration_cast<chrono::nanoseconds>(fin - inicio).count();

    // Escribir la salida.
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