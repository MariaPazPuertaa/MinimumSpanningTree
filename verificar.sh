#!/bin/bash
# Verifica que Kruskal y la fuerza bruta den el mismo costo en 200 instancias aleatorias
# pequeñas (n entre 3 y 8, m entre n-1 y n(n-1)/2), y que ambos reporten "grafo no conexo"
# cuando el grafo no es conexo. 

# Uso: ./verificar.sh

# Se utiliza para moverse a la carpeta donde está este archivo, para que las rutas funcionen desde cualquier lado.
cd "$(dirname "$0")"

# Se utilizan para compilar los tres programas con optimización (-O2) y C++17 y si alguno falla, el script se detiene.
g++ -O2 -std=c++17 -o algoritmos/kruskal algoritmos/kruskal.cpp || exit 1
g++ -O2 -std=c++17 -o algoritmos/fuerza_bruta algoritmos/fuerza_bruta.cpp || exit 1
g++ -O2 -std=c++17 -o generador/generador generador/generador.cpp || exit 1

# Se utiliza para crear una carpeta temporal donde se guardan las instancias y las salidas de la prueba.
tmp=$(mktemp -d)

# Es un contador de los fallos que se encuentren durante la verificación.
fallos=0

# Para cada semilla entre 1 y 200 calcula un n entre 3 y 8 y un m entre n-1 y el máximo de aristas, genera
# una instancia aleatoria conexa y corre los dos algoritmos sobre ella. Además, cuenta un fallo si los costos son
# distintos o si Kruskal no devuelve n-1 aristas.
for semilla in $(seq 1 200); do
  n=$((3 + semilla % 6))
  maximo=$((n*(n-1)/2))
  m=$((n - 1 + (semilla * 7) % (maximo - n + 2)))
  generador/generador $n $m $semilla $tmp/in.txt > /dev/null || exit 1
  algoritmos/kruskal $tmp/in.txt $tmp/k.out
  algoritmos/fuerza_bruta $tmp/in.txt $tmp/b.out
  if [ "$(grep '^costo' $tmp/k.out)" != "$(grep '^costo' $tmp/b.out)" ]; then
    echo "DIFERENCIA con semilla $semilla (n=$n, m=$m)"
    fallos=$((fallos + 1))
  fi
  if [ "$(grep '^aristas' $tmp/k.out)" != "aristas $((n-1))" ]; then
    echo "Kruskal no devolvio n-1 aristas con semilla $semilla"
    fallos=$((fallos + 1))
  fi
done
echo "200 instancias aleatorias comparadas, discrepancias: $fallos"

# Revisa que los dos algoritmos detecten un grafo que no es conexo: La primera línea de la salida
# de cada uno debe ser "grafo no conexo".
# Grafo no conexo: nodos 0-1 y 2-3 sin conexión entre ellos.
printf "4 2\n0 1 5\n2 3 7\n" > $tmp/desc.txt
algoritmos/kruskal $tmp/desc.txt $tmp/k.out
algoritmos/fuerza_bruta $tmp/desc.txt $tmp/b.out
if [ "$(head -1 $tmp/k.out)" = "grafo no conexo" ] && [ "$(head -1 $tmp/b.out)" = "grafo no conexo" ]; then
  echo "Grafo no conexo: ambos lo detectan"
else
  echo "Grafo no conexo: FALLO"
  fallos=$((fallos + 1))
fi

# Revisa que los dos algoritmos manejen un grafo con un solo nodo y sin aristas: La primera línea de la
# salida de cada uno debe ser "costo 0".
# Un solo nodo: El árbol es vacío y cuesta 0.
printf "1 0\n" > $tmp/uno.txt
algoritmos/kruskal $tmp/uno.txt $tmp/k.out
algoritmos/fuerza_bruta $tmp/uno.txt $tmp/b.out
if [ "$(head -1 $tmp/k.out)" = "costo 0" ] && [ "$(head -1 $tmp/b.out)" = "costo 0" ]; then
  echo "Un solo nodo: ambos dan costo 0"
else
  echo "Un solo nodo: FALLO"
  fallos=$((fallos + 1))
fi

# Se utiliza para borrar la carpeta temporal.
rm -rf $tmp

# Muestra el resultado final: Si no hubo fallos imprime "TODO BIEN", y si hubo alguno imprime "HAY FALLOS"
# y termina con error.
[ $fallos -eq 0 ] && echo "TODO BIEN" || { 
  echo "HAY FALLOS"; 
  exit 1; 
  }