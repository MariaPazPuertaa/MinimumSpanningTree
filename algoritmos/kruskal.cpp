// Kruskal para árbol de mínima cobertura (MST), versión eficiente O(m log m), según el pseudocódigo visto en clase.

#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;

typedef long long ll;

// Contador global de instrucciones simples (comparaciones, asignaciones, aritmética)
static ll ops = 0;

struct Arista { int u, v; ll w; };

// Make-set: crea el conjunto unitario de v
void crear_conjunto(vector<int>& p, vector<int>& rango, int v) {
    p[v] = v;
    rango[v] = 0;
    ops += 2;
}

// Find-set con compresión de caminos
int buscar_conjunto(vector<int>& p, int x) {
    int r = x;
    ops += 1;
    while (p[r] != r) {
        r = p[r];
        ops += 3;
    }
    ops += 1;
    while (p[x] != r) {
        int sig = p[x];
        p[x] = r;
        x = sig;
        ops += 4;
    }
    ops += 1;
    return r;
}

// Union(u, v) por rango: une los conjuntos que contienen a u y a v
void unir(vector<int>& p, vector<int>& rango, int u, int v) {
    int a = buscar_conjunto(p, u);
    int b = buscar_conjunto(p, v);
    ops += 1;
    if (rango[a] < rango[b]) {
        p[a] = b;
        ops += 1;
    } else {
        p[b] = a;
        ops += 2;
        if (rango[a] == rango[b]) {
            rango[a]++;
            ops += 2;
        }
    }
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Uso: %s entrada salida\n", argv[0]);
        return 1;
    }
    FILE* fin = fopen(argv[1], "r");
    if (!fin) { perror("entrada"); return 1; }
    int n, m;
    if (fscanf(fin, "%d %d", &n, &m) != 2) return 1;
    vector<Arista> E(m);
    for (int i = 0; i < m; i++) {
        if (fscanf(fin, "%d %d %lld", &E[i].u, &E[i].v, &E[i].w) != 3) return 1;
    }
    fclose(fin);

    vector<Arista> T;
    ll costo = 0;

    // Inicia la medición de tiempo
    auto t0 = steady_clock::now();

    // Ordenar aristas por costo (cada comparación del comparador cuenta como una instrucción)
    sort(E.begin(), E.end(), [](const Arista& a, const Arista& b) {
        ops++;
        return a.w < b.w;
    });

    // Make-set para cada nodo
    vector<int> p(n), rango(n);
    for (int v = 0; v < n; v++) {
        crear_conjunto(p, rango, v);
        ops += 2;
    }

    for (int i = 0; i < m; i++) {
        ops += 2;
        int u = E[i].u, v = E[i].v;
        ops += 2;
        ops += 1;
        if (buscar_conjunto(p, u) != buscar_conjunto(p, v)) {
            T.push_back(E[i]);
            costo += E[i].w;
            ops += 2;
            unir(p, rango, u, v);
        }
    }

    auto t1 = steady_clock::now();
    ll ns = duration_cast<nanoseconds>(t1 - t0).count();

    FILE* fout = fopen(argv[2], "w");
    if (!fout) { perror("salida"); return 1; }
    if ((int)T.size() != n - 1) {
        fprintf(fout, "grafo no conexo\n");
    } else {
        fprintf(fout, "costo %lld\n", costo);
        fprintf(fout, "aristas %d\n", (int)T.size());
        for (auto& e : T) fprintf(fout, "%d %d %lld\n", e.u, e.v, e.w);
    }
    fprintf(fout, "instrucciones %lld\n", ops);
    fprintf(fout, "tiempo_ns %lld\n", ns);
    fclose(fout);
    return 0;
}
