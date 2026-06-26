# Resultados dos experimentos — SCP (AG + Busca Local)

Log de execuções para alimentar o relatório (tabela de melhores resultados, tempos e
boxplots de GAP). Cada execução registra o custo obtido, a configuração usada, o tempo,
e a **validação independente** (recálculo de cobertura/custo do zero).

GAP = (custo_obtido − melhor_conhecido) / melhor_conhecido × 100. Valor negativo = superou
a referência do professor.

## Melhores valores conhecidos (referência do professor)

| Instância | Linhas | Colunas | Melhor conhecido |
|---|---|---|---|
| Teste_01 | 50  | 300  | 557,44 |
| Teste_02 | 50  | 500  | 537,89 |
| Teste_03 | 50  | 700  | 517,58 |
| Teste_04 | 100 | 500  | 1162,8 |
| Teste_05 | 100 | 700  | 1020,12 |
| Wren_01  | 200 | 539  | 7856 |
| Wren_02  | 222 | 5522 | 13908 |
| Wren_03  | 219 | 4990 | 13780 |
| Wren_04  | 798 | 4569 | 58161 |

## Configuração fixa do algoritmo

**Construção (população inicial):**
- Varredura de funções gulosas 1..7 × alpha {0,10; 0,20; 0,30}, 50 repetições cada (GRASP),
  escolhendo automaticamente a melhor combinação `func_global`/`alpha_global` por instância.
- Eliminação de redundância após cada construção.
- População alvo: 50 soluções distintas (máx. 1000 tentativas).
- Semente da construção: fixa (SEMENTE = 12345) → população inicial reprodutível.

**Busca local (memética, Lamarckiana):**
- Vizinhança A — troca (1,1) guiada por cobertura (substitui coluna por outra mais barata
  que cobre suas linhas exclusivas; remove redundantes). First-improvement até estabilizar.
- Vizinhança B — ruína e reconstrução guiada por desperdício
  (desperdício(c) = peso(c) × média(cobertura−1)); RODADAS_RUINA = 5.

**Algoritmo Genético (steady-state):**
- Seleção por roleta (favorece menor custo); cruzamento por fusão (Beasley & Chu) + reparo guloso.
- Mutação ~10% (dispara quando sorteio 0..10 == 9), constante 4.
- Substituição de um indivíduo da metade pior + rejeição de duplicatas (elitismo implícito).
- Parada: MAX_ITER = 50000, MAX_ESTAGNACAO = 5000, TEMPO_LIMITE configurável (argv[2], ms).

**Aleatoriedade:** a construção usa semente fixa, mas seleção e mutação usam `random_device`
(NÃO reprodutível) — por isso execuções únicas variam. Para boxplots, rodar N sementes.

---

## Execução baseline (2026-06-26)

Compilado com `g++ -std=c++17 -O2`. Testes a 10 s, Wren a 60 s. Todas VIÁVEIS (validador independente).

| Instância | Custo | Melhor conhecido | GAP % | Nº col | func/alpha | Iterações | Tempo (ms) | Limite (ms) | Validação |
|---|---|---|---|---|---|---|---|---|---|
| Teste_01 | 557,44   | 557,44  | 0,00  | 9   | 3 / 0,10 | 13445 | 1608  | 10000 | 50/50 |
| Teste_02 | 577,25   | 537,89  | +7,32 | 10  | 3 / 0,10 | 5608  | 1300  | 10000 | 50/50 |
| Teste_03 | 515,30   | 517,58  | −0,44 | 9   | 3 / 0,10 | 5000  | 1454  | 10000 | 50/50 |
| Teste_04 | 1157,08  | 1162,8  | −0,49 | 19  | 3 / 0,10 | 7738  | 2224  | 10000 | 100/100 |
| Teste_05 | 1018,67  | 1020,12 | −0,14 | 19  | 3 / 0,10 | 16666 | 9476  | 10000 | 100/100 |
| Wren_01  | 7873     | 7856    | +0,22 | 15  | 2 / 0,30 | 9558  | 2523  | 60000 | 200/200 |
| Wren_02  | 14979    | 13908   | +7,70 | 37  | 1 / 0,20 | 6156  | 25008 | 60000 | 222/222 |
| Wren_03  | 14844    | 13780   | +7,72 | 35  | 1 / 0,10 | 13695 | 39356 | 60000 | 219/219 |
| Wren_04  | 58939    | 58161   | +1,34 | 122 | 3 / 0,10 | 4412  | 60005 | 60000 | 798/798 |

### Observações da baseline
- **Pequenas (Teste):** ótimas. Teste_01 atinge o ótimo; Teste_03/04/05 superam a referência. Teste_02 é o ponto fraco (+7,32%).
- **Wren_01 e Wren_04:** quase no ótimo quando recebem tempo (+0,22% e +1,34%).
- **Wren_02 e Wren_03:** pararam ANTES do limite de 60 s (25 s e 39 s) por **estagnação** → travam em ótimo local ~+7,7%. Mais tempo não ajuda; precisam de mais diversificação.
- Variância entre execuções (random_device): ver tabela de sensibilidade ao tempo abaixo.

### Sensibilidade ao tempo limite — Wren_04
| Tempo limite | Custo | GAP % | Iterações |
|---|---|---|---|
| 10 s  | 63112 | +8,51 | 0 (construção consumiu o orçamento) |
| 60 s  | 58393 | +0,40 | 6046 |
| 120 s | 59091 | +1,60 | 12023 |

Nota: 120 s deu pior que 60 s por variância (sem semente fixa), não por o tempo prejudicar.
