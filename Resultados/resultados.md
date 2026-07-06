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
- **Restart parcial ao estagnar:** após `MAX_ESTAGNACAO = 5000` iterações sem melhorar, o laço
  não encerra — preserva o elite e reconstrói o resto da população (GRASP + busca local),
  escapando do ótimo local e usando todo o orçamento de tempo.
- Parada: TEMPO_LIMITE configurável (argv[2], ms) e MAX_ITER = 50000 (rede de segurança).

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
Após adotar o `gerador` semeado (revisão do parceiro), as execuções passaram a ser
reprodutíveis; a variação entre execuções agora vem da **semente** passada por argumento.

---

## Melhoria: restart parcial ao estagnar (2026-07-04)

**Diagnóstico.** As observações da baseline mostraram que Wren_02/Wren_03 **paravam antes do
limite de tempo** por estagnação (`MAX_ESTAGNACAO`), travando em ótimo local e desperdiçando
~⅔ do orçamento. **Correção mínima** (3 linhas no `main`): ao estagnar, em vez de encerrar, o
laço faz um **restart parcial** — preserva o incumbente (elite) e reconstrói o resto da
população (GRASP + busca local), continuando até o tempo acabar.

**Evidência A/B** (5 sementes por instância, 30 s, config calibrada, A+B, **na tomada / clock
cheio**; dados em `ab_restart.csv`). A coluna de iterações explica o mecanismo: a versão antiga
para por estagnação com ~11–15 mil iterações, sobrando orçamento; o restart segue até o tempo
acabar (~38–48 mil), e é desse "mais tempo bem usado" que vem o ganho.

| Instância | Versão | Melhor GAP % | Média GAP % | Iter. média | Viável |
|---|---|---|---|---|---|
| Wren_02 | antes  | +2,77 | +5,14 | 11 558 | 5/5 |
| Wren_02 | **restart** | +2,77 | **+3,49** | 38 634 | 5/5 |
| Wren_03 | antes  | +5,19 | +7,87 | 15 007 | 5/5 |
| Wren_03 | **restart** | **+2,61** | **+4,17** | 48 531 | 5/5 |

- **Wren_03** (pior caso do trabalho): melhor GAP caiu de +5,19% → **+2,61%** (mais que a metade);
  média +7,87% → +4,17%.
- **Wren_02:** melhor empatou, média caiu ~1,65 pp (mais consistente).
- **Sem regressão** nas instâncias já boas — e o **Teste_02** (ponto fraco entre as pequenas,
  ia a +7,32% em algumas sementes) passou a **cravar o ótimo** (0,00%). Todas as execuções VIÁVEIS.

Com a mudança, o critério de parada principal passou a ser o **tempo limite**; a estagnação virou
gatilho de diversificação. A campanha E1/E2/E3 abaixo foi **regerada** com essa versão.

---

## Ferramentas de experimento

O executável aceita argumentos posicionais (todos opcionais; sem argumentos, comporta-se
como antes):

```
scp_run.exe <instancia> <tempo_ms> <semente> <func> <alpha> <pop> <modo_bl>
```
- `func` = 0 → roda a varredura (calibração) e escolhe func/alpha; `func` > 0 → usa a
  config calibrada passada e **pula a varredura** (muito mais rápido nas instâncias grandes).
- `modo_bl`: 1 = só vizinhança A (troca), 2 = só B (ruína), 3 = A+B.
- Cada execução imprime uma linha `RESULTADO;...` para coleta automática.

**Config calibrada por instância** (vencedores da varredura, determinísticos):

| Instância | func | alpha |   | Instância | func | alpha |
|---|---|---|---|---|---|---|
| Teste_01..05 | 3 | 0,10 |   | Wren_02 | 1 | 0,20 |
| Wren_01 | 2 | 0,30 |   | Wren_03 | 1 | 0,10 |
|  |  |  |   | Wren_04 | 3 | 0,10 |

**Scripts:**
- `Resultados/run_experimentos.ps1` — campanha completa, gera `experimentos.csv`:
  - **E1** boxplots de GAP (10 sementes × 9 instâncias, config calibrada, busca local A+B);
  - **E2** estudo de vizinhanças (modos A, B, A+B nas instâncias rápidas);
  - **E3** estudo de tamanho de população (20/50/100).
- `Resultados/plot_boxplots.py` — lê o CSV e gera `graficos/*.png` + `resumo_experimentos.md`.

Reproduzir:
```
g++ -std=c++17 -O2 algoritmo_genetico.cpp -o scp_run.exe
powershell -ExecutionPolicy Bypass -File Resultados\run_experimentos.ps1
python Resultados\plot_boxplots.py
```

## Resultados da campanha (E1/E2/E3)

Tabelas completas em `resumo_experimentos.md`; gráficos em `graficos/boxplot_gap.png`
(E1) e `graficos/vizinhancas.png` (E2). Campanha de **234 execuções, 0 inviáveis**
(validador independente) — o reparo do bug de cobertura se mantém em todas as configurações.

> Campanha regerada **na tomada** (clock cheio). A 1ª campanha rodou na bateria e sofreu
> throttling térmico (~¼ das iterações → GAPs das Wren artificialmente altos); os números
> abaixo são os da versão em clock cheio, coerentes com o A/B controlado.

### Principais achados

**E1 — qualidade e GAP (10 sementes, config calibrada, A+B, com restart parcial):**
- Superam ou empatam o melhor conhecido em **7 das 9 instâncias**: **Teste_03, Teste_04,
  Teste_05, Wren_04** (sempre abaixo — Wren_04 chega a −0,50%), **Teste_01** (crava o ótimo nas
  10 sementes), **Teste_02** e **Wren_01** (ótimo na melhor semente).
- Instâncias difíceis: **Wren_02** (melhor +2,77%) e **Wren_03** (melhor +2,61%) — ainda acima,
  mas agora **ambas abaixo de +3%** (antes do restart, Wren_03 ia a +5,19%).
- **Efeito do restart parcial** (vs versão que parava por estagnação): melhor GAP de Wren_03
  +5,19% → +2,61% (média +8,00% → +4,74%); médias caíram em quase tudo (Teste_02 +4,01% →
  +0,62%; Teste_05 +0,23% → −1,84%; Wren_01 +3,28% → +1,86%; Wren_02 +5,74% → +3,81%). Nenhuma
  regressão; todas VIÁVEIS.
- Variância (importância dos boxplots): Teste_01/03/04 quase determinísticos (desvio ~0);
  Wren_02/03 com dispersão maior → uma única execução engana.

**E2 — estudo de vizinhanças (achado central):**
- A vizinhança **B (ruína-reconstrução guiada por desperdício — nossa contribuição original)
  é a mais forte** isoladamente (ex.: Teste_05 chega a −2,67% só com B).
- **A+B nem sempre supera B sozinha** — em Teste_02 e Teste_05 B isolada é melhor.
- **A (troca) sozinha é a mais fraca.** Conclusão para o relatório: o motor de melhoria é a
  ruína-reconstrução; a troca é complemento de intensificação, não a peça principal.

**E3 — tamanho da população:**
- Wren_01 melhora forte com população maior (GAP médio +3,03% → +0,07% de pop 20 → 100).
- Teste_02 também melhora com pop maior (+1,55% → +0,74%).
