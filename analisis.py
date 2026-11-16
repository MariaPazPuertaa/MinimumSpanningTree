# Lee resultados.csv, calcula la constante C de cada algoritmo y crea las tablas,
# las gráficas y el archivo constantes.tex que usa el informe.
# 
# Se puede ejecutar desde cualquier carpeta.
#
# La constante C se calcula comparando las instrucciones y el tiempo contra la función
# de complejidad teórica de cada algoritmo: m log2(m) para Kruskal y n * C(m, n-1) para fuerza bruta.

# Se utilizan para leer el csv, hacer cuentas log2 y combinaciones, manejar rutas y sacar la mediana.
import csv, math, os, statistics

# Se utiliza para hacer las gráficas; con "Agg" se guardan como archivos sin abrir ventanas.
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

# Se mueve a la carpeta donde está este archivo, para que las rutas funcionen desde cualquier lado.
os.chdir(os.path.dirname(os.path.abspath(__file__)))

# Lee el csv y guarda cada fila como un diccionario, con los nombres de las columnas como llaves.
with open("resultados/resultados.csv") as f:
    filas = list(csv.DictReader(f))

# Es una función que devuelve solo las filas del algoritmo que se pide, por ejemplo "kruskal-disperso".
def sel(alg):
    return [r for r in filas if r["algoritmo"] == alg]

# Es una función que calcula la constante C de un algoritmo: Donde g son los valores de f(n) e y son los datos medidos.
def constante(g, y):
    # C = mediana de los cocientes dato / f(n). Así C*f(n) queda en el medio de los datos.
    return statistics.median(a / b for a, b in zip(y, g))

# Es una función que calcula qué tan lejos queda C*f(n) de los datos, como el mayor error relativo.
def error_max(g, y, c):
    # Error relativo máximo entre los datos y C*f(n)
    return max(abs(a - c * b) / (c * b) for a, b in zip(y, g))

# Es una función que se queda solo con los tiempos mayores o iguales a 1 ms, junto con su f(n).
def medibles(g, t):
    # Los tiempos menores a 1 ms casi solo miden ruido; no se usan para calcular la C del tiempo.
    pares = [(a, b) for a, b in zip(g, t) if b >= 1e6]
    return [a for a, _ in pares], [b for _, b in pares]

# Kruskal en grafos dispersos: f(m) = m log2(m)
# Se separan las columnas del csv en listas, se calcula f(m) de cada dato y con eso las constantes C
# de instrucciones y de tiempo medible.
kd = sel("kruskal-disperso")
n_k = [int(r["n"]) for r in kd]
m_k = [int(r["m"]) for r in kd]
ops_k = [int(r["instrucciones"]) for r in kd]
t_k = [int(r["tiempo_ns"]) for r in kd]
f_k = [m * math.log2(m) for m in m_k]
c_ops_k = constante(f_k, ops_k)
fk, tk = medibles(f_k, t_k)
c_t_k = constante(fk, tk)

# Fuerza bruta en grafos completos: f(n) = n * C(m, n-1).
# Se hace lo mismo que con Kruskal, pero con f(n) igual al número de subconjuntos de n-1 aristas por n.
# Además, kc guarda las instrucciones de Kruskal en grafos completos por n para compararlas en la tabla.
bf = sel("fuerza_bruta-completo")
kc = {int(r["n"]): int(r["instrucciones"]) for r in sel("kruskal-completo")}
n_b = [int(r["n"]) for r in bf]
m_b = [int(r["m"]) for r in bf]
ops_b = [int(r["instrucciones"]) for r in bf]
t_b = [int(r["tiempo_ns"]) for r in bf]
f_b = [n * math.comb(m, n - 1) for n, m in zip(n_b, m_b)]
c_ops_b = constante(f_b, ops_b)
fb, tb = medibles(f_b, t_b)
c_t_b = constante(fb, tb)

# Muestra en pantalla las constantes de cada algoritmo.
print("Kruskal       C instrucciones:", round(c_ops_k, 2), " C tiempo:", round(c_t_k, 2))
print("Fuerza bruta  C instrucciones:", round(c_ops_b, 2), " C tiempo:", round(c_t_b, 2))

