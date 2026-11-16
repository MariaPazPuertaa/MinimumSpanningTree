#!/bin/bash
# Compila, genera instancias y ejecuta los experimentos. Resultados en resultados/resultados.csv
# Cada ejecución se repite varias veces: El número de instrucciones es siempre el mismo
# y el tiempo que se guarda es la mediana de las repeticiones.

# Se utiliza para moverse a la carpeta donde está este archivo, para que las rutas funcionen desde cualquier lado.
cd "$(dirname "$0")"

# Se utilizan para compilar los tres programas con optimización (-O2) y C++17 y si alguno falla, el script se detiene.
g++ -O2 -std=c++17 -o algoritmos/kruskal algoritmos/kruskal.cpp || exit 1
g++ -O2 -std=c++17 -o algoritmos/fuerza_bruta algoritmos/fuerza_bruta.cpp || exit 1
g++ -O2 -std=c++17 -o generador/generador generador/generador.cpp || exit 1

# Se utiliza para crear las carpetas de instancias y resultados si no existen.
mkdir -p instancias resultados

# Crea el csv con la fila de los nombres de las columnas y si ya existía, lo reemplaza.
echo "algoritmo,n,m,instrucciones,tiempo_ns,costo,repeticiones" > resultados/resultados.csv

# Es una función que ejecuta un programa las veces indicadas sobre una instancia, guarda la mediana
# de los tiempos, y agrega al csv una fila con el algoritmo, la familia del grafo, n, m, las instrucciones,
# el tiempo, el costo y las repeticiones.
correr() {
  local alg=$1 fam=$2 n=$3 m=$4 entrada=$5 reps=$6
  local salida=resultados/${alg}_${fam}_$n.out
  local tiempos=()
  for ((r = 0; r < reps; r++)); do
    algoritmos/$alg "$entrada" "$salida" || exit 1
    tiempos+=("$(grep '^tiempo_ns' "$salida" | cut -d' ' -f2)")
  done
  # Mediana (reps es impar)
  local mediana
  mediana=$(printf '%s\n' "${tiempos[@]}" | sort -n | awk '{a[NR]=$1} END{print a[int((NR+1)/2)]}')
  local ins costo
  ins=$(grep '^instrucciones' "$salida" | cut -d' ' -f2)
  costo=$(grep '^costo' "$salida" | cut -d' ' -f2)
  echo "$alg-$fam,$n,$m,$ins,$mediana,$costo,$reps" >> resultados/resultados.csv
}

# Para cada n entre 4 y 10 genera el grafo completo, donde m es n(n-1)/2, con semilla 42 y corre Kruskal
# 15 veces. La fuerza bruta se corre menos veces mientras más crece n, porque tarda mucho más.
for n in 4 5 6 7 8 9 10; do
  m=$((n*(n-1)/2))
  generador/generador $n $m 42 instancias/completo_$n.txt || exit 1
  correr kruskal completo $n $m instancias/completo_$n.txt 15
  if [ $n -le 8 ]; then reps=11; elif [ $n -eq 9 ]; then reps=5; else reps=3; fi
  correr fuerza_bruta completo $n $m instancias/completo_$n.txt $reps
done

# Para cada tamaño de la lista genera un grafo disperso con m igual a 3n y corre Kruskal 5 veces.
# Las instancias muy grandes se borran después de usarlas para no ocupar espacio.
#
# Grafos grandes con m = 3n (semilla 7): solo Kruskal.
# Las instancias de más de 100000 nodos se borran por tamaño; se regeneran con la misma semilla.
for n in 1000 2000 5000 10000 20000 50000 100000 200000 500000 1000000; do
  m=$((3*n))
  generador/generador $n $m 7 instancias/disperso_$n.txt || exit 1
  correr kruskal disperso $n $m instancias/disperso_$n.txt 5
  if [ $n -gt 100000 ]; then rm instancias/disperso_$n.txt; fi
done
