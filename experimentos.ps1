# Compila los programas, genera las instancias y ejecuta todos los experimentos con PowerShell en Windows.
# 
# Uso: ./ejecutar_experimentos.ps1
#
# Cada ejecución se repite varias veces: El número de instrucciones es siempre el mismo
# y el tiempo que se guarda es la mediana de las repeticiones.

# Se utiliza para mover a la carpeta donde está este archivo, para que las rutas funcionen desde cualquier lado.
Set-Location $PSScriptRoot

# Se utiliza para crear las carpetas de instancias y resultados si no existen.
New-Item -ItemType Directory -Force -Path instancias, resultados | Out-Null

# Se utilizan para compilar los tres programas con optimización (-O2) y C++17. Si alguno falla, el script se detiene.
g++ -O2 -std=c++17 -o algoritmos/kruskal.exe algoritmos/kruskal.cpp
if ($LASTEXITCODE -ne 0) { 
    exit 1 
}

g++ -O2 -std=c++17 -o algoritmos/fuerza_bruta.exe algoritmos/fuerza_bruta.cpp
if ($LASTEXITCODE -ne 0) { 
    exit 1 
}

g++ -O2 -std=c++17 -o generador/generador.exe generador/generador.cpp
if ($LASTEXITCODE -ne 0) { 
    exit 1 
}

# Crea el csv con la fila de los nombres de las columnas y si ya existía, lo reemplaza.
"algoritmo,n,m,instrucciones,tiempo_ns,costo,repeticiones" | Set-Content resultados/resultados.csv

# Es una función que lee un valor de un archivo de salida: Busca la línea que empieza con la clave y
# devuelve la palabra que viene después.
function Leer($archivo, $clave) {
    $linea = Get-Content $archivo | Where-Object { 
        $_ -like "$clave *" 
    } | Select-Object -First 1
    return ($linea -split " ")[1]
}

# Es una función que ejecuta un programa las veces indicadas sobre una instancia, guarda la mediana
# de los tiempos, y agrega al csv una fila con el algoritmo, la familia del grafo, n, m, las instrucciones,
# el tiempo, el costo y las repeticiones.
function Correr($programa, $familia, $n, $m, $entrada, $reps) {
    $salida = "resultados/$programa" + "_" + "$familia" + "_$n.out"
    $tiempos = @()
    for ($r = 0; $r -lt $reps; $r++) {
        & "./algoritmos/$programa.exe" $entrada $salida
        $tiempos += [int64](Leer $salida "tiempo_ns")
    }
    # Mediana (reps es impar)
    $ordenados = $tiempos | Sort-Object
    $mediana = $ordenados[[math]::Floor(($reps - 1) / 2)]
    $ins = Leer $salida "instrucciones"
    $costo = Leer $salida "costo"
    Add-Content resultados/resultados.csv "$programa-$familia,$n,$m,$ins,$mediana,$costo,$reps"
}

# Para cada n entre 4 y 10 genera el grafo completo, donde m es n(n-1)/2, con semilla 42 y corre Kruskal
# 15 veces. La fuerza bruta se corre menos veces mientras más crece n, porque tarda mucho más.
foreach ($n in 4..10) {
    $m = $n * ($n - 1) / 2
    $entrada = "instancias/completo_$n.txt"
    & ./generador/generador.exe $n $m 42 $entrada
    Correr "kruskal" "completo" $n $m $entrada 15
    if ($n -le 8) {
        $reps = 11 
    } elseif ($n -eq 9) {
        $reps = 5 
    } else {
        $reps = 3 
    }
    Correr "fuerza_bruta" "completo" $n $m $entrada $reps
}

# Para cada tamaño de la lista genera un grafo disperso con m igual a 3n y corre Kruskal 5 veces.
# Las instancias muy grandes se borran después de usarlas para no ocupar espacio.
# 
# Grafos grandes con m = 3n (semilla 7): solo Kruskal.
# Las instancias de más de 100000 nodos se borran por tamaño; se regeneran con la misma semilla.
foreach ($n in 1000, 2000, 5000, 10000, 20000, 50000, 100000, 200000, 500000, 1000000) {
    $m = 3 * $n
    $entrada = "instancias/disperso_$n.txt"
    & ./generador/generador.exe $n $m 7 $entrada
    Correr "kruskal" "disperso" $n $m $entrada 5
    if ($n -gt 100000) { 
        Remove-Item $entrada 
    }
}