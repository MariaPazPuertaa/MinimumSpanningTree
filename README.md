# Arbol de minima cobertura: Kruskal frente a fuerza bruta

## Estructura

| Carpeta o archivo | Contenido |
|---|---|
| `algoritmos/kruskal.cpp` | Algoritmo eficiente (pseudocodigo visto en clase) |
| `algoritmos/fuerza_bruta.cpp` | Version de fuerza bruta |
| `generador/generador.cpp` | Crea instancias aleatorias |
| `instancias/` | Archivos de entrada generados (las de mas de 100000 nodos se borran por tamano y se regeneran con la semilla 7) |
| `resultados/` | Salidas de los algoritmos y `resultados.csv` |
| `graficas/` | Figuras que usa el informe |
| `informe/informe.tex` | Informe en LaTeX (y `informe.pdf`) |
| `experimentos.ps1` / `experimentos.sh` | Compilan, generan instancias y ejecutan todo (cada corrida se repite y se guarda la mediana del tiempo) |
| `verificar.sh` | Compara Kruskal y fuerza bruta en 200 instancias aleatorias y revisa los casos de grafo no conexo y de un solo nodo |
| `analisis.py` | Calcula las constantes y crea tablas, graficas y `informe/constantes.tex` |

## Puntos del enunciado

1. Dos algoritmos con entrada y salida por archivo: `algoritmos/`
2. Contar instrucciones: variable `instrucciones` (misma regla en los dos programas, explicada al inicio de cada archivo)
3. Medir el tiempo: `chrono` en `main`
4. Generador de instancias: `generador/generador.cpp`
5. Tabla con varios tamanos: `resultados/resultados.csv` y tablas del informe
6. Analisis teorico: seccion del informe
7. Grafica y constante: `analisis.py` y `graficas/`
8. Informe en LaTeX: `informe/informe.tex`

## Uso (desde la carpeta MinimumSpanningTree, en PowerShell)

Compilar:

    g++ -O2 -std=c++17 -o algoritmos/kruskal.exe algoritmos/kruskal.cpp
    g++ -O2 -std=c++17 -o algoritmos/fuerza_bruta.exe algoritmos/fuerza_bruta.cpp
    g++ -O2 -std=c++17 -o generador/generador.exe generador/generador.cpp

Crear una instancia (5 nodos, 7 aristas, semilla 1) y resolverla:

    .\generador\generador.exe 5 7 1 instancias\entrada.txt
    .\algoritmos\kruskal.exe instancias\entrada.txt resultados\salida.txt
    .\algoritmos\fuerza_bruta.exe instancias\entrada.txt resultados\salida_fb.txt

Formato de entrada: primera linea `n m`, luego `m` lineas `u v costo` (nodos de 0 a n-1).
Formato de salida: `costo`, `aristas`, la lista de aristas, `instrucciones` y `tiempo_ns`.
Si el grafo no es conexo, en vez de costo y aristas se escribe `grafo no conexo`.

Experimentos completos: `.\experimentos.ps1` (la fuerza bruta con 10 nodos tarda cerca de un minuto por repeticion, 3 repeticiones).

Verificacion de correccion (en bash): `./verificar.sh`

Graficas y tablas: `python analisis.py` (requiere `pip install matplotlib`). Tambien escribe `informe/constantes.tex`, de donde el informe toma las constantes y los numeros de las conclusiones.

Informe: dentro de `informe/`, `pdflatex informe.tex` dos veces. Si se vuelven a correr los experimentos en otro equipo, hay que correr `analisis.py` y recompilar el informe, y releer la seccion "Comparacion con los datos".
