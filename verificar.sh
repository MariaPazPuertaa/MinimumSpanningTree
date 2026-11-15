#!/bin/bash
# Verifica que Kruskal y la fuerza bruta den el mismo costo en 200 instancias aleatorias
# pequenas (n entre 3 y 8, m entre n-1 y n(n-1)/2), y que ambos reporten "grafo no conexo"
# cuando el grafo no es conexo. Uso: ./verificar.sh
cd "$(dirname "$0")"
g++ -O2 -std=c++17 -o algoritmos/kruskal algoritmos/kruskal.cpp || exit 1
g++ -O2 -std=c++17 -o algoritmos/fuerza_bruta algoritmos/fuerza_bruta.cpp || exit 1
g++ -O2 -std=c++17 -o generador/generador generador/generador.cpp || exit 1
tmp=$(mktemp -d)

fallos=0
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

# Grafo no conexo: nodos 0-1 y 2-3 sin conexion entre ellos
printf "4 2\n0 1 5\n2 3 7\n" > $tmp/desc.txt
algoritmos/kruskal $tmp/desc.txt $tmp/k.out
algoritmos/fuerza_bruta $tmp/desc.txt $tmp/b.out
if [ "$(head -1 $tmp/k.out)" = "grafo no conexo" ] && [ "$(head -1 $tmp/b.out)" = "grafo no conexo" ]; then
  echo "Grafo no conexo: ambos lo detectan"
else
  echo "Grafo no conexo: FALLO"
  fallos=$((fallos + 1))
fi

# Un solo nodo: el arbol es vacio y cuesta 0
printf "1 0\n" > $tmp/uno.txt
algoritmos/kruskal $tmp/uno.txt $tmp/k.out
algoritmos/fuerza_bruta $tmp/uno.txt $tmp/b.out
if [ "$(head -1 $tmp/k.out)" = "costo 0" ] && [ "$(head -1 $tmp/b.out)" = "costo 0" ]; then
  echo "Un solo nodo: ambos dan costo 0"
else
  echo "Un solo nodo: FALLO"
  fallos=$((fallos + 1))
fi

rm -rf $tmp
[ $fallos -eq 0 ] && echo "TODO BIEN" || { echo "HAY FALLOS"; exit 1; }