# Gráficas
# Es una función que crea una gráfica con los datos y la curva C*f(n), con el eje y en escala
# logarítmica, y la guarda en un archivo. En este caso, x_log indica si el eje x también va en escala logarítmica.
def grafica(x, datos, f, c, titulo, ylabel, archivo, xlabel, x_log):
    plt.figure(figsize=(6, 4))
    plt.plot(x, datos, "o", label="Datos")
    plt.plot(x, [c * v for v in f], "-", label="C f(n)  con C = %.2f" % c)
    plt.yscale("log")
    if x_log:
        plt.xscale("log")
    else:
        plt.xticks(x)
    plt.xlabel(xlabel); plt.ylabel(ylabel); plt.title(titulo)
    plt.legend(); plt.grid(True, which="both", alpha=0.3)
    plt.tight_layout(); plt.savefig(archivo); plt.close()

# Se crean las cuatro gráficas: instrucciones y tiempo de Kruskal, e instrucciones y tiempo de fuerza bruta.
grafica(m_k, ops_k, f_k, c_ops_k, "Kruskal: instrucciones", "Instrucciones",
        "graficas/kruskal_ops.pdf", "Numero de aristas m", True)
grafica(m_k, t_k, f_k, c_t_k, "Kruskal: tiempo", "Tiempo (ns)",
        "graficas/kruskal_tiempo.pdf", "Numero de aristas m", True)
grafica(n_b, ops_b, f_b, c_ops_b, "Fuerza bruta: instrucciones", "Instrucciones",
        "graficas/bruta_ops.pdf", "Numero de nodos n (grafo completo)", False)
grafica(n_b, t_b, f_b, c_t_b, "Fuerza bruta: tiempo", "Tiempo (ns)",
        "graficas/bruta_tiempo.pdf", "Numero de nodos n (grafo completo)", False)

# Tablas para el informe
# Escribe la tabla de Kruskal en formato LaTeX: n, m, instrucciones y tiempo en milisegundos.
with open("informe/tabla_kruskal.tex", "w") as f:
    f.write("\\begin{tabular}{rrrr}\n\\toprule\n$n$ & $m$ & Instrucciones & Tiempo (ms)\\\\\n\\midrule\n")
    for n, m, o, t in zip(n_k, m_k, ops_k, t_k):
        f.write(f"{n} & {m} & {o} & {t/1e6:.3f}\\\\\n")
    f.write("\\bottomrule\n\\end{tabular}\n")

# Escribe la tabla de fuerza bruta en formato LaTeX, con una columna extra con las instrucciones de Kruskal.
with open("informe/tabla_bruta.tex", "w") as f:
    f.write("\\begin{tabular}{rrrrr}\n\\toprule\n$n$ & $m$ & Instrucciones & Tiempo (ms) & Kruskal (instr.)\\\\\n\\midrule\n")
    for n, m, o, t in zip(n_b, m_b, ops_b, t_b):
        f.write(f"{n} & {m} & {o} & {t/1e6:.3f} & {kc[n]}\\\\\n")
    f.write("\\bottomrule\n\\end{tabular}\n")

# Números que cita el texto del informe.
# Es una función que convierte un número en texto con k decimales.
def d(x, k=1):
    return f"{x:.{k}f}"

# Son las posiciones de los datos de n = 9 y n = 10 en fuerza bruta, para comparar sus tiempos.
i9, i10 = n_b.index(9), n_b.index(10)

# Son los números que se citan en el texto del informe: Las constantes C, sus errores máximos en
# porcentaje, los tamaños y tiempos máximos de Kruskal y los tiempos de fuerza bruta con n = 10.
macros = {
    "cOpsK": d(c_ops_k, 2), "cTK": d(c_t_k, 2),
    "cOpsB": d(c_ops_b, 2), "cTB": d(c_t_b, 2),
    "errOpsK": d(100 * error_max(f_k, ops_k, c_ops_k), 0),
    "errTK": d(100 * error_max(fk, tk, c_t_k), 0),
    "errOpsB": d(100 * error_max(f_b, ops_b, c_ops_b), 0),
    "errTB": d(100 * error_max(fb, tb, c_t_b), 0),
    "nKruskalMax": str(n_k[-1]), "mKruskalMax": str(m_k[-1]),
    "tKruskalMax": d(t_k[-1] / 1e9, 2),
    "tBrutaDiez": d(t_b[i10] / 1e9, 1),
    "factorNueveDiez": d(t_b[i10] / t_b[i9], 1),
}

# Escribe cada número como un comando de LaTeX (\newcommand) para usarlo directo en el texto del informe.
with open("informe/constantes.tex", "w") as f:
    for nombre, valor in macros.items():
        f.write(f"\\newcommand{{\\{nombre}}}{{{valor}}}\n")
