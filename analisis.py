# Lee resultados.csv, calcula la constante C de cada algoritmo y crea las tablas,
# las graficas y el archivo constantes.tex que usa el informe.
# Se puede ejecutar desde cualquier carpeta.
import csv, math, os, statistics
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

os.chdir(os.path.dirname(os.path.abspath(__file__)))

with open("resultados/resultados.csv") as f:
    filas = list(csv.DictReader(f))

def sel(alg):
    return [r for r in filas if r["algoritmo"] == alg]

def constante(g, y):
    # C = mediana de los cocientes dato / f(n). Asi C*f(n) queda "en el medio" de los datos.
    return statistics.median(a / b for a, b in zip(y, g))

def error_max(g, y, c):
    # Error relativo maximo entre los datos y C*f(n)
    return max(abs(a - c * b) / (c * b) for a, b in zip(y, g))

def medibles(g, t):
    # Los tiempos menores a 1 ms casi solo miden ruido; no se usan para calcular C del tiempo
    pares = [(a, b) for a, b in zip(g, t) if b >= 1e6]
    return [a for a, _ in pares], [b for _, b in pares]

# ---------- Kruskal en grafos dispersos: f(m) = m log2(m) ----------
kd = sel("kruskal-disperso")
n_k = [int(r["n"]) for r in kd]
m_k = [int(r["m"]) for r in kd]
ops_k = [int(r["instrucciones"]) for r in kd]
t_k = [int(r["tiempo_ns"]) for r in kd]
f_k = [m * math.log2(m) for m in m_k]
c_ops_k = constante(f_k, ops_k)
fk, tk = medibles(f_k, t_k)
c_t_k = constante(fk, tk)

# ---------- Fuerza bruta en grafos completos: f(n) = n * C(m, n-1) ----------
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

print("Kruskal       C instrucciones:", round(c_ops_k, 2), " C tiempo:", round(c_t_k, 2))
print("Fuerza bruta  C instrucciones:", round(c_ops_b, 2), " C tiempo:", round(c_t_b, 2))

# ---------- Graficas ----------
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

grafica(m_k, ops_k, f_k, c_ops_k, "Kruskal: instrucciones", "Instrucciones",
        "graficas/kruskal_ops.pdf", "Numero de aristas m", True)
grafica(m_k, t_k, f_k, c_t_k, "Kruskal: tiempo", "Tiempo (ns)",
        "graficas/kruskal_tiempo.pdf", "Numero de aristas m", True)
grafica(n_b, ops_b, f_b, c_ops_b, "Fuerza bruta: instrucciones", "Instrucciones",
        "graficas/bruta_ops.pdf", "Numero de nodos n (grafo completo)", False)
grafica(n_b, t_b, f_b, c_t_b, "Fuerza bruta: tiempo", "Tiempo (ns)",
        "graficas/bruta_tiempo.pdf", "Numero de nodos n (grafo completo)", False)

# ---------- Tablas para el informe ----------
with open("informe/tabla_kruskal.tex", "w") as f:
    f.write("\\begin{tabular}{rrrr}\n\\toprule\n$n$ & $m$ & Instrucciones & Tiempo (ms)\\\\\n\\midrule\n")
    for n, m, o, t in zip(n_k, m_k, ops_k, t_k):
        f.write(f"{n} & {m} & {o} & {t/1e6:.3f}\\\\\n")
    f.write("\\bottomrule\n\\end{tabular}\n")
with open("informe/tabla_bruta.tex", "w") as f:
    f.write("\\begin{tabular}{rrrrr}\n\\toprule\n$n$ & $m$ & Instrucciones & Tiempo (ms) & Kruskal (instr.)\\\\\n\\midrule\n")
    for n, m, o, t in zip(n_b, m_b, ops_b, t_b):
        f.write(f"{n} & {m} & {o} & {t/1e6:.3f} & {kc[n]}\\\\\n")
    f.write("\\bottomrule\n\\end{tabular}\n")

# ---------- Numeros que cita el texto del informe ----------
def d(x, k=1):
    return f"{x:.{k}f}"
i9, i10 = n_b.index(9), n_b.index(10)
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
with open("informe/constantes.tex", "w") as f:
    for nombre, valor in macros.items():
        f.write(f"\\newcommand{{\\{nombre}}}{{{valor}}}\n")
