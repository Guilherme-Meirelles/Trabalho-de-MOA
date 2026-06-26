# Gera boxplots e tabelas-resumo a partir de Resultados/experimentos.csv.
# Uso:  python Resultados/plot_boxplots.py
# Saidas: Resultados/graficos/*.png  e  Resultados/resumo_experimentos.md
import csv
import os
import statistics as st
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt

AQUI = os.path.dirname(os.path.abspath(__file__))
CSV = os.path.join(AQUI, "experimentos.csv")
GRAF = os.path.join(AQUI, "graficos")
os.makedirs(GRAF, exist_ok=True)

ORDEM = ["Teste_01", "Teste_02", "Teste_03", "Teste_04", "Teste_05",
         "Wren_01", "Wren_02", "Wren_03", "Wren_04"]

linhas = []
with open(CSV, encoding="utf-8-sig") as f:
    for r in csv.DictReader(f, delimiter=";"):
        r["custo"] = float(r["custo"])
        r["gap_pct"] = float(r["gap_pct"])
        r["melhor_conhecido"] = float(r["melhor_conhecido"])
        r["semente"] = int(r["semente"])
        r["modo_bl"] = int(r["modo_bl"])
        r["pop"] = int(r["pop"])
        linhas.append(r)

def filtra(exp):
    return [r for r in linhas if r["experimento"] == exp]

# ---------- E1: boxplot de GAP por instancia ----------
e1 = filtra("E1")
insts = [i for i in ORDEM if any(r["instancia"] == i for r in e1)]
dados = [[r["gap_pct"] for r in e1 if r["instancia"] == i] for i in insts]

fig, ax = plt.subplots(figsize=(11, 5))
ax.boxplot(dados, labels=insts, showmeans=True)
ax.axhline(0, color="red", ls="--", lw=1, label="melhor conhecido (GAP=0)")
ax.set_ylabel("GAP (%)")
ax.set_title("Distribuicao do GAP por instancia (E1, %d sementes, busca local A+B)" % len(dados[0]))
ax.grid(axis="y", ls=":", alpha=0.6)
ax.legend()
plt.xticks(rotation=30)
plt.tight_layout()
plt.savefig(os.path.join(GRAF, "boxplot_gap.png"), dpi=130)
plt.close()

# ---------- E2: vizinhancas (modos 1=A, 2=B, 3=A+B) ----------
e2 = filtra("E2")
modo_nome = {1: "A (troca)", 2: "B (ruina)", 3: "A+B"}
insts2 = [i for i in ORDEM if any(r["instancia"] == i for r in e2)]
if e2:
    fig, ax = plt.subplots(figsize=(12, 5))
    pos = 0
    ticks, labels = [], []
    cores = {1: "#8ecae6", 2: "#ffb703", 3: "#90be6d"}
    for i in insts2:
        for m in (1, 2, 3):
            vals = [r["gap_pct"] for r in e2 if r["instancia"] == i and r["modo_bl"] == m]
            if not vals:
                continue
            pos += 1
            bp = ax.boxplot([vals], positions=[pos], widths=0.6, patch_artist=True)
            for b in bp["boxes"]:
                b.set_facecolor(cores[m])
        ticks.append(pos - 1)
        labels.append(i)
        pos += 1
    ax.set_ylabel("GAP (%)")
    ax.set_title("Estudo de vizinhancas: A vs B vs A+B (E2)")
    ax.set_xticks(ticks)
    ax.set_xticklabels(labels, rotation=30)
    handles = [plt.Rectangle((0, 0), 1, 1, fc=cores[m]) for m in (1, 2, 3)]
    ax.legend(handles, [modo_nome[m] for m in (1, 2, 3)])
    ax.grid(axis="y", ls=":", alpha=0.6)
    plt.tight_layout()
    plt.savefig(os.path.join(GRAF, "vizinhancas.png"), dpi=130)
    plt.close()

# ---------- Tabelas-resumo (markdown) ----------
def resumo(rows, chave):
    grupos = {}
    for r in rows:
        grupos.setdefault(r[chave], []).append(r)
    return grupos

out = []
out.append("# Resumo dos experimentos\n")
out.append("Gerado por `plot_boxplots.py` a partir de `experimentos.csv`.\n")

# E1 summary
out.append("## E1 - Melhores resultados e GAP (config calibrada, busca local A+B)\n")
out.append("| Instancia | Melhor conhec. | Melhor obtido | GAP melhor % | Custo medio | GAP medio % | Desvio (custo) | Tempo medio (ms) | Exec |")
out.append("|---|---|---|---|---|---|---|---|---|")
for i in insts:
    rs = [r for r in e1 if r["instancia"] == i]
    custos = [r["custo"] for r in rs]
    gaps = [r["gap_pct"] for r in rs]
    tempos = [float(r["tempo_ms"]) for r in rs]
    best = min(custos)
    bestgap = min(gaps)
    desvio = st.pstdev(custos) if len(custos) > 1 else 0.0
    out.append("| %s | %s | %.2f | %+.2f | %.2f | %+.2f | %.2f | %.0f | %d |" % (
        i, rs[0]["melhor_conhecido"], best, bestgap,
        st.mean(custos), st.mean(gaps), desvio, st.mean(tempos), len(rs)))
out.append("")

# E2 summary
if e2:
    out.append("## E2 - Influencia das vizinhancas (GAP medio %)\n")
    out.append("| Instancia | A (troca) | B (ruina) | A+B |")
    out.append("|---|---|---|---|")
    for i in insts2:
        cels = []
        for m in (1, 2, 3):
            vals = [r["gap_pct"] for r in e2 if r["instancia"] == i and r["modo_bl"] == m]
            cels.append(("%+.2f" % st.mean(vals)) if vals else "-")
        out.append("| %s | %s | %s | %s |" % (i, cels[0], cels[1], cels[2]))
    out.append("")

# E3 summary
e3 = filtra("E3")
if e3:
    out.append("## E3 - Influencia do tamanho da populacao (GAP medio %)\n")
    insts3 = [i for i in ORDEM if any(r["instancia"] == i for r in e3)]
    pops = sorted({r["pop"] for r in e3})
    out.append("| Instancia | " + " | ".join("pop=%d" % p for p in pops) + " |")
    out.append("|---" * (len(pops) + 1) + "|")
    for i in insts3:
        cels = []
        for p in pops:
            vals = [r["gap_pct"] for r in e3 if r["instancia"] == i and r["pop"] == p]
            cels.append(("%+.2f" % st.mean(vals)) if vals else "-")
        out.append("| %s | %s |" % (i, " | ".join(cels)))
    out.append("")

with open(os.path.join(AQUI, "resumo_experimentos.md"), "w", encoding="utf-8") as f:
    f.write("\n".join(out))

print("OK: graficos em", GRAF)
print("OK: resumo em", os.path.join(AQUI, "resumo_experimentos.md"))
