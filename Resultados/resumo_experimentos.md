# Resumo dos experimentos

Gerado por `plot_boxplots.py` a partir de `experimentos.csv`.

## E1 - Melhores resultados e GAP (config calibrada, busca local A+B)

| Instancia | Melhor conhec. | Melhor obtido | GAP melhor % | Custo medio | GAP medio % | Desvio (custo) | Tempo medio (ms) | Exec |
|---|---|---|---|---|---|---|---|---|
| Teste_01 | 557.44 | 557.44 | +0.00 | 557.44 | +0.00 | 0.00 | 3606 | 10 |
| Teste_02 | 537.89 | 537.89 | +0.00 | 541.24 | +0.62 | 4.31 | 4618 | 10 |
| Teste_03 | 517.58 | 515.30 | -0.44 | 515.30 | -0.44 | 0.00 | 5000 | 10 |
| Teste_04 | 1162.8 | 1157.08 | -0.49 | 1157.08 | -0.49 | 0.00 | 5000 | 10 |
| Teste_05 | 1020.12 | 990.52 | -2.90 | 1001.30 | -1.84 | 11.37 | 8000 | 10 |
| Wren_01 | 7856.0 | 7856.00 | +0.00 | 8002.40 | +1.86 | 139.72 | 5731 | 10 |
| Wren_02 | 13908.0 | 14293.00 | +2.77 | 14437.40 | +3.81 | 112.12 | 30000 | 10 |
| Wren_03 | 13780.0 | 14139.00 | +2.61 | 14433.30 | +4.74 | 228.31 | 30000 | 10 |
| Wren_04 | 58161.0 | 57868.00 | -0.50 | 58394.50 | +0.40 | 320.17 | 60000 | 10 |

## E2 - Influencia das vizinhancas (GAP medio %)

| Instancia | A (troca) | B (ruina) | A+B |
|---|---|---|---|
| Teste_01 | +0.00 | +0.00 | +0.00 |
| Teste_02 | +1.75 | +0.37 | +1.04 |
| Teste_03 | -0.37 | -0.44 | -0.44 |
| Teste_04 | -0.49 | -0.49 | -0.49 |
| Teste_05 | +1.01 | -2.67 | -1.74 |
| Wren_01 | +3.03 | +1.32 | +1.25 |
| Wren_02 | +3.68 | +4.02 | +3.76 |
| Wren_03 | +5.24 | +5.05 | +4.38 |
| Wren_04 | +3.01 | -0.48 | +0.51 |

## E3 - Influencia do tamanho da populacao (GAP medio %)

| Instancia | pop=20 | pop=50 | pop=100 |
|---|---|---|---|
| Teste_02 | +1.55 | +1.04 | +0.74 |
| Wren_01 | +3.03 | +1.25 | +0.07 |
