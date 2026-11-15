// Kruskal(V, E, c): Árbol de mínima cobertura (MST) resuelto con el algoritmo de Kruskal.
// Se lee un grafo desde un archivo de texto, después se calcula el árbol de mínima cobertura y por
// último el resultado se pone en otro archivo.
//
// Se utiliza la regla de conteo: Donde se suma 1 a "instrucciones" por cada
// instrucción simple que se ejecuta, es decir, por cada línea simple (asignación,
// comparación, condición de un if, llamada a función, entre otras). El encabezado de un for
// cuenta 1 por cada iteración. En el ordenamiento se cuenta 1 por cada comparación
// que hace sort, así se mide el costo de ordenar.

// Uso: ./kruskal entrada.txt salida.txt
// La entrada tiene "n m" en la primera línea y luego m líneas "u v costo".

// Se utiliza para hacer el sort.
#include <algorithm> 

// Se utiliza para medir el tiempo.
#include <chrono>

// Se pone para utilizar las funciones de fopen, fscanf, fprint, entre otras. Que son para leer y escribir.
#include <cstdio>

// Se utiliza para hacer un vector.
#include <vector>

using namespace std;

// Es un número entero muy grande, que se utiliza en los costos y el contador para que un int normal
// no se desborde.
typedef long long ll;

// Es un arco del grafo: Que une los nodos u y v y tiene un costo.
// Por ejemplo, la línea "0 3 148568" de la entrada es u = 0, v = 3 y
// costo = 148568.
struct Arista {
    int u, v;
    ll costo;
};

// Es un contador de instrucciones simple.
ll instrucciones = 0;

// Es un arreglo del union-find: padre[x] es el "jefe" del nodo x.
vector<int> padre;      

// Es un procedimiento que compara dos aristas por costo para ordenarlas
bool comparar(const Arista& a, const Arista& b) {

    // Ayuda a contar cada comparación del sort.
    instrucciones++;
    return a.costo < b.costo;
}

// Es un procedimiento que al inicio cada nodo está solo en su propio grupo, es decir, es su propio jefe. Lo que
// significa que crea un grupo nuevo que tiene solo a x
void make_set(int x) {
    instrucciones += 2;                 // llamada y asignación
    padre[x] = x;
}

// Es una función que devuelve el jefe del grupo al que pertenece x.
int find_set(int x) {
    // Llamada e if.
    instrucciones += 2;     
    
    // Si x no es su propio jefe: seguir subiendo.
    if (padre[x] != x) {
        instrucciones++; 
        padre[x] = find_set(padre[x]);  // Compresión de caminos: Sube, y deja a x apuntando al jefe.
    }
    return padre[x]; // Retorna al jefe del grupo x.
}

// Este procedimiento junta el grupo de u con el grupo de v.
void union_set(int u, int v) {
    // Llamada y asignación
    instrucciones += 2;               
    padre[find_set(u)] = find_set(v);
}

// Kruskal(V, E, c): n es el número de nodos y m el número de aristas. 
// En este caso, devuelve las aristas del árbol de mínima cobertura. 
vector<Arista> Kruskal(int n, int m, vector<Arista>& E) {
    // sort m edges by cost and renumber them
    // Ordena las aristas de menor a mayor costo.
    sort(E.begin(), E.end(), comparar);

    // T (Guarda los aristas del árbol) = vacío
    vector<Arista> T;
    padre.resize(n);
    instrucciones += 2;

    // for each v in V: make-set(v)
    // Cada nodo empieza en su propio grupo.
    for (int v = 0; v < n; v++) {
        instrucciones++;                
        make_set(v);
    }

    // for i = 1 to m
    // Recorre las aristas en orden, del más barato al más caro.
    for (int i = 0; i < m; i++) {
        instrucciones += 4;             
        // (u, v) = e_i
        int u = E[i].u;
        int v = E[i].v;
        // if find-set(u) != find-set(v)
        // Si u y v están en grupos distintos, las aristas no forman ciclo.
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
    // Es una condición que revisa que si pasaron los dos archivos.
    if (argc != 3) {
        printf("Uso: %s entrada.txt salida.txt\n", argv[0]);
        return 1;
    }

    // Leer la entrada: Primero n y m, y luego los m arcos
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

    // Medir el tiempo que tarda Kruskal (sin contar leer ni escribir los archivos).
    auto inicio = chrono::steady_clock::now();
    vector<Arista> T = Kruskal(n, m, E);
    auto fin = chrono::steady_clock::now();
    ll tiempo = chrono::duration_cast<chrono::nanoseconds>(fin - inicio).count();

    // Escribir la salida o resultado.
    FILE* salida = fopen(argv[2], "w");
    if (!salida) {
        printf("No se pudo crear el archivo de salida: %s\n", argv[2]);
        return 1;
    }
    if ((int)T.size() != n - 1) {
        // Un árbol de cobertura tiene n-1 aristas; si hay menos, el grafo no es conexo.
        fprintf(salida, "grafo no conexo\n");
    } else {
        // Sumar el costo de las aristas del árbol.
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
