# Compila los programas, genera las instancias y ejecuta todos los experimentos (PowerShell en Windows)
# Resultados: resultados/resultados.csv
# Cada ejecucion se repite varias veces: el numero de instrucciones es siempre el mismo
# y el tiempo que se guarda es la mediana de las repeticiones.
Set-Location $PSScriptRoot
New-Item -ItemType Directory -Force -Path instancias, resultados | Out-Null

g++ -O2 -std=c++17 -o algoritmos/kruskal.exe algoritmos/kruskal.cpp
if ($LASTEXITCODE -ne 0) { exit 1 }
g++ -O2 -std=c++17 -o algoritmos/fuerza_bruta.exe algoritmos/fuerza_bruta.cpp
if ($LASTEXITCODE -ne 0) { exit 1 }
g++ -O2 -std=c++17 -o generador/generador.exe generador/generador.cpp
if ($LASTEXITCODE -ne 0) { exit 1 }

"algoritmo,n,m,instrucciones,tiempo_ns,costo,repeticiones" | Set-Content resultados/resultados.csv

# Lee un valor del archivo de salida, por ejemplo instrucciones, tiempo_ns o costo
function Leer($archivo, $clave) {
    $linea = Get-Content $archivo | Where-Object { $_ -like "$clave *" } | Select-Object -First 1
    return ($linea -split " ")[1]
}

# Ejecuta un programa varias veces sobre una instancia y guarda una fila en el csv
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

# Grafos completos pequenos: se ejecutan los dos algoritmos
foreach ($n in 4..10) {
    $m = $n * ($n - 1) / 2
    $entrada = "instancias/completo_$n.txt"
    & ./generador/generador.exe $n $m 42 $entrada
    Correr "kruskal" "completo" $n $m $entrada 15
    if ($n -le 8) { $reps = 11 } elseif ($n -eq 9) { $reps = 5 } else { $reps = 3 }
    Correr "fuerza_bruta" "completo" $n $m $entrada $reps
}

# Grafos grandes con m = 3n (semilla 7): solo Kruskal.
# Las instancias de mas de 100000 nodos se borran por tamano; se regeneran con la misma semilla.
foreach ($n in 1000, 2000, 5000, 10000, 20000, 50000, 100000, 200000, 500000, 1000000) {
    $m = 3 * $n
    $entrada = "instancias/disperso_$n.txt"
    & ./generador/generador.exe $n $m 7 $entrada
    Correr "kruskal" "disperso" $n $m $entrada 5
    if ($n -gt 100000) { Remove-Item $entrada }
}
