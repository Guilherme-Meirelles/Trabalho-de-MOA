# Resumo dos experimentos

Gerado por `plot_boxplots.py` a partir de `experimentos.csv`.

## E1 - Melhores resultados e GAP (config calibrada, busca local A+B)

| Instancia | Melhor conhec. | Melhor obtido | GAP melhor % | Custo medio | GAP medio % | Desvio (custo) | Tempo medio (ms) | Exec |
|---|---|---|---|---|---|---|---|---|
| Teste_01 | 557.44 | 557.44 | +0.00 | 568.64 | +2.01 | 17.10 | 514 | 10 |
| Teste_02 | 537.89 | 544.60 | +1.25 | 559.48 | +4.01 | 14.73 | 726 | 10 |
| Teste_03 | 517.58 | 515.30 | -0.44 | 515.52 | -0.40 | 0.67 | 1166 | 10 |
| Teste_04 | 1162.8 | 1157.08 | -0.49 | 1157.08 | -0.49 | 0.00 | 1246 | 10 |
| Teste_05 | 1020.12 | 1015.51 | -0.45 | 1022.49 | +0.23 | 6.21 | 1795 | 10 |
| Wren_01 | 7856.0 | 7856.00 | +0.00 | 8113.40 | +3.28 | 85.80 | 627 | 10 |
| Wren_02 | 13908.0 | 14293.00 | +2.77 | 14685.30 | +5.59 | 211.47 | 8902 | 10 |
| Wren_03 | 13780.0 | 14495.00 | +5.19 | 14858.90 | +7.83 | 292.97 | 9848 | 10 |
| Wren_04 | 58161.0 | 57868.00 | -0.50 | 58409.80 | +0.43 | 345.08 | 43148 | 10 |

## E2 - Influencia das vizinhancas (GAP medio %)

| Instancia | A (troca) | B (ruina) | A+B |
|---|---|---|---|
| Teste_01 | +3.38 | +1.13 | +2.23 |
| Teste_02 | +4.17 | +2.14 | +5.50 |
| Teste_03 | -0.15 | -0.08 | -0.37 |
| Teste_04 | +0.82 | -0.49 | -0.49 |
| Teste_05 | +2.76 | -0.52 | +0.36 |
| Wren_01 | +3.03 | +3.64 | +3.03 |

## E3 - Influencia do tamanho da populacao (GAP medio %)

| Instancia | pop=20 | pop=50 | pop=100 |
|---|---|---|---|
| Teste_02 | +4.67 | +5.50 | +4.39 |
| Wren_01 | +3.64 | +3.03 | +2.43 |
